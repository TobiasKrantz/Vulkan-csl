#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;
      output.color = input.color;
      return output;
}
