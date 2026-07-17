#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> ssao;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] f32 color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      constexpr i32        blur_range = 2;
      const vector<u32, 2> dimensions = resource.ssao.size(0);
      const vector<f32, 2> texel_size = vector<f32, 2>{ 1.0f, 1.0f } / vector<f32, 2>{ f32(dimensions.x), f32(dimensions.y) };

      i32 count  = 0;
      f32 result = 0.0f;
      for (i32 x = -blur_range; x <= blur_range; ++x)
      {
            for (i32 y = -blur_range; y <= blur_range; ++y)
            {
                  const vector<f32, 2> offset = vector<f32, 2>{ f32(x), f32(y) } * texel_size;
                  result += resource.ssao.sample(input.uv + offset).r;
                  ++count;
            }
      }

      output.color = result / f32(count);
      return output;
}
