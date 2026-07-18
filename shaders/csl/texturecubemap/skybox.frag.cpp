#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> cube_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> uvw;
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

      output.color = resource.cube_map.sample(input.uvw);
      return output;
}
