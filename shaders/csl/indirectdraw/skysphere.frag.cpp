#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 4> gradient_start = vector<f32, 4>{ 0.93f, 0.9f, 0.81f, 1.0f };
      const vector<f32, 4> gradient_end   = vector<f32, 4>{ 0.35f, 0.5f, 1.0f, 1.0f };

      const f32 amount = min(0.5f - (input.uv.y + 0.05f), 0.5f) / 0.15f + 0.5f;
      output.color     = lerp(gradient_start, gradient_end, vector<f32, 4>{ amount, amount, amount, amount });
      return output;
}
