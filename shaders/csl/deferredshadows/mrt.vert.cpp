#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
      vector<f32, 4>    instance_position[3];
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> normal;
      [[csl::location(4)]] vector<f32, 4> tangent;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> world_position;
      [[csl::location(4)]] vector<f32, 3> tangent;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 4> local_position = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f } + scene.ubo->instance_position[instance_index()];

      output.clip = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local_position;

      output.uv = input.uv;

      output.world_position = (scene.ubo->model * local_position).xyz;

      const matrix<f32, 3, 3> normal_matrix = transpose(inverse(mat3(scene.ubo->model)));
      output.normal                         = normal_matrix * normalize(input.normal);
      output.tangent                        = normal_matrix * normalize(input.tangent.xyz);

      output.color = input.color;
      return output;
}
