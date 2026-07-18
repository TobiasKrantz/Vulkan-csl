#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
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

      const vector<f32, 3> n = normalize(input.normal);
      const vector<f32, 3> l = normalize(input.light_vector);
      const vector<f32, 3> v = normalize(input.view_vector);
      const vector<f32, 3> r = reflect(-l, n);

      const f32 diffuse  = max(dot(n, l), 0.0f);
      const f32 specular = pow(max(dot(r, v), 0.0f), 1.0f);
      const f32 lit      = (diffuse + specular) * 0.25f;

      output.color = vector<f32, 4>{ lit, lit, lit, 1.0f };
      return output;
}
