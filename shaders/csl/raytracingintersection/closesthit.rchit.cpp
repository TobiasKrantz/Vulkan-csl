#include <csl/csl.h>

using namespace csl;

struct Sphere
{
      vector<f32, 3> center;
      f32            radius;
      vector<f32, 4> color;
};

struct Ubo
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      vector<f32, 4>    light_position;
};

struct Resources
{
      [[csl::uniform_buffer(0, 2)]] const Ubo* ubo;
      [[csl::binding(0, 3)]] Sphere*           spheres;
};

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;

[[csl::closest_hit]] void closest_hit_main()
{
      const Resources resources_ = resources<Resources>();

      const Sphere         sphere         = resources_.spheres[primitive_id()];
      const vector<f32, 3> world_position = world_ray_origin() + world_ray_direction() * ray_t_max();
      const vector<f32, 3> world_normal   = normalize(world_position - sphere.center);

      const vector<f32, 3> light_vector = normalize(resources_.ubo->light_position.xyz);
      const f32            diffuse      = max(dot(light_vector, world_normal), 0.2f);
      hit_value                         = sphere.color.xyz * diffuse;
}
