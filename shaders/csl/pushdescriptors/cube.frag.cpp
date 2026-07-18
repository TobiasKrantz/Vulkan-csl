#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
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
      output.color = textures.color_map.sample(input.uv) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      return output;
}
