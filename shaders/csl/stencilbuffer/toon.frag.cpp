#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> normal    = normalize(input.normal);
      const vector<f32, 3> light     = normalize(input.light_vector);
      const f32            intensity = dot(normal, light);

      vector<f32, 3> color = input.color * 0.2f;
      color                = intensity > 0.25f ? input.color * 0.4f : color;
      color                = intensity > 0.5f ? input.color * 0.6f : color;
      color                = intensity > 0.9f ? input.color * 1.0f : color;
      color                = intensity > 0.98f ? input.color * 1.5f : color;

      const f32 luminance = dot(vector<f32, 3>{ 0.2126f, 0.7152f, 0.0722f }, color);
      color               = lerp(color, vector<f32, 3>{ luminance, luminance, luminance }, vector<f32, 3>{ 0.1f, 0.1f, 0.1f });

      output.color     = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      output.color.xyz = color;
      return output;
}
