#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      vector<f32, 3> ambient = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      if (input.color.x >= 0.9f || input.color.y >= 0.9f || input.color.z >= 0.9f)
            ambient = input.color * 0.25f;

      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const vector<f32, 3> view      = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(-light, normal);

      const vector<f32, 3> diffuse  = max(dot(normal, light), 0.0f) * input.color;
      const vector<f32, 3> specular = pow(max(dot(reflected, view), 0.0f), 8.0f) * vector<f32, 3>{ 0.75f, 0.75f, 0.75f };

      const vector<f32, 3> lit = ambient + diffuse + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
