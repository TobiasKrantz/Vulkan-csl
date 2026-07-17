#include <csl/csl.h>

using namespace csl;

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::hit_attribute]] vector<f32, 2>     attributes;

[[csl::closest_hit]] void closest_hit_main()
{
      const vector<f32, 3> barycentrics = vector<f32, 3>{ 1.0f - attributes.x - attributes.y, attributes.x, attributes.y };
      hit_value                         = barycentrics;
}
