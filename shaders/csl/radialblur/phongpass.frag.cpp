#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> gradient_ramp;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 2> uv;
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

      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      if ((input.color.x >= 0.9f) || (input.color.y >= 0.9f) || (input.color.z >= 0.9f))
      {
            output.color.xyz = textures.gradient_ramp.sample(input.uv).xyz;
            return output;
      }

      const vector<f32, 3> eye       = normalize(-input.eye_position);
      const vector<f32, 3> reflected = normalize(reflect(-input.light_vector, input.normal));

      const vector<f32, 4> ambient  = vector<f32, 4>{ 0.2f, 0.2f, 0.2f, 1.0f };
      const vector<f32, 4> diffuse  = vector<f32, 4>{ 0.5f, 0.5f, 0.5f, 0.5f } * max(dot(input.normal, input.light_vector), 0.0f);
      const f32            specular = 0.25f;
      const vector<f32, 4> lit_specular = vector<f32, 4>{ 0.5f, 0.5f, 0.5f, 1.0f } * pow(max(dot(reflected, eye), 0.0f), 4.0f) * specular;

      output.color = (ambient + diffuse) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f } + lit_specular;
      return output;
}
