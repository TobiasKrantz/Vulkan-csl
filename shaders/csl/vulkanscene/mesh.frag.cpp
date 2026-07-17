#include <csl/csl.h>

using namespace csl;

static f32 spec_part(vector<f32, 3> light, vector<f32, 3> normal, vector<f32, 3> half_vector)
{
      if (dot(normal, light) > 0.0f)
            return pow(clamp(dot(half_vector, normal), 0.0f, 1.0f), 64.0f);
      return 0.0f;
}

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> eye_position;
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

      const vector<f32, 3> eye       = normalize(vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - input.eye_position);
      const vector<f32, 3> reflected = normalize(reflect(-input.light_vector, input.normal));

      const vector<f32, 3> half_vector = normalize(input.light_vector + input.eye_position);
      const f32            diffuse     = clamp(dot(input.light_vector, input.normal), 0.0f, 1.0f);
      const f32            spec        = spec_part(input.light_vector, input.normal, half_vector);
      const f32            intensity   = 0.1f + diffuse + spec;

      const vector<f32, 4> ambient        = vector<f32, 4>{ 0.2f, 0.2f, 0.2f, 1.0f };
      const vector<f32, 4> diffuse_term   = vector<f32, 4>{ 0.5f, 0.5f, 0.5f, 0.5f } * max(dot(input.normal, input.light_vector), 0.0f);
      const f32            shininess      = 0.75f;
      const vector<f32, 4> specular_term  = vector<f32, 4>{ 0.5f, 0.5f, 0.5f, 1.0f } * pow(max(dot(reflected, eye), 0.0f), 2.0f) * shininess;

      const vector<f32, 4> base = vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      output.color              = (ambient + diffuse_term) * base + specular_term;

      if (intensity > 0.95f)
            output.color = output.color * 2.25f;
      if (intensity < 0.15f)
            output.color = vector<f32, 4>{ 0.1f, 0.1f, 0.1f, 0.1f };
      return output;
}
