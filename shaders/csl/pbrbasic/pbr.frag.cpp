#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
      vector<f32, 3>    camera_position;
};

struct UboShared
{
      vector<f32, 4> lights[4];
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*       ubo;
      [[csl::uniform_buffer(0, 1)]] const UboShared* params;
};

struct PushConstants
{
      vector<f32, 3> object_position;
      f32            roughness;
      f32            metallic;
      f32            r;
      f32            g;
      f32            b;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> world_position;
      [[csl::location(1)]] vector<f32, 3> normal;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

#define PI 3.14159265359f

static f32 d_ggx(f32 dot_nh, f32 roughness)
{
      const f32 alpha  = roughness * roughness;
      const f32 alpha2 = alpha * alpha;
      const f32 denom  = dot_nh * dot_nh * (alpha2 - 1.0f) + 1.0f;
      return alpha2 / (PI * denom * denom);
}

static f32 g_schlicksmith_ggx(f32 dot_nl, f32 dot_nv, f32 roughness)
{
      const f32 r  = roughness + 1.0f;
      const f32 k  = (r * r) / 8.0f;
      const f32 gl = dot_nl / (dot_nl * (1.0f - k) + k);
      const f32 gv = dot_nv / (dot_nv * (1.0f - k) + k);
      return gl * gv;
}

static vector<f32, 3> f_schlick(f32 cos_theta, f32 metallic, vector<f32, 3> material_color)
{
      const vector<f32, 3> f0 = lerp(vector<f32, 3>{ 0.04f, 0.04f, 0.04f }, material_color, vector<f32, 3>{ metallic, metallic, metallic });
      return f0 + (vector<f32, 3>{ 1.0f, 1.0f, 1.0f } - f0) * pow(1.0f - cos_theta, 5.0f);
}

static vector<f32, 3> brdf(vector<f32, 3> l, vector<f32, 3> v, vector<f32, 3> n, f32 metallic, f32 roughness, vector<f32, 3> material_color)
{
      const vector<f32, 3> h      = normalize(v + l);
      const f32            dot_nv = clamp(dot(n, v), 0.0f, 1.0f);
      const f32            dot_nl = clamp(dot(n, l), 0.0f, 1.0f);
      const f32            dot_nh = clamp(dot(n, h), 0.0f, 1.0f);

      const vector<f32, 3> light_color = vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      vector<f32, 3>       color       = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      if (dot_nl > 0.0f)
      {
            const f32            rroughness = max(0.05f, roughness);
            const f32            d          = d_ggx(dot_nh, roughness);
            const f32            g          = g_schlicksmith_ggx(dot_nl, dot_nv, rroughness);
            const vector<f32, 3> f          = f_schlick(dot_nv, metallic, material_color);
            const vector<f32, 3> spec       = d * f * g / (4.0f * dot_nl * dot_nv);
            color                           = color + spec * dot_nl * light_color;
      }
      return color;
}

[[csl::fragment]] FragmentOutput fragment_main(PushConstants push)
{
      const Scene    scene = resources<Scene>();
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> material_color = vector<f32, 3>{ push.r, push.g, push.b };

      const vector<f32, 3> n = normalize(input.normal);
      const vector<f32, 3> v = normalize(scene.ubo->camera_position - input.world_position);

      const f32 roughness = push.roughness;

      vector<f32, 3> lo = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      for (i32 i = 0; i < 4; ++i)
      {
            const vector<f32, 3> l = normalize(scene.params->lights[i].xyz - input.world_position);
            lo                     = lo + brdf(l, v, n, push.metallic, roughness, material_color);
      }

      vector<f32, 3> color = material_color * 0.02f;
      color                = color + lo;
      color                = pow(color, vector<f32, 3>{ 0.4545f, 0.4545f, 0.4545f });

      output.color = vector<f32, 4>{ color.x, color.y, color.z, 1.0f };
      return output;
}
