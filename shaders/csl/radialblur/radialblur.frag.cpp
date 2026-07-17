#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      f32            radial_blur_scale;
      f32            radial_blur_strength;
      vector<f32, 2> radial_origin;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                        ubo;
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
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<u32, 2> texel_dimensions = resources_.color.size(0);
      const vector<f32, 2> radial_size      = vector<f32, 2>{ 1.0f / f32(texel_dimensions.x), 1.0f / f32(texel_dimensions.y) };

      vector<f32, 2> uv    = input.uv;
      vector<f32, 4> color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      uv += radial_size * 0.5f - resources_.ubo->radial_origin;

      const i32 samples = 32;
      for (i32 i = 0; i < samples; ++i)
      {
            const f32 scale = 1.0f - resources_.ubo->radial_blur_scale * (f32(i) / f32(samples - 1));
            color += resources_.color.sample(uv * scale + resources_.ubo->radial_origin);
      }

      output.color = (color / f32(samples)) * resources_.ubo->radial_blur_strength;
      return output;
}
