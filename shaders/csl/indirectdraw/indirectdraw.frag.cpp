#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> sampler_array;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> uv;
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

      const vector<f32, 4> color = textures.sampler_array.sample(input.uv);
      if (color.w < 0.5f)
            discard();

      const vector<f32, 3> normal  = normalize(input.normal);
      const vector<f32, 3> light   = normalize(input.light_vector);
      const vector<f32, 3> ambient = vector<f32, 3>{ 0.65f, 0.65f, 0.65f };
      const vector<f32, 3> diffuse = max(dot(normal, light), 0.0f) * input.color;

      const vector<f32, 3> lit = (ambient + diffuse) * color.xyz;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
