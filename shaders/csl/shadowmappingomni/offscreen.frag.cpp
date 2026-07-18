#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 3> light_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] f32 color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> light_vector = input.position.xyz - input.light_position;
      output.color                      = length(light_vector);
      return output;
}
