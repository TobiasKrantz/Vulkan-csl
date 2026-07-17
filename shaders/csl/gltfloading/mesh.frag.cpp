#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 4> color = textures.color_map.sample(input.uv) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const vector<f32, 3> view      = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(light, normal);

      const vector<f32, 3> diffuse  = max(dot(normal, light), 0.15f) * input.color;
      const vector<f32, 3> specular = pow(max(dot(reflected, view), 0.0f), 16.0f) * vector<f32, 3>{ 0.75f, 0.75f, 0.75f };

      const vector<f32, 3> lit = diffuse * color.xyz + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
