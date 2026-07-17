#include <csl/csl.h>

using namespace csl;

#define PI 3.1415926535897932384626433832795f

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
      vector<f32, 3>    camera_position;
};

struct UboParams
{
      vector<f32, 4> lights[4];
      f32            exposure;
      f32            gamma;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                               ubo;
      [[csl::uniform_buffer(0, 1)]] const UboParams*                         params;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> irradiance_map;
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D>       brdf_lut;
      [[csl::binding(0, 4)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> prefiltered_map;
      [[csl::binding(0, 5)]] CombinedSampler<f32, ImageType::IMAGE_2D>       albedo_map;
      [[csl::binding(0, 6)]] CombinedSampler<f32, ImageType::IMAGE_2D>       normal_map;
      [[csl::binding(0, 7)]] CombinedSampler<f32, ImageType::IMAGE_2D>       ao_map;
      [[csl::binding(0, 8)]] CombinedSampler<f32, ImageType::IMAGE_2D>       metallic_map;
      [[csl::binding(0, 9)]] CombinedSampler<f32, ImageType::IMAGE_2D>       roughness_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> world_position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 4> tangent;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static vector<f32, 3> uncharted2_tonemap(vector<f32, 3> x)
{
      const f32 a = 0.15f;
      const f32 b = 0.50f;
      const f32 c = 0.10f;
      const f32 d = 0.20f;
      const f32 e = 0.02f;
      const f32 f = 0.30f;
      return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

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

static vector<f32, 3> f_schlick(f32 cos_theta, vector<f32, 3> f0)
{
      return f0 + (vector<f32, 3>{ 1.0f, 1.0f, 1.0f } - f0) * pow(1.0f - cos_theta, 5.0f);
}

static vector<f32, 3> f_schlick_roughness(f32 cos_theta, vector<f32, 3> f0, f32 roughness)
{
      const vector<f32, 3> ceiling = vector<f32, 3>{ 1.0f - roughness, 1.0f - roughness, 1.0f - roughness };
      return f0 + (max(ceiling, f0) - f0) * pow(1.0f - cos_theta, 5.0f);
}

static vector<f32, 3> specular_contribution(vector<f32, 3> l, vector<f32, 3> v, vector<f32, 3> n, vector<f32, 3> f0, f32 metallic, f32 roughness, vector<f32, 3> albedo)
{
      const vector<f32, 3> h      = normalize(v + l);
      const f32            dot_nh = clamp(dot(n, h), 0.0f, 1.0f);
      const f32            dot_nv = clamp(dot(n, v), 0.0f, 1.0f);
      const f32            dot_nl = clamp(dot(n, l), 0.0f, 1.0f);

      vector<f32, 3> color = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      if (dot_nl > 0.0f)
      {
            const f32            d    = d_ggx(dot_nh, roughness);
            const f32            g    = g_schlicksmith_ggx(dot_nl, dot_nv, roughness);
            const vector<f32, 3> f    = f_schlick(dot_nv, f0);
            const vector<f32, 3> spec = d * f * g / (4.0f * dot_nl * dot_nv + 0.001f);
            const vector<f32, 3> kd   = (vector<f32, 3>{ 1.0f, 1.0f, 1.0f } - f) * (1.0f - metallic);
            color                     = color + (kd * albedo / PI + spec) * dot_nl;
      }
      return color;
}

static vector<f32, 3> calculate_normal(vector<f32, 3> tangent_normal_sample, vector<f32, 3> in_normal, vector<f32, 3> in_tangent)
{
      const vector<f32, 3>    tangent_normal = tangent_normal_sample * 2.0f - 1.0f;
      const vector<f32, 3>    n              = normalize(in_normal);
      const vector<f32, 3>    t              = normalize(in_tangent);
      const vector<f32, 3>    b              = normalize(cross(n, t));
      const matrix<f32, 3, 3> tbn            = mat3(t, b, n);
      return normalize(tbn * tangent_normal);
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources res   = resources<Resources>();
      FragmentInput   input = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> albedo = pow(res.albedo_map.sample(input.uv).xyz, vector<f32, 3>{ 2.2f, 2.2f, 2.2f });

      const vector<f32, 3> tangent_normal = res.normal_map.sample(input.uv).xyz;
      const vector<f32, 3> n              = calculate_normal(tangent_normal, input.normal, input.tangent.xyz);

      const vector<f32, 3> v = normalize(res.ubo->camera_position - input.world_position);
      const vector<f32, 3> r = reflect(-v, n);

      const f32 metallic  = res.metallic_map.sample(input.uv).x;
      const f32 roughness = res.roughness_map.sample(input.uv).x;

      const vector<f32, 3> f0 = lerp(vector<f32, 3>{ 0.04f, 0.04f, 0.04f }, albedo, vector<f32, 3>{ metallic, metallic, metallic });

      vector<f32, 3> lo = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      for (i32 i = 0; i < 4; i++)
      {
            const vector<f32, 3> l = normalize(res.params->lights[i].xyz - input.world_position);
            lo                     = lo + specular_contribution(l, v, n, f0, metallic, roughness, albedo);
      }

      const f32            n_dot_v = max(dot(n, v), 0.0f);
      const vector<f32, 2> brdf    = res.brdf_lut.sample(vector<f32, 2>{ n_dot_v, roughness }).xy;

      const f32            lod          = roughness * 9.0f;
      const f32            lodf         = floor(lod);
      const f32            lodc         = ceil(lod);
      const vector<f32, 3> reflect_low  = res.prefiltered_map.sample_lod(r, lodf).xyz;
      const vector<f32, 3> reflect_high = res.prefiltered_map.sample_lod(r, lodc).xyz;
      const vector<f32, 3> reflection   = lerp(reflect_low, reflect_high, vector<f32, 3>{ lod - lodf, lod - lodf, lod - lodf });

      const vector<f32, 3> irradiance = res.irradiance_map.sample(n).xyz;
      const vector<f32, 3> diffuse    = irradiance * albedo;

      const vector<f32, 3> f        = f_schlick_roughness(n_dot_v, f0, roughness);
      const vector<f32, 3> specular = reflection * (f * brdf.x + brdf.y);

      const vector<f32, 3> kd      = (vector<f32, 3>{ 1.0f, 1.0f, 1.0f } - f) * (1.0f - metallic);
      const f32            ao      = res.ao_map.sample(input.uv).x;
      const vector<f32, 3> ambient = (kd * diffuse + specular) * vector<f32, 3>{ ao, ao, ao };

      vector<f32, 3> color = ambient + lo;

      color                      = uncharted2_tonemap(color * res.params->exposure);
      const vector<f32, 3> white = vector<f32, 3>{ 11.2f, 11.2f, 11.2f };
      color                      = color * (1.0f / uncharted2_tonemap(white));
      const f32 inv_gamma        = 1.0f / res.params->gamma;
      color                      = pow(color, vector<f32, 3>{ inv_gamma, inv_gamma, inv_gamma });

      output.color = vector<f32, 4>{ color.x, color.y, color.z, 1.0f };
      return output;
}
