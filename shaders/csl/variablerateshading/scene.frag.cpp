#include <csl/csl.h>

using namespace csl;

constexpr u32 SHADING_RATE_2_VERTICAL_PIXELS   = 1;
constexpr u32 SHADING_RATE_4_VERTICAL_PIXELS   = 2;
constexpr u32 SHADING_RATE_2_HORIZONTAL_PIXELS = 4;
constexpr u32 SHADING_RATE_4_HORIZONTAL_PIXELS = 8;

struct UboScene
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    light_position;
      vector<f32, 4>    view_position;
      i32               color_shading_rates;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const UboScene*                   scene;
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
      [[csl::constant(0)]] const bool ALPHA_MASK        = false;
      [[csl::constant(1)]] const f32  ALPHA_MASK_CUTOFF = 0.0f;

      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      vector<f32, 4> color = resources_.color_map.sample(input.uv) * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      if (ALPHA_MASK)
            if (color.w < ALPHA_MASK_CUTOFF)
                  discard();

      vector<f32, 3>    N   = normalize(input.normal);
      vector<f32, 3>    T   = normalize(input.tangent.xyz);
      vector<f32, 3>    B   = cross(input.normal, input.tangent.xyz) * input.tangent.w;
      matrix<f32, 3, 3> TBN = mat3(T, B, N);
      N                     = TBN * normalize(resources_.normal_map.sample(input.uv).xyz * 2.0f - vector<f32, 3>{ 1.0f, 1.0f, 1.0f });

      const f32      ambient        = 0.25f;
      vector<f32, 3> L              = normalize(input.light_vector);
      vector<f32, 3> V              = normalize(input.view_vector);
      vector<f32, 3> R              = reflect(-L, N);
      f32            diffuse_amount = max(dot(N, L), ambient);
      vector<f32, 3> diffuse        = vector<f32, 3>{ diffuse_amount, diffuse_amount, diffuse_amount };
      f32            specular       = pow(max(dot(R, V), 0.0f), 32.0f);
      output.color                  = vector<f32, 4>{ diffuse.x * color.x + specular, diffuse.y * color.y + specular, diffuse.z * color.z + specular, color.w };

      if (resources_.scene->color_shading_rates == 1)
      {
            i32       v    = 1;
            i32       h    = 1;
            const u32 rate = shading_rate();
            if ((rate & SHADING_RATE_2_VERTICAL_PIXELS) == SHADING_RATE_2_VERTICAL_PIXELS)
                  v = 2;
            if ((rate & SHADING_RATE_4_VERTICAL_PIXELS) == SHADING_RATE_4_VERTICAL_PIXELS)
                  v = 4;
            if ((rate & SHADING_RATE_2_HORIZONTAL_PIXELS) == SHADING_RATE_2_HORIZONTAL_PIXELS)
                  h = 2;
            if ((rate & SHADING_RATE_4_HORIZONTAL_PIXELS) == SHADING_RATE_4_HORIZONTAL_PIXELS)
                  h = 4;

            vector<f32, 4> tint = vector<f32, 4>{ 0.0f, 0.8f, 0.4f, 1.0f };
            if (v == 2 && h == 1)
                  tint = vector<f32, 4>{ 0.2f, 0.6f, 1.0f, 1.0f };
            else if (v == 1 && h == 2)
                  tint = vector<f32, 4>{ 0.0f, 0.4f, 0.8f, 1.0f };
            else if (v == 2 && h == 2)
                  tint = vector<f32, 4>{ 1.0f, 1.0f, 0.2f, 1.0f };
            else if (v == 4 && h == 2)
                  tint = vector<f32, 4>{ 0.8f, 0.8f, 0.0f, 1.0f };
            else if (v == 2 && h == 4)
                  tint = vector<f32, 4>{ 1.0f, 0.4f, 0.2f, 1.0f };

            output.color = output.color * tint;
      }
      return output;
}
