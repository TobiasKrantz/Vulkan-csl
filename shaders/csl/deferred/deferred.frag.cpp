#include <csl/csl.h>

using namespace csl;

struct Light
{
      vector<f32, 4> position;
      vector<f32, 3> color;
      f32            radius;
};

struct Ubo
{
      Light          lights[6];
      vector<f32, 4> view_position;
      i32            display_debug_target;
};

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> position;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal;
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D> albedo;
      [[csl::uniform_buffer(0, 4)]] const Ubo*                         ubo;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> frag_position = resources_.position.sample(input.uv).xyz;
      const vector<f32, 3> normal        = resources_.normal.sample(input.uv).xyz;
      const vector<f32, 4> albedo        = resources_.albedo.sample(input.uv);

      if (resources_.ubo->display_debug_target > 0)
      {
            switch (resources_.ubo->display_debug_target)
            {
            case 1:
                  output.color.xyz = frag_position;
                  break;
            case 2:
                  output.color.xyz = normal;
                  break;
            case 3:
                  output.color.xyz = albedo.xyz;
                  break;
            case 4:
                  output.color.xyz = albedo.www;
                  break;
            }
            output.color.w = 1.0f;
            return output;
      }

      constexpr f32 ambient = 0.0f;

      vector<f32, 3> frag_color = albedo.xyz * ambient;

      for (i32 i = 0; i < 6; ++i)
      {
            vector<f32, 3> light_vector = resources_.ubo->lights[i].position.xyz - frag_position;
            const f32      dist         = length(light_vector);

            vector<f32, 3> view = resources_.ubo->view_position.xyz - frag_position;
            view                = normalize(view);

            light_vector = normalize(light_vector);

            const f32 attenuation = resources_.ubo->lights[i].radius / (pow(dist, 2.0f) + 1.0f);

            const vector<f32, 3> n            = normalize(normal);
            const f32            n_dot_l      = max(0.0f, dot(n, light_vector));
            const vector<f32, 3> diffuse      = resources_.ubo->lights[i].color * albedo.xyz * n_dot_l * attenuation;

            const vector<f32, 3> reflected = reflect(-light_vector, n);
            const f32            n_dot_r   = max(0.0f, dot(reflected, view));
            const vector<f32, 3> specular  = resources_.ubo->lights[i].color * albedo.w * pow(n_dot_r, 16.0f) * attenuation;

            frag_color = frag_color + diffuse + specular;
      }

      output.color = vector<f32, 4>{ frag_color.x, frag_color.y, frag_color.z, 1.0f };
      return output;
}
