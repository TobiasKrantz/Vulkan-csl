#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> uvw;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static f32 hash33(vector<f32, 3> p3)
{
      const vector<f32, 3> scale = vector<f32, 3>{ 443.897f, 441.423f, 437.195f };
      p3                         = fract(p3 * scale);
      p3                         = p3 + dot(p3, vector<f32, 3>{ p3.y, p3.x, p3.z } + vector<f32, 3>{ 19.19f, 19.19f, 19.19f });
      return fract((p3.x + p3.y) * p3.z + (p3.x + p3.z) * p3.y + (p3.y + p3.z) * p3.x);
}

static vector<f32, 3> star_field(vector<f32, 3> position)
{
      vector<f32, 3> color     = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      const f32      threshold = 1.0f - 0.01f;
      const f32      random    = hash33(position);
      if (random >= threshold)
      {
            const f32 star = pow((random - threshold) / (1.0f - threshold), 16.0f);
            color          = color + vector<f32, 3>{ star, star, star };
      }
      return color;
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const f32            band       = input.uvw.y + 0.25f;
      const vector<f32, 3> atmosphere = clamp(vector<f32, 3>{ 0.1f, 0.15f, 0.4f } * band, vector<f32, 3>{ 0.0f, 0.0f, 0.0f }, vector<f32, 3>{ 1.0f, 1.0f, 1.0f });

      const vector<f32, 3> color = star_field(input.uvw) + atmosphere;
      output.color               = vector<f32, 4>{ color.x, color.y, color.z, 1.0f };
      return output;
}
