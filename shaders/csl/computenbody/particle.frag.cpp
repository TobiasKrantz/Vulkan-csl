#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> gradient_ramp;
};

struct FragmentInput
{
      [[csl::location(0)]] f32 gradient_pos;
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

      const vector<f32, 3> color = textures.gradient_ramp.sample(vector<f32, 2>{ input.gradient_pos, 0.0f }).xyz;
      const vector<f32, 3> rgb   = textures.color_map.sample(point_coord()).xyz * color;
      output.color               = vector<f32, 4>{ rgb.x, rgb.y, rgb.z, 1.0f };
      return output;
}
