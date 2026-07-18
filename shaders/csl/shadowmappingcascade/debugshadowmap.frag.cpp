#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> shadow_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] u32            cascade_index;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> coordinate = vector<f32, 3>{ input.uv.x, input.uv.y, f32(input.cascade_index) };
      const f32            depth      = resource.shadow_map.sample(coordinate).x;
      output.color                    = vector<f32, 4>{ depth, depth, depth, 1.0f };
      return output;
}
