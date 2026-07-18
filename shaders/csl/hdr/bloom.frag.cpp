#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color0;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color1;
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
      [[csl::constant(0)]] const i32 direction = 0;

      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const f32 weights[25] = { 0.0024499299678342f, 0.0043538453346397f, 0.0073599963704157f, 0.0118349786570722f, 0.0181026699707781f, 0.0263392293891488f, 0.0364543006660986f,
                                0.0479932050577658f, 0.0601029809166942f, 0.0715974486241365f, 0.0811305381519717f, 0.0874493212267511f, 0.0896631113333857f, 0.0874493212267511f,
                                0.0811305381519717f, 0.0715974486241365f, 0.0601029809166942f, 0.0479932050577658f, 0.0364543006660986f, 0.0263392293891488f, 0.0181026699707781f,
                                0.0118349786570722f, 0.0073599963704157f, 0.0043538453346397f, 0.0024499299678342f };

      const f32 blur_scale    = 0.003f;
      const f32 blur_strength = 1.0f;

      f32 aspect = 1.0f;
      if (direction == 1)
      {
            const vector<u32, 2> size = textures.color1.size(0);
            aspect                    = f32(size.y) / f32(size.x);
      }

      const vector<f32, 2> origin = input.uv.yx - vector<f32, 2>{ 0.0f, 12.0f * aspect * blur_scale };

      vector<f32, 4> color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      for (i32 i = 0; i < 25; i++)
      {
            const vector<f32, 2> offset = vector<f32, 2>{ 0.0f, f32(i) * blur_scale } * aspect;
            color                       = color + textures.color1.sample(origin + offset) * weights[i] * blur_strength;
      }

      output.color = color;
      return output;
}
