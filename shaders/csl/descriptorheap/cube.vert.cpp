#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model[2];
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo[2];
};

struct PushConstants
{
      i32 sampler_index;
      i32 frame_index;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] i32            instance_index;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Resources resources_ = resources<Resources>();
      VertexInput     input      = stage_input<VertexInput>();
      VertexOutput    output;

      output.normal         = input.normal;
      output.color          = input.color;
      output.uv             = input.uv;
      output.instance_index = static_cast<i32>(instance_index());

      const u32            frame = static_cast<u32>(push.frame_index);
      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip = resources_.ubo[frame]->projection * resources_.ubo[frame]->view * resources_.ubo[frame]->model[instance_index()] * local;
      return output;
}
