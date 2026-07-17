#include <csl/csl.h>

using namespace csl;

struct UboScene
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      vector<f32, 4>    light_position;
      vector<f32, 4>    view_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const UboScene* ubo;
};

struct PushConstants
{
      matrix<f32, 4, 4> model;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
      [[csl::location(4)]] vector<f32, 4> tangent;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
      [[csl::location(5)]] vector<f32, 4> tangent;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color   = input.color;
      output.uv      = input.uv;
      output.tangent = input.tangent;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * push.model * local;

      output.normal = mat3(push.model) * input.normal;

      const vector<f32, 4> position = push.model * local;
      output.light_vector           = scene.ubo->light_position.xyz - position.xyz;
      output.view_vector            = scene.ubo->view_position.xyz - position.xyz;
      return output;
}
