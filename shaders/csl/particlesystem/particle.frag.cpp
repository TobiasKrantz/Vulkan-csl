#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> smoke;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> fire;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 4> color;
      [[csl::location(1)]] f32            alpha;
      [[csl::location(2)]] i32            type;
      [[csl::location(3)]] f32            rotation;
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

      const f32 alpha = (input.alpha <= 1.0f) ? input.alpha : 2.0f - input.alpha;

      const f32 rotation_center = 0.5f;
      const f32 rotation_cos    = cos(input.rotation);
      const f32 rotation_sin    = sin(input.rotation);

      const vector<f32, 2> point = point_coord();
      const vector<f32, 2> rotated_uv = vector<f32, 2>{
            rotation_cos * (point.x - rotation_center) + rotation_sin * (point.y - rotation_center) + rotation_center,
            rotation_cos * (point.y - rotation_center) - rotation_sin * (point.x - rotation_center) + rotation_center
      };

      vector<f32, 4> color;
      f32            out_alpha;
      if (input.type == 0)
      {
            color     = textures.fire.sample(rotated_uv);
            out_alpha = 0.0f;
      }
      else
      {
            color     = textures.smoke.sample(rotated_uv);
            out_alpha = color.w * alpha;
      }

      const vector<f32, 3> rgb = color.xyz * input.color.xyz * alpha;
      output.color             = vector<f32, 4>{ rgb.x, rgb.y, rgb.z, out_alpha };
      return output;
}
