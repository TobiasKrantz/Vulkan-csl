#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> world_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
      [[csl::location(1)]] vector<f32, 4> position;
      [[csl::location(2)]] vector<f32, 4> normal;
      [[csl::location(3)]] vector<f32, 4> albedo;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const f32 near_plane = 0.1f;
      [[csl::constant(1)]] const f32 far_plane  = 256.0f;

      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      output.position = vector<f32, 4>{ input.world_position.x, input.world_position.y, input.world_position.z, 1.0f };

      vector<f32, 3> normal = normalize(input.normal);
      normal.y              = -normal.y;
      output.normal         = vector<f32, 4>{ normal.x, normal.y, normal.z, 1.0f };

      output.albedo = vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      const f32 depth   = frag_coord().z;
      const f32 z       = depth * 2.0f - 1.0f;
      output.position.a = (2.0f * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));

      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      return output;
}
