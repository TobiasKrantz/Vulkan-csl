#include <csl/csl.h>

using namespace csl;

struct RayPayload
{
      vector<f32, 3> color;
      f32            distance;
      vector<f32, 3> normal;
      f32            reflector;
};

struct Ubo
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      vector<f32, 4>    light_position;
      i32               vertex_size;
};

struct Resources
{
      [[csl::binding(0, 0)]] AccelerationStructure top_level;
      [[csl::uniform_buffer(0, 2)]] const Ubo*     ubo;
      [[csl::binding(0, 3)]] vector<f32, 4>*       vertices;
      [[csl::binding(0, 4)]] u32*                  indices;
};

[[csl::ray_payload_in(0)]] RayPayload  ray_payload;
[[csl::hit_attribute]] vector<f32, 2>  attributes;

static vector<f32, 3> unpack_normal(vector<f32, 4> d0, vector<f32, 4> d1)
{
      return vector<f32, 3>{ d0.w, d1.x, d1.y };
}

[[csl::closest_hit]] void closest_hit_main()
{
      const Resources resources_ = resources<Resources>();

      const u32 primitive = primitive_id();
      const i32 i0        = i32(resources_.indices[3 * primitive + 0]);
      const i32 i1        = i32(resources_.indices[3 * primitive + 1]);
      const i32 i2        = i32(resources_.indices[3 * primitive + 2]);

      const i32            m       = resources_.ubo->vertex_size / 16;
      const vector<f32, 3> normal0 = unpack_normal(resources_.vertices[m * i0 + 0], resources_.vertices[m * i0 + 1]);
      const vector<f32, 3> normal1 = unpack_normal(resources_.vertices[m * i1 + 0], resources_.vertices[m * i1 + 1]);
      const vector<f32, 3> normal2 = unpack_normal(resources_.vertices[m * i2 + 0], resources_.vertices[m * i2 + 1]);
      const vector<f32, 3> color0  = resources_.vertices[m * i0 + 2].xyz;

      const vector<f32, 3> barycentrics = vector<f32, 3>{ 1.0f - attributes.x - attributes.y, attributes.x, attributes.y };
      const vector<f32, 3> normal       = normalize(normal0 * barycentrics.x + normal1 * barycentrics.y + normal2 * barycentrics.z);

      const vector<f32, 3> light_vector = normalize(resources_.ubo->light_position.xyz);
      const f32            dot_product  = max(dot(light_vector, normal), 0.6f);

      ray_payload.color     = color0 * dot_product;
      ray_payload.distance  = ray_t_max();
      ray_payload.normal    = normal;
      ray_payload.reflector = (color0.x == 1.0f && color0.y == 1.0f && color0.z == 1.0f) ? 1.0f : 0.0f;
}
