#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::input_attachment(0, 1, 0)]] SubpassInput<f32>            position_depth;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> texture;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> color;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static f32 linear_depth(f32 depth, f32 near_plane, f32 far_plane)
{
      const f32 z = depth * 2.0f - 1.0f;
      return (2.0f * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const f32 near_plane = 0.1f;
      [[csl::constant(1)]] const f32 far_plane  = 256.0f;

      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const f32            depth         = resources_.position_depth.load().w;
      const vector<f32, 4> sampled_color = resources_.texture.sample(input.uv);

      if ((depth != 0.0f) && (linear_depth(frag_coord().z, near_plane, far_plane) > depth))
      {
            discard();
      }

      output.color = sampled_color;
      return output;
}
