#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> light_space;
      vector<f32, 4>    light_position;
      f32               z_near;
      f32               z_far;
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
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 4> shadow_coordinate;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const matrix<f32, 4, 4> bias_matrix = mat4(vector<f32, 4>{ 0.5f, 0.0f, 0.0f, 0.0f }, vector<f32, 4>{ 0.0f, 0.5f, 0.0f, 0.0f }, vector<f32, 4>{ 0.0f, 0.0f, 1.0f, 0.0f },
                                                 vector<f32, 4>{ 0.5f, 0.5f, 0.0f, 1.0f });

      output.color = input.color;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local;

      const vector<f32, 4> position = scene.ubo->model * local;
      output.normal                 = mat3(scene.ubo->model) * input.normal;
      output.light_vector           = normalize(scene.ubo->light_position.xyz - input.position);
      output.view_vector            = -position.xyz;

      output.shadow_coordinate = (bias_matrix * scene.ubo->light_space * scene.ubo->model) * local;
      return output;
}
