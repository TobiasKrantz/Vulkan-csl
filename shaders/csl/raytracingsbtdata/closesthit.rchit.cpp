#include <csl/csl.h>

using namespace csl;

struct ShaderRecord
{
      f32 r;
      f32 g;
      f32 b;
};

[[csl::ray_payload_in(0)]] vector<f32, 3> hit_value;
[[csl::hit_attribute]] vector<f32, 2>     attributes;
[[csl::shader_record]] ShaderRecord       record;

[[csl::closest_hit]] void closest_hit_main()
{
      hit_value = vector<f32, 3>{ record.r, record.g, record.b };
}
