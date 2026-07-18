#include <csl/csl.h>

using namespace csl;

struct BufferReferences
{
      u64 vertices;
      u64 indices;
};

struct Resources
{
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D> image;
};

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::hit_attribute]] vector<f32, 2>     attribs;

static vector<f32, 2> triangle_uv(u32 index, u32 vertex_size, u64 vertices_address, u64 indices_address)
{
      const u32              triangle = index * 3;
      const u32*             indices  = buffer_pointer<u32>(indices_address);
      const vector<f32, 4>*  vertices = buffer_pointer<vector<f32, 4>>(vertices_address);
      const u32              stride   = vertex_size / 16u;

      const vector<f32, 2> uv0 = vertices[indices[triangle + 0] * stride + 1].zw;
      const vector<f32, 2> uv1 = vertices[indices[triangle + 1] * stride + 1].zw;
      const vector<f32, 2> uv2 = vertices[indices[triangle + 2] * stride + 1].zw;

      const vector<f32, 3> barycentrics = vector<f32, 3>{ 1.0f - attribs.x - attribs.y, attribs.x, attribs.y };
      return uv0 * barycentrics.x + uv1 * barycentrics.y + uv2 * barycentrics.z;
}

[[csl::closest_hit]] void closest_hit_main(BufferReferences push)
{
      const Resources resources_ = resources<Resources>();

      const vector<f32, 2> uv = triangle_uv(primitive_id(), 32u, push.vertices, push.indices);
      hit_value               = resources_.image.sample_lod(uv, 0.0f).xyz;
}
