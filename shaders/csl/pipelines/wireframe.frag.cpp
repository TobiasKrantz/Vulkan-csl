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
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;
      output.color     = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      output.color.xyz = input.color * 1.5f;
      return output;
}
