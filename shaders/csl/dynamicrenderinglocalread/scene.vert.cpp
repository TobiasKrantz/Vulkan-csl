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

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 4> color;
      [[csl::location(2)]] vector<f32, 3> world_position;
      [[csl::location(3)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local;

      vector<f32, 3> world_position = (scene.ubo->model * local).xyz;
      world_position.y              = -world_position.y;
      output.world_position         = world_position;

      const matrix<f32, 3, 3> normal_matrix = transpose(inverse(mat3(scene.ubo->model)));
      output.normal                         = normalize(normal_matrix * input.normal);

      output.color = vector<f32, 4>{ input.color.x, input.color.y, input.color.z, 1.0f };
      output.uv    = input.uv;
      return output;
}
