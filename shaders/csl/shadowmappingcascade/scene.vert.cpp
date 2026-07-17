#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct PushConstants
{
      vector<f32, 4> position;
      u32            cascade_index;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_position;
      [[csl::location(3)]] vector<f32, 3> position;
      [[csl::location(4)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color  = input.color;
      output.normal = input.normal;
      output.uv     = input.uv;

      const vector<f32, 3> position = input.position + push.position.xyz;
      output.position               = position;

      const vector<f32, 4> world = vector<f32, 4>{ position.x, position.y, position.z, 1.0f };
      output.view_position       = (scene.ubo->view * world).xyz;
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * world;
      return output;
}
