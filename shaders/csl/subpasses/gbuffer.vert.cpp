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
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> world_position;
      [[csl::location(3)]] vector<f32, 3> tangent;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.clip = scene.ubo->projection * scene.ubo->view * scene.ubo->model * input.position;

      const vector<f32, 4> world = scene.ubo->model * input.position;
      output.world_position      = world.xyz;
      output.world_position.y    = -output.world_position.y;

      const matrix<f32, 3, 3> normal_matrix = transpose(inverse(mat3(scene.ubo->model)));
      output.normal                         = normal_matrix * normalize(input.normal);

      output.color = input.color;
      return output;
}
