#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> inverse_model;
      f32               lod_bias;
      i32               cube_map_index;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                                   ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP_ARRAY> cube_map_array;
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

      const vector<f32, 4> coordinate = vector<f32, 4>{ input.uvw.x, input.uvw.y, input.uvw.z, static_cast<f32>(resource.ubo->cube_map_index) };
      output.color                    = resource.cube_map_array.sample_lod(coordinate, resource.ubo->lod_bias);
      return output;
}
