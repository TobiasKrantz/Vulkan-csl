#include <csl/csl.h>

using namespace csl;

struct Light
{
      vector<f32, 4> position;
      vector<f32, 3> color;
      f32            radius;
};

struct Resources
{
      [[csl::input_attachment(0, 0, 0)]] SubpassInput<f32> input_position;
      [[csl::input_attachment(0, 1, 1)]] SubpassInput<f32> input_normal;
      [[csl::input_attachment(0, 2, 2)]] SubpassInput<f32> input_albedo;
      [[csl::binding(0, 3)]] const Light*                  lights;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentOutput  output;

      const vector<f32, 3> frag_position = resources_.input_position.load().xyz;
      const vector<f32, 3> normal        = resources_.input_normal.load().xyz;
      const vector<f32, 4> albedo        = resources_.input_albedo.load();

      constexpr f32  ambient    = 0.05f;
      vector<f32, 3> frag_color = albedo.xyz * ambient;

      for (u32 i = 0; i < array_length(resources_.lights); ++i)
      {
            vector<f32, 3> light_vector = resources_.lights[i].position.xyz - frag_position;
            const f32      dist         = length(light_vector);

            light_vector = normalize(light_vector);
            const f32 attenuation = resources_.lights[i].radius / (pow(dist, 3.0f) + 1.0f);

            const vector<f32, 3> n       = normalize(normal);
            const f32            n_dot_l = max(0.0f, dot(n, light_vector));
            const vector<f32, 3> diffuse = resources_.lights[i].color * albedo.xyz * n_dot_l * attenuation;

            frag_color = frag_color + diffuse;
      }

      output.color = vector<f32, 4>{ frag_color.x, frag_color.y, frag_color.z, 1.0f };
      return output;
}
