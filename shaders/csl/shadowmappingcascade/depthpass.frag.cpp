#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

[[csl::fragment]] void fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();

      const f32 alpha = resource.color_map.sample(input.uv).w;
      if (alpha < 0.5f)
            discard();
}
