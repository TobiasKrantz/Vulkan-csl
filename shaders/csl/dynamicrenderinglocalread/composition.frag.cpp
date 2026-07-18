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
      [[csl::input_attachment(0, 0, 0)]] SubpassInput<f32> position_depth;
      [[csl::input_attachment(0, 1, 1)]] SubpassInput<f32> normal;
      [[csl::input_attachment(0, 2, 2)]] SubpassInput<f32> albedo;
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

      const vector<f32, 3> frag_position = resources_.position_depth.load().xyz;
      const vector<f32, 3> normal        = resources_.normal.load().xyz;
      const vector<f32, 4> albedo        = resources_.albedo.load();

      constexpr f32  ambient    = 0.15f;
      vector<f32, 3> frag_color = albedo.xyz * ambient;

      for (u32 i = 0; i < array_length(resources_.lights); ++i)
      {
            const vector<f32, 3> light_vector = resources_.lights[i].position.xyz - frag_position;
            const f32            dist         = length(light_vector);

            const f32 attenuation = resources_.lights[i].radius / (pow(dist, 8.0f) + 1.0f);

            const f32            n_dot_l = max(0.0f, dot(normalize(normal), normalize(light_vector)));
            const vector<f32, 3> diffuse = resources_.lights[i].color * albedo.xyz * n_dot_l * attenuation;

            frag_color = frag_color + diffuse;
      }

      output.color = vector<f32, 4>{ frag_color.x, frag_color.y, frag_color.z, 1.0f };
      return output;
}
