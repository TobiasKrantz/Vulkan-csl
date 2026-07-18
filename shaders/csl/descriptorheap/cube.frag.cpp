#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(1, 0)]] SampledImage<f32, ImageType::IMAGE_2D> texture_image[2];
      [[csl::binding(2, 0)]] Sampler                                texture_sampler[2];
};

struct PushConstants
{
      i32 sampler_index;
      i32 frame_index;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] i32            instance_index;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main(PushConstants push)
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const u32            instance = static_cast<u32>(input.instance_index);
      const u32            sampler  = static_cast<u32>(push.sampler_index);
      const vector<f32, 4> texel    = resources_.texture_image[instance].sample(resources_.texture_sampler[sampler], input.uv);
      output.color                  = texel * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      return output;
}
