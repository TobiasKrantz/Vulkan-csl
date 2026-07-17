#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> shadow_cube_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 3> world_position;
      [[csl::location(5)]] vector<f32, 3> light_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      constexpr f32 EPSILON        = 0.15f;
      constexpr f32 SHADOW_OPACITY = 0.5f;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 4> ambient = vector<f32, 4>{ 0.05f, 0.05f, 0.05f, 1.0f };
      const vector<f32, 4> diffuse = vector<f32, 4>{ 1.0f, 1.0f, 1.0f, 1.0f } * max(dot(input.normal, input.light_vector), 0.0f);

      output.color = ambient + diffuse * vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };

      const vector<f32, 3> light_vector = input.world_position - input.light_position;
      const f32            sampled_dist = resource.shadow_cube_map.sample(light_vector).r;
      const f32            dist         = length(light_vector);

      const f32 shadow = (dist <= sampled_dist + EPSILON) ? 1.0f : SHADOW_OPACITY;
      output.color.xyz = output.color.xyz * shadow;
      return output;
}
