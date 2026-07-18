#include <csl/csl.h>

using namespace csl;

#define PI 3.1415926536f

struct PushConstants
{
      matrix<f32, 4, 4> mvp;
      f32               roughness;
      u32               num_samples;
};

struct Resources
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> environment;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> position;
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

static f32 d_ggx(f32 dot_nh, f32 roughness)
{
      const f32 alpha  = roughness * roughness;
      const f32 alpha2 = alpha * alpha;
      const f32 denom  = dot_nh * dot_nh * (alpha2 - 1.0f) + 1.0f;
      return alpha2 / (PI * denom * denom);
}

[[csl::fragment]] FragmentOutput fragment_main(PushConstants push)
{
      const Resources res   = resources<Resources>();
      FragmentInput   input = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> n         = normalize(input.position);
      const vector<f32, 3> v         = n;
      const f32            roughness = push.roughness;
      const f32            env_dim   = f32(res.environment.size(0).x);

      vector<f32, 3> color        = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      f32            total_weight = 0.0f;
      for (u32 i = 0u; i < push.num_samples; i++)
      {
            const vector<f32, 2> xi     = hammersley_2d(i, push.num_samples);
            const vector<f32, 3> h      = importance_sample_ggx(xi, roughness, n);
            const vector<f32, 3> l      = 2.0f * dot(v, h) * h - v;
            const f32            dot_nl = clamp(dot(n, l), 0.0f, 1.0f);
            if (dot_nl <= 0.0f)
                  continue;

            const f32 dot_nh = clamp(dot(n, h), 0.0f, 1.0f);
            const f32 dot_vh = clamp(dot(v, h), 0.0f, 1.0f);

            const f32 pdf       = d_ggx(dot_nh, roughness) * dot_nh / (4.0f * dot_vh) + 0.0001f;
            const f32 omega_s   = 1.0f / (f32(push.num_samples) * pdf);
            const f32 omega_p   = 4.0f * PI / (6.0f * env_dim * env_dim);
            const f32 mip_level = roughness == 0.0f ? 0.0f : max(0.5f * log2(omega_s / omega_p) + 1.0f, 0.0f);

            color        = color + res.environment.sample_lod(l, mip_level).xyz * dot_nl;
            total_weight = total_weight + dot_nl;
      }

      const vector<f32, 3> prefiltered = color / total_weight;
      output.color                     = vector<f32, 4>{ prefiltered.x, prefiltered.y, prefiltered.z, 1.0f };
      return output;
}
