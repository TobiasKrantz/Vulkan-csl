#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> light_space;
      vector<f32, 4>    light_position;
      f32               z_near;
      f32               z_far;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                          ubo;
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

static f32 linearize_depth(f32 depth, f32 z_near, f32 z_far)
{
      return (2.0f * z_near) / (z_far + z_near - depth * (z_far - z_near));
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const f32 depth  = resource.color.sample(input.uv).x;
      const f32 linear = 1.0f - linearize_depth(depth, resource.ubo->z_near, resource.ubo->z_far);
      output.color     = vector<f32, 4>{ linear, linear, linear, 1.0f };
      return output;
}
