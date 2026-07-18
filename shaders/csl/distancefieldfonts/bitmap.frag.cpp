#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color;
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
      const f32 alpha = textures.color.sample(input.uv).w;
      output.color    = vector<f32, 4>{ alpha, alpha, alpha, alpha };
      return output;
}
