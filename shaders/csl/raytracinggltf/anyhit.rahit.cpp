#include <csl/csl.h>

using namespace csl;

struct GeometryNode
{
      u64 vertex_buffer_address;
      u64 index_buffer_address;
      i32 texture_index_base_color;
      i32 texture_index_occlusion;
};

struct Vertex
{
      vector<f32, 3> position;
      vector<f32, 3> normal;
      vector<f32, 2> uv;
};

struct Triangle
{
      Vertex         vertices[3];
      vector<f32, 3> normal;
      vector<f32, 2> uv;
};

struct Resources
{
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D> image;
      [[csl::binding(0, 4)]] GeometryNode*                          geometry_nodes;
      [[csl::binding(0, 5)]] CombinedSampler<f32, ImageType::IMAGE_2D> textures[];
};

[[csl::ray_payload_in(3)]] u32        payload_seed;
[[csl::hit_attribute]] vector<f32, 2> attribs;

static u32 lcg(u32& previous)
{
      previous = 1664525u * previous + 1013904223u;
      return previous & 0x00FFFFFFu;
}

static f32 rnd(u32& previous)
{
      return f32(lcg(previous)) / f32(0x01000000u);
}

static Triangle unpack_triangle(u32 index, GeometryNode geometry_node)
{
      Triangle  triangle;
      const u32 triangle_index = index * 3u;

      const u32*            indices  = buffer_pointer<u32>(geometry_node.index_buffer_address);
      const vector<f32, 4>* vertices = buffer_pointer<vector<f32, 4>>(geometry_node.vertex_buffer_address);

      for (u32 i = 0u; i < 3u; i++)
      {
            const u32            offset = indices[triangle_index + i] * 6u;
            const vector<f32, 4> d0     = vertices[offset + 0u];
            const vector<f32, 4> d1     = vertices[offset + 1u];
            triangle.vertices[i].position = d0.xyz;
            triangle.vertices[i].normal   = vector<f32, 3>{ d0.w, d1.x, d1.y };
            triangle.vertices[i].uv       = d1.zw;
      }

      const vector<f32, 3> barycentrics = vector<f32, 3>{ 1.0f - attribs.x - attribs.y, attribs.x, attribs.y };
      triangle.uv     = triangle.vertices[0].uv * barycentrics.x + triangle.vertices[1].uv * barycentrics.y + triangle.vertices[2].uv * barycentrics.z;
      triangle.normal = triangle.vertices[0].normal * barycentrics.x + triangle.vertices[1].normal * barycentrics.y + triangle.vertices[2].normal * barycentrics.z;
      return triangle;
}

[[csl::any_hit]] void any_hit_main()
{
      const Resources    resources_    = resources<Resources>();
      const GeometryNode geometry_node = resources_.geometry_nodes[ray_geometry_index()];
      const Triangle     triangle      = unpack_triangle(primitive_id(), geometry_node);

      const vector<f32, 4> color = resources_.textures[nonuniform(u32(geometry_node.texture_index_base_color))].sample_lod(triangle.uv, 0.0f);
      if (color.w < 0.9f)
            if (rnd(payload_seed) > color.w)
                  ignore_intersection();
}
