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

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = input.color;
      output.uv    = input.uv;
      output.clip  = scene.ubo->projection * scene.ubo->view * scene.ubo->model * input.position;

      const vector<f32, 3> light_position = vector<f32, 3>{ -5.0f, -5.0f, 0.0f };
      const vector<f32, 4> position       = scene.ubo->view * scene.ubo->model * input.position;
      output.normal                       = mat3(scene.ubo->view * scene.ubo->model) * input.normal;
      output.light_vector                 = light_position - position.xyz;
      output.view_vector                  = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - position.xyz;
      return output;
}
