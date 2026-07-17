#include <csl/csl.h>

using namespace csl;

[[csl::callable_data_in(0)]] vector<f32, 3> out_color;

[[csl::callable]] void callable_main()
{
      const vector<u32, 3> id    = launch_id();
      const vector<f32, 2> pos   = vector<f32, 2>{ f32(id.x / 8), f32(id.y / 8) };
      const f32            value = mod(pos.x + mod(pos.y, 2.0f), 2.0f);
      out_color                  = vector<f32, 3>{ value, value, value };
}
