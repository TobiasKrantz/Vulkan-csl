#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> font;
};

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
      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const f32 color = textures.font.sample(input.uv).x;
      output.color    = vector<f32, 4>{ color, color, color, 1.0f };
      return output;
}
