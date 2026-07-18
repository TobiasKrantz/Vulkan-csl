#include <csl/csl.h>

using namespace csl;

struct MatrixReference
{
      matrix<f32, 4, 4> mvp;
      u32               sampler_index;
      u32               image_heap_index_offset;
};

struct PushConstants
{
      const MatrixReference* matrix_reference;
};

struct Resources
{
      [[csl::descriptor_heap]] SampledImage<f32, ImageType::IMAGE_2D>* texture_image;
      [[csl::descriptor_heap]] Sampler*                                texture_sampler;
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

      const MatrixReference* uniform_data = push.matrix_reference;

      const u32            image   = uniform_data->image_heap_index_offset + u32(input.instance_index);
      const vector<f32, 4> texel   = resources_.texture_image[image].sample(resources_.texture_sampler[uniform_data->sampler_index], input.uv);
      output.color                 = texel * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      return output;
}
