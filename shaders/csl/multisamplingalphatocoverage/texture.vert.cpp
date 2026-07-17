#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    light_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct PushConstants
{
      vector<f32, 3> position;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 3> local_position = input.position + push.position;
      const vector<f32, 4> local          = vector<f32, 4>{ local_position.x, local_position.y, local_position.z, 1.0f };

      output.uv   = input.uv;
      output.clip = scene.ubo->projection * scene.ubo->model * local;

      const vector<f32, 4> position = scene.ubo->model * local;
      output.normal                 = mat3(scene.ubo->model) * input.normal;
      const vector<f32, 3> light    = mat3(scene.ubo->model) * scene.ubo->light_position.xyz;
      output.light_vector           = light - position.xyz;
      output.view_vector            = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - position.xyz;
      return output;
}
