#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resources_ = resources<Resources>();
      FragmentInput   input      = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 4> sampled = resources_.color_map.sample(input.uv);

      const f32            diffuse_factor = max(dot(input.normal, input.light_vector), 0.0f);
      const vector<f32, 4> ambient        = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      const vector<f32, 4> diffuse        = vector<f32, 4>{ 1.0f, 1.0f, 1.0f, 1.0f } * diffuse_factor;
      const vector<f32, 4> texel          = vector<f32, 4>{ sampled.x, sampled.y, sampled.z, 1.0f };

      output.color = (ambient + diffuse) * texel;
      return output;
}
