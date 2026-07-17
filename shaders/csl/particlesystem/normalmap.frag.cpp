#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal_height_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> light_vector;
      [[csl::location(2)]] vector<f32, 3> light_vector_b;
      [[csl::location(3)]] vector<f32, 3> light_dir;
      [[csl::location(4)]] vector<f32, 3> view_vector;
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

      const f32 light_radius   = 45.0f;
      const vector<f32, 3> specular_color = vector<f32, 3>{ 0.85f, 0.5f, 0.0f };
      const f32 inverse_radius = 1.0f / light_radius;
      const f32 ambient        = 0.25f;

      const vector<f32, 3> rgb    = textures.color_map.sample(input.uv).xyz;
      const vector<f32, 3> normal = normalize((textures.normal_height_map.sample(input.uv).xyz - 0.5f) * 2.0f);

      const f32            distance_squared = dot(input.light_vector_b, input.light_vector_b);
      const vector<f32, 3> light_vec        = input.light_vector_b * rsqrt(distance_squared);

      const f32 attenuation = max(clamp(1.0f - inverse_radius * sqrt(distance_squared), 0.0f, 1.0f), ambient);
      const f32 diffuse     = clamp(dot(light_vec, normal), 0.0f, 1.0f);

      const vector<f32, 3> light      = normalize(-input.light_vector);
      const vector<f32, 3> view       = normalize(input.view_vector);
      const vector<f32, 3> reflected  = reflect(-light, normal);
      const f32            specular   = pow(max(dot(view, reflected), 0.0f), 4.0f);

      const vector<f32, 3> lit = (rgb * attenuation + (diffuse * rgb + 0.5f * specular * specular_color)) * attenuation;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
