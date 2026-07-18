#include <csl/csl.h>

using namespace csl;

struct Resources
{
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

      const vector<f32, 2> texel_offset = vector<f32, 2>{ 0.01f, 0.01f };

      const vector<f32, 2> tc0 = input.uv + vector<f32, 2>{ -texel_offset.x, -texel_offset.y };
      const vector<f32, 2> tc1 = input.uv + vector<f32, 2>{ 0.0f, -texel_offset.y };
      const vector<f32, 2> tc2 = input.uv + vector<f32, 2>{ +texel_offset.x, -texel_offset.y };
      const vector<f32, 2> tc3 = input.uv + vector<f32, 2>{ -texel_offset.x, 0.0f };
      const vector<f32, 2> tc4 = input.uv + vector<f32, 2>{ 0.0f, 0.0f };
      const vector<f32, 2> tc5 = input.uv + vector<f32, 2>{ +texel_offset.x, 0.0f };
      const vector<f32, 2> tc6 = input.uv + vector<f32, 2>{ -texel_offset.x, +texel_offset.y };
      const vector<f32, 2> tc7 = input.uv + vector<f32, 2>{ 0.0f, +texel_offset.y };
      const vector<f32, 2> tc8 = input.uv + vector<f32, 2>{ +texel_offset.x, +texel_offset.y };

      const vector<f32, 4> col0 = resources_.color.sample(tc0);
      const vector<f32, 4> col1 = resources_.color.sample(tc1);
      const vector<f32, 4> col2 = resources_.color.sample(tc2);
      const vector<f32, 4> col3 = resources_.color.sample(tc3);
      const vector<f32, 4> col4 = resources_.color.sample(tc4);
      const vector<f32, 4> col5 = resources_.color.sample(tc5);
      const vector<f32, 4> col6 = resources_.color.sample(tc6);
      const vector<f32, 4> col7 = resources_.color.sample(tc7);
      const vector<f32, 4> col8 = resources_.color.sample(tc8);

      const vector<f32, 4> sum = (1.0f * col0 + 2.0f * col1 + 1.0f * col2 + 2.0f * col3 + 4.0f * col4 + 2.0f * col5 + 1.0f * col6 + 2.0f * col7 + 1.0f * col8) / 16.0f;

      output.color = vector<f32, 4>{ sum.x, sum.y, sum.z, 1.0f };
      return output;
}
