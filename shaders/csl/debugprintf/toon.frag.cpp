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

      const vector<f32, 3> normal = normalize(input.normal);
      const vector<f32, 3> light  = normalize(input.light_vector);

      const f32 intensity = dot(normal, light);
      f32       shade     = 1.0f;
      shade               = intensity < 0.5f ? 0.75f : shade;
      shade               = intensity < 0.35f ? 0.6f : shade;
      shade               = intensity < 0.25f ? 0.5f : shade;
      shade               = intensity < 0.1f ? 0.25f : shade;

      output.color     = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      output.color.xyz = input.color * 3.0f * shade;
      return output;
}
