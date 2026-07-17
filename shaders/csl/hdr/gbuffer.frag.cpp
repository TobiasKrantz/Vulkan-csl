#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      matrix<f32, 4, 4> inverse_modelview;
      f32               exposure;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                              ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> env_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> uvw;
      [[csl::location(1)]] vector<f32, 3> position;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color0;
      [[csl::location(1)]] vector<f32, 4> color1;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const i32 type = 0;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      vector<f32, 4> color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };

      switch (type)
      {
      case 0:
      {
            const vector<f32, 3> normal = normalize(input.uvw);
            color                       = resource.env_map.sample(normal);
            break;
      }
      case 1:
      {
            const vector<f32, 3> world_view   = mat3(resource.ubo->inverse_modelview) * normalize(input.view_vector);
            const vector<f32, 3> normal       = normalize(input.normal);
            const vector<f32, 3> world_normal = mat3(resource.ubo->inverse_modelview) * normal;

            const f32            n_dot_l   = max(dot(normal, input.light_vector), 0.0f);
            const vector<f32, 3> eye_dir   = normalize(input.view_vector);
            const vector<f32, 3> half_vec  = normalize(input.light_vector + eye_dir);
            const f32            n_dot_h   = max(dot(normal, half_vec), 0.0f);
            const f32            n_dot_v   = max(dot(normal, eye_dir), 0.0f);
            const f32            v_dot_h   = max(dot(eye_dir, half_vec), 0.0f);

            const f32 n_h_2         = 2.0f * n_dot_h;
            const f32 g1            = (n_h_2 * n_dot_v) / v_dot_h;
            const f32 g2            = (n_h_2 * n_dot_l) / v_dot_h;
            const f32 geometric     = min(1.0f, min(g1, g2));
            const f32 f0            = 0.6f;
            const f32 k             = 0.2f;
            f32       fresnel       = pow(1.0f - v_dot_h, 5.0f);
            fresnel *= (1.0f - f0);
            fresnel += f0;
            const f32 spec = (fresnel * geometric) / (n_dot_v * n_dot_l * 3.14f);

            color                  = resource.env_map.sample(reflect(-world_view, world_normal));
            const vector<f32, 3> lit = color.xyz * n_dot_l * (k + spec * (1.0f - k));
            color                  = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
            break;
      }
      case 2:
      {
            const vector<f32, 3> world_view   = mat3(resource.ubo->inverse_modelview) * normalize(input.view_vector);
            const vector<f32, 3> world_normal = mat3(resource.ubo->inverse_modelview) * input.normal;
            color                             = resource.env_map.sample(refract(-world_view, world_normal, 1.0f / 1.6f));
            break;
      }
      }

      const vector<f32, 3> exposed = vector<f32, 3>{ 1.0f, 1.0f, 1.0f } - exp(-color.xyz * resource.ubo->exposure);
      output.color0.xyz            = exposed;

      const f32            luma      = dot(exposed, vector<f32, 3>{ 0.2126f, 0.7152f, 0.0722f });
      const f32            threshold = 0.75f;
      const vector<f32, 3> bright    = luma > threshold ? exposed : vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      output.color1                  = vector<f32, 4>{ bright.x, bright.y, bright.z, 1.0f };
      return output;
}
