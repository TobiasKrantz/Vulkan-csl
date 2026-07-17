#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> shadow_cube_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;
      output.color = vector<f32, 4>{ 0.05f, 0.05f, 0.05f, 1.0f };

      vector<f32, 3> sample_position = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      const i32 x = i32(floor(input.uv.x / 0.25f));
      const i32 y = i32(floor(input.uv.y / (1.0f / 3.0f)));

      if (y == 1)
      {
            vector<f32, 2> uv = vector<f32, 2>{ input.uv.x * 4.0f, (input.uv.y - 1.0f / 3.0f) * 3.0f };
            uv                = 2.0f * vector<f32, 2>{ uv.x - f32(x), uv.y } - 1.0f;
            switch (x)
            {
            case 0: sample_position = vector<f32, 3>{ -1.0f, uv.y, uv.x }; break;
            case 1: sample_position = vector<f32, 3>{ uv.x, uv.y, 1.0f }; break;
            case 2: sample_position = vector<f32, 3>{ 1.0f, uv.y, -uv.x }; break;
            case 3: sample_position = vector<f32, 3>{ -uv.x, uv.y, -1.0f }; break;
            }
      }
      else if (x == 1)
      {
            vector<f32, 2> uv = vector<f32, 2>{ (input.uv.x - 0.25f) * 4.0f, (input.uv.y - f32(y) / 3.0f) * 3.0f };
            uv                = 2.0f * uv - 1.0f;
            switch (y)
            {
            case 0: sample_position = vector<f32, 3>{ uv.x, -1.0f, uv.y }; break;
            case 2: sample_position = vector<f32, 3>{ uv.x, 1.0f, -uv.y }; break;
            }
      }

      if ((sample_position.x != 0.0f) && (sample_position.y != 0.0f))
      {
            const f32 dist = length(resource.shadow_cube_map.sample(sample_position).xyz) * 0.005f;
            output.color   = vector<f32, 4>{ dist, dist, dist, 1.0f };
      }
      return output;
}
