#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> normal      = normalize(input.normal);
      const vector<f32, 3> light       = normalize(vector<f32, 3>{ -4.0f, -4.0f, 0.0f });
      const vector<f32, 4> texel       = resources_.color_map.sample(input.uv);
      const f32            intensity   = clamp(max(dot(normal, light), 0.0f), 0.2f, 1.0f);
      const vector<f32, 3> shaded      = texel.xyz * intensity;
      output.color                     = vector<f32, 4>{ shaded.x, shaded.y, shaded.z, 1.0f };
      return output;
}
