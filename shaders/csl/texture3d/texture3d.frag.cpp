#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_3D> color;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> uv;
      [[csl::location(1)]] f32            lod_bias;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
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

      const vector<f32, 4> sampled = resource.color.sample(input.uv);

      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const vector<f32, 3> view      = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(-light, normal);

      const vector<f32, 3> diffuse  = max(dot(normal, light), 0.0f) * vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      const f32            specular = pow(max(dot(reflected, view), 0.0f), 16.0f) * sampled.x;

      const vector<f32, 3> lit = diffuse * sampled.x + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
