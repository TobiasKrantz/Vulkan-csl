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

      const vector<f32, 3> bary = bary_coord();
      const vector<f32, 4> base = vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      if (bary.x < 0.02f || bary.y < 0.02f || bary.z < 0.02f)
      {
            output.color = base * 2.0f;
      }
      else
      {
            output.color = base * 0.5f;
      }
      return output;
}
