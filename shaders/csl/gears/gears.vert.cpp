#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      vector<f32, 4>    light_position;
      matrix<f32, 4, 4> model[3];
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
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const matrix<f32, 4, 4> model = scene.ubo->model[instance_index()];

      output.normal = normalize(mat3(model) * input.normal);
      output.color  = input.color;

      const matrix<f32, 4, 4> model_view = scene.ubo->view * model;
      const vector<f32, 4>    position   = model_view * input.position;
      output.eye_position                = (model_view * position).xyz;

      const vector<f32, 4> light_local    = vector<f32, 4>{ scene.ubo->light_position.x, scene.ubo->light_position.y, scene.ubo->light_position.z, 1.0f };
      const vector<f32, 4> light_position = light_local * model_view;
      output.light_vector                 = normalize(light_position.xyz - output.eye_position);

      output.clip = scene.ubo->projection * position;
      return output;
}
