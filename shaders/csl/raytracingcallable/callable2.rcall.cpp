#include <csl/csl.h>

using namespace csl;

[[csl::callable_data_in(0)]] vector<f32, 3> out_color;

[[csl::callable]] void callable_main()
{
      out_color = vector<f32, 3>{ 0.0f, 1.0f, 0.0f };
}
