#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(1, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
      [[csl::location(5)]] vector<f32, 4> tangent;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const bool alpha_mask        = false;
      [[csl::constant(1)]] const f32  alpha_mask_cutoff = 0.0f;

      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 4> color = textures.color_map.sample(input.uv) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      if (alpha_mask)
      {
            if (color.w < alpha_mask_cutoff)
            {
                  discard();
            }
      }

      const vector<f32, 3> t = normalize(input.tangent.xyz);
      const vector<f32, 3> b = cross(input.normal, input.tangent.xyz) * input.tangent.w;
      const vector<f32, 3> n = normalize(input.normal);

      const matrix<f32, 3, 3> tbn     = mat3(t, b, n);
      const vector<f32, 3>    sampled = textures.normal_map.sample(input.uv).xyz * 2.0f - vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      const vector<f32, 3>    normal  = tbn * normalize(sampled);

      const f32            ambient  = 0.1f;
      const vector<f32, 3> light    = normalize(input.light_vector);
      const vector<f32, 3> view     = normalize(input.view_vector);
      const vector<f32, 3> reflected = reflect(-light, normal);

      const f32            shade    = max(dot(normal, light), ambient);
      const vector<f32, 3> diffuse  = vector<f32, 3>{ shade, shade, shade };
      const f32            specular = pow(max(dot(reflected, view), 0.0f), 32.0f);

      const vector<f32, 3> lit = diffuse * color.xyz + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, color.w };
      return output;
}
