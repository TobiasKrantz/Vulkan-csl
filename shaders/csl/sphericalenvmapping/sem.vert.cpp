#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> normal;
      matrix<f32, 4, 4> view;
      i32               texture_index;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> color;
      [[csl::location(1)]] vector<f32, 3> eye_position;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] i32            texture_index;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = input.color;

      const matrix<f32, 4, 4> model_view = scene.ubo->view * scene.ubo->model;

      output.eye_position   = normalize((model_view * input.position).xyz);
      output.texture_index  = scene.ubo->texture_index;
      output.normal         = normalize(mat3(scene.ubo->normal) * input.normal);
      output.clip           = scene.ubo->projection * model_view * input.position;
      return output;
}
