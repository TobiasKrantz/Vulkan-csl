#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> world_position;
      [[csl::location(4)]] vector<f32, 3> tangent;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 4> normal;
      [[csl::location(2)]] vector<f32, 4> albedo;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      output.position = vector<f32, 4>{ input.world_position.x, input.world_position.y, input.world_position.z, 1.0f };

      const vector<f32, 3>    n   = normalize(input.normal);
      const vector<f32, 3>    t   = normalize(input.tangent);
      const vector<f32, 3>    b   = cross(n, t);
      const matrix<f32, 3, 3> tbn = mat3(t, b, n);

      const vector<f32, 3> sampled_normal = resources_.normal_map.sample(input.uv).xyz * 2.0f - vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      const vector<f32, 3> tangent_normal = tbn * normalize(sampled_normal);
      output.normal                       = vector<f32, 4>{ tangent_normal.x, tangent_normal.y, tangent_normal.z, 1.0f };

      output.albedo = resources_.color.sample(input.uv);
      return output;
}
