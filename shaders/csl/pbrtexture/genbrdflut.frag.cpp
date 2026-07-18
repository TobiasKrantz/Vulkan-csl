#include <csl/csl.h>

using namespace csl;

#define PI 3.1415926536f

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static f32 random(vector<f32, 2> co)
{
      const f32 dt = dot(co, vector<f32, 2>{ 12.9898f, 78.233f });
      const f32 sn = dt - 3.14f * floor(dt / 3.14f);
      return fract(sin(sn) * 43758.5453f);
}

static vector<f32, 2> hammersley_2d(u32 i, u32 count)
{
      u32 bits = (i << 16u) | (i >> 16u);
      bits     = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
      bits     = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
      bits     = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
      bits     = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

      const f32 rdi = f32(bits) * 2.3283064365386963e-10f;
      return vector<f32, 2>{ f32(i) / f32(count), rdi };
}

static vector<f32, 3> importance_sample_ggx(vector<f32, 2> xi, f32 roughness, vector<f32, 3> normal)
{
      const f32 alpha     = roughness * roughness;
      const f32 phi       = 2.0f * PI * xi.x + random(normal.xz) * 0.1f;
      const f32 cos_theta = sqrt((1.0f - xi.y) / (1.0f + (alpha * alpha - 1.0f) * xi.y));
      const f32 sin_theta = sqrt(1.0f - cos_theta * cos_theta);

      const vector<f32, 3> h  = vector<f32, 3>{ sin_theta * cos(phi), sin_theta * sin(phi), cos_theta };
      const vector<f32, 3> up = abs(normal.z) < 0.999f ? vector<f32, 3>{ 0.0f, 0.0f, 1.0f } : vector<f32, 3>{ 1.0f, 0.0f, 0.0f };

      const vector<f32, 3> tangent_x = normalize(cross(up, normal));
      const vector<f32, 3> tangent_y = normalize(cross(normal, tangent_x));

      return normalize(tangent_x * h.x + tangent_y * h.y + normal * h.z);
}

static f32 g_schlicksmith_ggx(f32 dot_nl, f32 dot_nv, f32 roughness)
{
      const f32 k  = (roughness * roughness) / 2.0f;
      const f32 gl = dot_nl / (dot_nl * (1.0f - k) + k);
      const f32 gv = dot_nv / (dot_nv * (1.0f - k) + k);
      return gl * gv;
}

static vector<f32, 2> brdf(f32 n_dot_v, f32 roughness, u32 num_samples)
{
      const vector<f32, 3> n = vector<f32, 3>{ 0.0f, 0.0f, 1.0f };
      const vector<f32, 3> v = vector<f32, 3>{ sqrt(1.0f - n_dot_v * n_dot_v), 0.0f, n_dot_v };

      vector<f32, 2> lut = vector<f32, 2>{ 0.0f, 0.0f };
      for (u32 i = 0u; i < num_samples; i++)
      {
            const vector<f32, 2> xi = hammersley_2d(i, num_samples);
            const vector<f32, 3> h  = importance_sample_ggx(xi, roughness, n);
            const vector<f32, 3> l  = 2.0f * dot(v, h) * h - v;

            const f32 dot_nl = max(dot(n, l), 0.0f);
            const f32 dot_nv = max(dot(n, v), 0.0f);
            const f32 dot_vh = max(dot(v, h), 0.0f);
            const f32 dot_nh = max(dot(h, n), 0.0f);

            if (dot_nl > 0.0f)
            {
                  const f32 g       = g_schlicksmith_ggx(dot_nl, dot_nv, roughness);
                  const f32 g_vis   = (g * dot_vh) / (dot_nh * dot_nv);
                  const f32 fresnel = pow(1.0f - dot_vh, 5.0f);
                  lut               = lut + vector<f32, 2>{ (1.0f - fresnel) * g_vis, fresnel * g_vis };
            }
      }
      return lut / f32(num_samples);
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const u32 NUM_SAMPLES = 1024u;

      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 2> lut = brdf(input.uv.x, input.uv.y, NUM_SAMPLES);
      output.color             = vector<f32, 4>{ lut.x, lut.y, 0.0f, 1.0f };
      return output;
}
