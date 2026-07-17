#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      vector<f32, 4>    light_position;
};

struct Resources
{
      [[csl::uniform_buffer(0, 2)]] const Ubo* ubo;
};

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::hit_attribute]] vector<f32, 2>     attributes;

[[csl::closest_hit]] void closest_hit_main()
{
      const Resources resources_ = resources<Resources>();

      const vector<f32, 3> barycentrics = vector<f32, 3>{ 1.0f - attributes.x - attributes.y, attributes.x, attributes.y };

      const vector<f32, 3> position0 = hit_triangle_vertex_position(0);
      const vector<f32, 3> position1 = hit_triangle_vertex_position(1);
      const vector<f32, 3> position2 = hit_triangle_vertex_position(2);
      const vector<f32, 3> current   = position0 * barycentrics.x + position1 * barycentrics.y + position2 * barycentrics.z;

      vector<f32, 3>       normal          = normalize(cross(position1 - position0, position2 - position0));
      const vector<f32, 4> object_normal   = normal * world_to_object();
      normal                               = normalize(object_normal.xyz);

      const vector<f32, 3> light_direction = normalize(resources_.ubo->light_position.xyz - current);
      const f32            diffuse         = max(dot(normal, light_direction), 0.0f);
      hit_value                            = vector<f32, 3>{ 0.1f + diffuse, 0.1f + diffuse, 0.1f + diffuse };
}
