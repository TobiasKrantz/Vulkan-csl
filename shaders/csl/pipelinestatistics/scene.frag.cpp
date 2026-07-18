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

      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const vector<f32, 3> view      = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(-light, normal);
      const vector<f32, 3> diffuse   = max(dot(normal, light), 0.0f) * input.color;
      const vector<f32, 3> specular  = pow(max(dot(reflected, view), 0.0f), 8.0f) * vector<f32, 3>{ 0.75f, 0.75f, 0.75f };

      const vector<f32, 3> lit = diffuse + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 0.5f };
      return output;
}
