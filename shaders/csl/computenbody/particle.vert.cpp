#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 2>    screen_dimensions;
};

struct Resources
{
      [[csl::uniform_buffer(0, 2)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 4> velocity;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4> clip;
      [[csl::point_size]] f32          point_size;
      [[csl::location(0)]] f32         gradient_pos;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Resources resources_ = resources<Resources>();
      VertexInput     input      = stage_input<VertexInput>();
      VertexOutput    output;

      const f32 sprite_size = 0.005f * input.position.w;

      const vector<f32, 4> eye_position     = resources_.ubo->modelview * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 4> projected_corner = resources_.ubo->projection * vector<f32, 4>{ 0.5f * sprite_size, 0.5f * sprite_size, eye_position.z, eye_position.w };

      output.point_size   = clamp(resources_.ubo->screen_dimensions.x * projected_corner.x / projected_corner.w, 1.0f, 128.0f);
      output.clip         = resources_.ubo->projection * eye_position;
      output.gradient_pos = input.velocity.w;
      return output;
}
