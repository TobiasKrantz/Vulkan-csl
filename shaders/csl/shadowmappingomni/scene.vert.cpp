#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    light_position;
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
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 3> world_position;
      [[csl::location(5)]] vector<f32, 3> light_position;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color  = input.color;
      output.normal = input.normal;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local;

      output.eye_position   = (scene.ubo->model * local).xyz;
      output.light_vector   = normalize(scene.ubo->light_position.xyz - input.position);
      output.world_position = input.position;
      output.light_position = scene.ubo->light_position.xyz;
      return output;
}
