#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> color;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentOutput output;
      output.color = vector<f32, 4>{ 1.0f, 1.0f, 1.0f, 1.0f };
      return output;
}
