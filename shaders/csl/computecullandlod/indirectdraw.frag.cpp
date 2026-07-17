#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> normal   = normalize(input.normal);
      const vector<f32, 3> light    = normalize(input.light_vector);
      const vector<f32, 3> ambient  = vector<f32, 3>{ 0.25f, 0.25f, 0.25f };
      const f32            factor   = max(dot(normal, light), 0.0f);
      const vector<f32, 3> diffuse  = vector<f32, 3>{ factor, factor, factor };

      const vector<f32, 3> lit = (ambient + diffuse) * input.color;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
