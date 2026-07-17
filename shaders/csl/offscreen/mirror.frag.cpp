#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 4> position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      vector<f32, 4> projected = input.position * (1.0f / input.position.w);
      projected                = projected + 1.0f;
      projected                = projected * 0.5f;

      const f32 blur_size = 1.0f / 512.0f;
      output.color        = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };

      if (front_facing())
      {
            vector<f32, 4> reflection = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
            for (i32 x = -3; x <= 3; ++x)
                  for (i32 y = -3; y <= 3; ++y)
                        reflection = reflection + textures.color.sample(vector<f32, 2>{ projected.x + f32(x) * blur_size, projected.y + f32(y) * blur_size }) / 49.0f;
            output.color = output.color + reflection;
      }
      return output;
}
