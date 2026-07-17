#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> normal;
      matrix<f32, 4, 4> view;
      vector<f32, 3>    light_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> eye_position;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv     = input.uv;
      output.normal = normalize(mat3(scene.ubo->normal) * input.normal);
      output.color  = input.color;

      const matrix<f32, 4, 4> model_view = scene.ubo->view * scene.ubo->model;
      const vector<f32, 4>    position   = model_view * input.position;
      output.clip                        = scene.ubo->projection * position;

      output.eye_position        = (model_view * position).xyz;
      const vector<f32, 3> lp    = scene.ubo->light_position;
      const vector<f32, 4> light = vector<f32, 4>{ lp.x, lp.y, lp.z, 1.0f } * model_view;
      output.light_vector        = normalize(light.xyz - output.eye_position);
      return output;
}
