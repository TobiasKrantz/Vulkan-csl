#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(1, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> color;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 4> color;
      [[csl::location(2)]] vector<f32, 3> world_position;
      [[csl::location(3)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
      [[csl::location(1)]] vector<f32, 4> position_depth;
      [[csl::location(2)]] vector<f32, 4> normal;
      [[csl::location(3)]] vector<f32, 4> albedo;
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

      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      vector<f32, 4> position_depth = vector<f32, 4>{ input.world_position.x, input.world_position.y, input.world_position.z, 1.0f };

      vector<f32, 3> normal = normalize(input.normal);
      normal.y              = -normal.y;
      output.normal         = vector<f32, 4>{ normal.x, normal.y, normal.z, 1.0f };

      const vector<f32, 4> sampled = textures.color.sample(input.uv);
      if (sampled.w < 0.5f)
      {
            discard();
      }
      output.albedo = sampled * input.color;

      position_depth.w      = linear_depth(frag_coord().z, near_plane, far_plane);
      output.position_depth = position_depth;

      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      return output;
}
