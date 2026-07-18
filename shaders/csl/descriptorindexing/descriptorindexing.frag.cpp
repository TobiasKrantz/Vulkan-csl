#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> textures[];
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] i32            texture_index;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();

      FragmentOutput output;
      output.color = resources_.textures[nonuniform(input.texture_index)].sample(input.uv);
      return output;
}
