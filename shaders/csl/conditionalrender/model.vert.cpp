#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
};

struct Node
{
      matrix<f32, 4, 4> matrix;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*  ubo;
      [[csl::uniform_buffer(1, 0)]] const Node* node;
};

struct PushConstants
{
      vector<f32, 4> base_color_factor;
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
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = push.base_color_factor.xyz;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * scene.node->matrix * local;

      output.normal = mat3(scene.ubo->view * scene.ubo->model * scene.node->matrix) * input.normal;

      const vector<f32, 4> local_position = scene.ubo->view * scene.ubo->model * scene.node->matrix * local;
      const vector<f32, 3> light_position = vector<f32, 3>{ 10.0f, -10.0f, 10.0f };
      output.light_vector                 = light_position - local_position.xyz;
      output.view_vector                  = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - local_position.xyz;
      return output;
}
