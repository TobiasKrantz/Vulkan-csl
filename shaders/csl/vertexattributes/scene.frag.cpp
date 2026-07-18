#include <csl/csl.h>

using namespace csl;

struct Push
{
      matrix<f32, 4, 4> model;
      u32               alpha_mask;
      f32               alpha_cutoff;
};

struct Textures
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(1, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 4> tangent;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main(Push push)
{
      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 4> color = textures.color_map.sample(input.uv);

      if (push.alpha_mask == 1 && color.w < push.alpha_cutoff)
            discard();

      const vector<f32, 3>    mapped   = textures.normal_map.sample(input.uv).xyz * 2.0f - 1.0f;
      const vector<f32, 3>    tangent  = normalize(input.tangent.xyz);
      const vector<f32, 3>    bitangnt = cross(input.normal, input.tangent.xyz) * input.tangent.w;
      const matrix<f32, 3, 3> tbn      = mat3(tangent, bitangnt, normalize(input.normal));
      const vector<f32, 3>    n        = tbn * normalize(mapped);

      const f32            ambient  = 0.1f;
      const vector<f32, 3> l        = normalize(input.light_vector);
      const vector<f32, 3> v        = normalize(input.view_vector);
      const vector<f32, 3> r        = reflect(-l, n);
      const f32            shade    = max(dot(n, l), ambient);
      const vector<f32, 3> diffuse  = vector<f32, 3>{ shade, shade, shade };
      const f32            specular = pow(max(dot(r, v), 0.0f), 32.0f);
      const vector<f32, 3> lit      = diffuse * color.xyz + specular;

      output.color = vector<f32, 4>{ lit.x, lit.y, lit.z, color.w };
      return output;
}
