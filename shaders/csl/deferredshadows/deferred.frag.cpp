#include <csl/csl.h>

using namespace csl;

constexpr i32 LIGHT_COUNT    = 3;
constexpr f32 SHADOW_FACTOR  = 0.25f;
constexpr f32 AMBIENT_LIGHT  = 0.1f;

struct Light
{
      vector<f32, 4>    position;
      vector<f32, 4>    target;
      vector<f32, 4>    color;
      matrix<f32, 4, 4> view_matrix;
};

struct Ubo
{
      vector<f32, 4> view_position;
      Light          lights[LIGHT_COUNT];
      i32            use_shadows;
      i32            debug_display_target;
};

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D>       position;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D>       normal;
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D>       albedo;
      [[csl::uniform_buffer(0, 4)]] const Ubo*                              ubo;
      [[csl::binding(0, 5)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> shadow_map;
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

      const vector<u32, 3> shadow_size = resources_.shadow_map.size(0u);
      const f32            dx          = 1.5f / f32(shadow_size.x);
      const f32            dy          = 1.5f / f32(shadow_size.y);

      f32 combined_shadow = 1.0f;
      for (i32 i = 0; i < LIGHT_COUNT; ++i)
      {
            const vector<f32, 4> clip      = resources_.ubo->lights[i].view_matrix * vector<f32, 4>{ frag_position.x, frag_position.y, frag_position.z, 1.0f };
            const vector<f32, 4> projected = clip / clip.w;
            const vector<f32, 2> shadow_uv = projected.xy * 0.5f + 0.5f;
            const bool           in_range  = projected.z > -1.0f && projected.z < 1.0f;

            f32 shadow_factor = 0.0f;
            for (i32 x = -1; x <= 1; ++x)
            {
                  for (i32 y = -1; y <= 1; ++y)
                  {
                        const vector<f32, 3> coordinate = vector<f32, 3>{ shadow_uv.x + dx * f32(x), shadow_uv.y + dy * f32(y), f32(i) };
                        const f32            dist       = resources_.shadow_map.sample(coordinate).x;
                        const bool           shadowed   = in_range && projected.w > 0.0f && dist < projected.z;
                        shadow_factor += shadowed ? SHADOW_FACTOR : 1.0f;
                  }
            }
            combined_shadow *= shadow_factor / 9.0f;
      }

      if (resources_.ubo->debug_display_target > 0)
      {
            switch (resources_.ubo->debug_display_target)
            {
            case 1:
                  output.color.xyz = vector<f32, 3>{ combined_shadow, combined_shadow, combined_shadow };
                  break;
            case 2:
                  output.color.xyz = frag_position;
                  break;
            case 3:
                  output.color.xyz = normal;
                  break;
            case 4:
                  output.color.xyz = albedo.xyz;
                  break;
            case 5:
                  output.color.xyz = albedo.www;
                  break;
            }
            output.color.w = 1.0f;
            return output;
      }

      vector<f32, 3> frag_color = albedo.xyz * AMBIENT_LIGHT;

      const vector<f32, 3> n = normalize(normal);

      const f32 light_cos_inner_angle = cos(radians(15.0f));
      const f32 light_cos_outer_angle = cos(radians(25.0f));
      const f32 light_range           = 100.0f;

      for (i32 i = 0; i < LIGHT_COUNT; ++i)
      {
            vector<f32, 3> light = resources_.ubo->lights[i].position.xyz - frag_position;
            const f32      dist  = length(light);
            light                = normalize(light);

            const vector<f32, 3> view = normalize(resources_.ubo->view_position.xyz - frag_position);

            const vector<f32, 3> direction = normalize(resources_.ubo->lights[i].position.xyz - resources_.ubo->lights[i].target.xyz);

            const f32 cos_direction      = dot(light, direction);
            const f32 spot_effect        = smooth_step(light_cos_outer_angle, light_cos_inner_angle, cos_direction);
            const f32 height_attenuation = smooth_step(light_range, 0.0f, dist);

            const f32            n_dot_l   = max(0.0f, dot(n, light));
            const vector<f32, 3> reflected = reflect(-light, n);
            const f32            n_dot_r   = max(0.0f, dot(reflected, view));
            const f32            specular  = pow(n_dot_r, 16.0f) * albedo.w * 2.5f;

            const f32 intensity = (n_dot_l + specular) * spot_effect * height_attenuation;
            frag_color          = frag_color + intensity * resources_.ubo->lights[i].color.xyz * albedo.xyz;
      }

      if (resources_.ubo->use_shadows > 0)
      {
            frag_color = frag_color * combined_shadow;
      }

      output.color = vector<f32, 4>{ frag_color.x, frag_color.y, frag_color.z, 1.0f };
      return output;
}
