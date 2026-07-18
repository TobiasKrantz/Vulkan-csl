#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> gradient_ramp;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> color;
      [[csl::location(1)]] vector<f32, 2> uv;
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

      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      if ((input.color.x >= 0.9f) || (input.color.y >= 0.9f) || (input.color.z >= 0.9f))
            output.color.xyz = textures.gradient_ramp.sample(input.uv).xyz;
      else
            output.color.xyz = input.color;
      return output;
}
