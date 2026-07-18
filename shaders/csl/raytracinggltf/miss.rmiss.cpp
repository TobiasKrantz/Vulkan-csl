#include <csl/csl.h>

using namespace csl;

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;

[[csl::miss]] void miss_main()
{
      hit_value = vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
}
