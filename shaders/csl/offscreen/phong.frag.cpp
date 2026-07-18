#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
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

      const vector<f32, 3> eye       = normalize(-input.eye_position);
      const vector<f32, 3> reflected = normalize(reflect(-input.light_vector, input.normal));

      const vector<f32, 4> ambient      = vector<f32, 4>{ 0.1f, 0.1f, 0.1f, 1.0f };
      const f32            diffuse_term = max(dot(input.normal, input.light_vector), 0.0f);
      const vector<f32, 4> diffuse      = vector<f32, 4>{ diffuse_term, diffuse_term, diffuse_term, diffuse_term };

      const f32      specular_strength = 0.75f;
      vector<f32, 4> specular          = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      if (dot(input.eye_position, input.normal) < 0.0f)
      {
            const f32 term = pow(max(dot(reflected, eye), 0.0f), 16.0f) * specular_strength;
            specular       = vector<f32, 4>{ 0.5f, 0.5f, 0.5f, 1.0f } * term;
      }

      const vector<f32, 4> base = vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      output.color              = (ambient + diffuse) * base + specular;
      return output;
}
