#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      vector<f32, 2> brightness_contrast;
      vector<f32, 2> range;
      i32            attachment_index;
};

struct Attachments
{
      [[csl::input_attachment(0, 0, 0)]] SubpassInput<f32> input_color;
      [[csl::input_attachment(0, 1, 1)]] SubpassInput<f32> input_depth;
      [[csl::uniform_buffer(0, 2)]] const Ubo*             ubo;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static vector<f32, 3> brightness_contrast(vector<f32, 3> color, f32 brightness, f32 contrast)
{
      return (color - 0.5f) * contrast + 0.5f + brightness;
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Attachments attachments = resources<Attachments>();
      FragmentOutput    output;

      if (attachments.ubo->attachment_index == 0)
      {
            const vector<f32, 3> color = attachments.input_color.load().xyz;
            output.color.xyz           = brightness_contrast(color, attachments.ubo->brightness_contrast.x, attachments.ubo->brightness_contrast.y);
      }

      if (attachments.ubo->attachment_index == 1)
      {
            const f32 depth = attachments.input_depth.load().x;
            const f32 value = (depth - attachments.ubo->range.x) * 1.0f / (attachments.ubo->range.y - attachments.ubo->range.x);
            output.color.xyz = vector<f32, 3>{ value, value, value };
      }

      return output;
}
