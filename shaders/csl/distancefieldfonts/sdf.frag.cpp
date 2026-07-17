#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    outline_color;
      f32               outline_width;
      f32               outline;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color;
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
      const Scene    scene = resources<Scene>();
      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      const f32 distance_value = scene.color.sample(input.uv).w;
      const f32 smooth_width   = fwidth(distance_value);
      f32       alpha          = smooth_step(0.5f - smooth_width, 0.5f + smooth_width, distance_value);
      vector<f32, 3> rgb       = vector<f32, 3>{ alpha, alpha, alpha };

      if (scene.ubo->outline > 0.0f)
      {
            const f32 width = 1.0f - scene.ubo->outline_width;
            alpha           = smooth_step(width - smooth_width, width + smooth_width, distance_value);
            const vector<f32, 3> amount = vector<f32, 3>{ alpha, alpha, alpha };
            rgb += lerp(amount, scene.ubo->outline_color.xyz, amount);
      }

      output.color = vector<f32, 4>{ rgb.x, rgb.y, rgb.z, alpha };
      return output;
}
