#include <csl/csl.h>

using namespace csl;

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::callable_data(0)]] vector<f32, 3>  out_color;

[[csl::closest_hit]] void closest_hit_main()
{
      execute_callable(ray_geometry_index(), &out_color);
      hit_value = out_color;
}
