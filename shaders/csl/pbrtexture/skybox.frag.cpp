#include <csl/csl.h>

using namespace csl;

struct UboParams
{
      vector<f32, 4> lights[4];
      f32            exposure;
      f32            gamma;
};

struct Resources
{
      [[csl::uniform_buffer(0, 1)]] const UboParams*                         params;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> environment;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> uvw;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static vector<f32, 3> uncharted2_tonemap(vector<f32, 3> color)
{
      const f32 a = 0.15f;
      const f32 b = 0.50f;
      const f32 c = 0.10f;
      const f32 d = 0.20f;
      const f32 e = 0.02f;
      const f32 f = 0.30f;
      return ((color * (a * color + c * b) + d * e) / (color * (a * color + b) + d * f)) - e / f;
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources res   = resources<Resources>();
      FragmentInput   input = stage_input<FragmentInput>();
      FragmentOutput  output;

      vector<f32, 3> color = res.environment.sample(input.uvw).xyz;

      color                      = uncharted2_tonemap(color * res.params->exposure);
      const vector<f32, 3> white = vector<f32, 3>{ 11.2f, 11.2f, 11.2f };
      color                      = color * (1.0f / uncharted2_tonemap(white));
      const f32 inv_gamma        = 1.0f / res.params->gamma;
      color                      = pow(color, vector<f32, 3>{ inv_gamma, inv_gamma, inv_gamma });

      output.color = vector<f32, 4>{ color.x, color.y, color.z, 1.0f };
      return output;
}
