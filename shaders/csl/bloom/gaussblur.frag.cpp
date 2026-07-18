#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      f32 blur_scale;
      f32 blur_strength;
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
      [[csl::constant(0)]] const i32 blur_direction = 0;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      f32 weight[5];
      weight[0] = 0.227027f;
      weight[1] = 0.1945946f;
      weight[2] = 0.1216216f;
      weight[3] = 0.054054f;
      weight[4] = 0.016216f;

      const vector<u32, 2> size        = resource.color.size(0);
      const vector<f32, 2> texel_offset = vector<f32, 2>{ 1.0f / f32(size.x), 1.0f / f32(size.y) } * resource.ubo->blur_scale;

      vector<f32, 3> result = resource.color.sample(input.uv).xyz * weight[0];
      for (i32 i = 1; i < 5; ++i)
      {
            if (blur_direction == 1)
            {
                  result += resource.color.sample(input.uv + vector<f32, 2>{ texel_offset.x * f32(i), 0.0f }).xyz * weight[i] * resource.ubo->blur_strength;
                  result += resource.color.sample(input.uv - vector<f32, 2>{ texel_offset.x * f32(i), 0.0f }).xyz * weight[i] * resource.ubo->blur_strength;
            }
            else
            {
                  result += resource.color.sample(input.uv + vector<f32, 2>{ 0.0f, texel_offset.y * f32(i) }).xyz * weight[i] * resource.ubo->blur_strength;
                  result += resource.color.sample(input.uv - vector<f32, 2>{ 0.0f, texel_offset.y * f32(i) }).xyz * weight[i] * resource.ubo->blur_strength;
            }
      }

      output.color = vector<f32, 4>{ result.x, result.y, result.z, 1.0f };
      return output;
}
