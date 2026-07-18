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
      p3 = fract(p3 * vector<f32, 3>{ 443.897f, 441.423f, 437.195f });
      p3 += dot(p3, p3.yxz + vector<f32, 3>{ 19.19f, 19.19f, 19.19f });
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
            color += vector<f32, 3>{ star, star, star };
      }
      return color;
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> stars = star_field(input.uvw);
      output.color               = vector<f32, 4>{ stars.x, stars.y, stars.z, 1.0f };
      return output;
}
