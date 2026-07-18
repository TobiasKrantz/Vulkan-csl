#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct PushConstants
{
      vector<f32, 4> color;
      vector<f32, 4> position;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> color;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = input.color * push.color.xyz;

      const vector<f32, 4> local     = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 3> local_pos = (scene.ubo->model * local).xyz;
      const vector<f32, 3> world_pos = local_pos + push.position.xyz;
      output.clip                    = scene.ubo->projection * scene.ubo->view * vector<f32, 4>{ world_pos.x, world_pos.y, world_pos.z, 1.0f };
      return output;
}
