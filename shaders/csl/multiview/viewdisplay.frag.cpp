#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection[2];
      matrix<f32, 4, 4> modelview[2];
      vector<f32, 4>    light_position;
      f32               distortion_alpha;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                                ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> view;
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
      [[csl::constant(0)]] const f32 view_layer = 0.0f;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const f32 alpha = resource.ubo->distortion_alpha;

      const vector<f32, 2> p1 = input.uv * 2.0f - 1.0f;
      vector<f32, 2>       p2 = p1 / (1.0f - alpha * length(p1));
      p2                      = (p2 + 1.0f) * 0.5f;

      const bool inside = p2.x >= 0.0f && p2.x <= 1.0f && p2.y >= 0.0f && p2.y <= 1.0f;
      output.color      = inside ? resource.view.sample(vector<f32, 3>{ p2.x, p2.y, view_layer }) : vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      return output;
}
