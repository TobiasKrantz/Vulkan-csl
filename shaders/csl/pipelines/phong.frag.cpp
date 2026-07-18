#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const f32            luminance = dot(vector<f32, 3>{ 0.2126f, 0.7152f, 0.0722f }, input.color);
      const vector<f32, 3> color     = lerp(input.color, vector<f32, 3>{ luminance, luminance, luminance }, vector<f32, 3>{ 0.65f, 0.65f, 0.65f });

      const vector<f32, 3> ambient   = color;
      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const vector<f32, 3> view      = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(-light, normal);
      const vector<f32, 3> diffuse   = max(dot(normal, light), 0.0f) * color;
      const vector<f32, 3> specular  = pow(max(dot(reflected, view), 0.0f), 32.0f) * vector<f32, 3>{ 0.35f, 0.35f, 0.35f };

      const vector<f32, 3> lit = ambient + diffuse * 1.75f + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
