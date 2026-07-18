#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
      f32               near_plane;
      f32               far_plane;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                         ubo;
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> view_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 4> normal;
      [[csl::location(2)]] vector<f32, 4> albedo;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const f32 depth  = frag_coord().z;
      const f32 z      = depth * 2.0f - 1.0f;
      const f32 linear = (2.0f * resource.ubo->near_plane * resource.ubo->far_plane)
                         / (resource.ubo->far_plane + resource.ubo->near_plane - z * (resource.ubo->far_plane - resource.ubo->near_plane));

      output.position = vector<f32, 4>{ input.view_position.x, input.view_position.y, input.view_position.z, linear };

      const vector<f32, 3> normal = normalize(input.normal) * 0.5f + 0.5f;
      output.normal               = vector<f32, 4>{ normal.x, normal.y, normal.z, 1.0f };

      output.albedo = resource.color_map.sample(input.uv) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      return output;
}
