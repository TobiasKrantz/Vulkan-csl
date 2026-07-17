#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
      vector<f32, 3>    camera_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 4> tangent;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> world_position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 4> tangent;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 4> local          = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 3> world_position = (scene.ubo->model * local).xyz;
      const vector<f32, 3> world_tangent  = mat3(scene.ubo->model) * input.tangent.xyz;

      output.world_position = world_position;
      output.normal         = mat3(scene.ubo->model) * input.normal;
      output.tangent        = vector<f32, 4>{ world_tangent.x, world_tangent.y, world_tangent.z, input.tangent.w };
      output.uv             = input.uv;
      output.clip           = scene.ubo->projection * scene.ubo->view * vector<f32, 4>{ world_position.x, world_position.y, world_position.z, 1.0f };
      return output;
}
