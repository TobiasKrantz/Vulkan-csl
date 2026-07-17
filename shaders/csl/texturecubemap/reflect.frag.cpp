#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> inverse_model;
      f32               lod_bias;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                             ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> color;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources     resource = resources<Resources>();
      FragmentInput       input    = stage_input<FragmentInput>();
      FragmentOutput      output;

      const vector<f32, 3> incident  = normalize(input.position);
      vector<f32, 3>       reflected = reflect(incident, normalize(input.normal));

      reflected     = (resource.ubo->inverse_model * vector<f32, 4>{ reflected.x, reflected.y, reflected.z, 0.0f }).xyz;
      reflected.xy = reflected.xy * -1.0f;

      const vector<f32, 4> sampled = resource.color.sample_bias(reflected, resource.ubo->lod_bias);

      const vector<f32, 3> normal      = normalize(input.normal);
      const vector<f32, 3> light       = normalize(input.light_vector);
      const vector<f32, 3> view        = normalize(input.view_vector);
      const vector<f32, 3> mirror      = reflect(-light, normal);
      const vector<f32, 3> ambient     = vector<f32, 3>{ 0.5f, 0.5f, 0.5f } * sampled.xyz;
      const vector<f32, 3> diffuse     = max(dot(normal, light), 0.0f) * vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      const vector<f32, 3> specular    = pow(max(dot(mirror, view), 0.0f), 16.0f) * vector<f32, 3>{ 0.5f, 0.5f, 0.5f };

      const vector<f32, 3> lit = ambient + diffuse * sampled.xyz + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
