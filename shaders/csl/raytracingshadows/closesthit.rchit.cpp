#include <csl/csl.h>

using namespace csl;

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

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::ray_payload(2)]] bool              shadowed;
[[csl::hit_attribute]] vector<f32, 2>     attributes;

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
      const f32            dot_product  = max(dot(light_vector, normal), 0.2f);
      hit_value                         = color0 * dot_product;

      const f32            t_min  = 0.001f;
      const f32            t_max  = 10000.0f;
      const vector<f32, 3> origin = world_ray_origin() + world_ray_direction() * ray_t_max();

      shadowed = true;
      trace_ray(resources_.top_level, RayFlags::TerminateOnFirstHit | RayFlags::Opaque | RayFlags::SkipClosestHitShader, 0xff, 0, 0, 1, origin, t_min, light_vector, t_max, &shadowed);
      if (shadowed)
            hit_value = hit_value * 0.3f;
}
