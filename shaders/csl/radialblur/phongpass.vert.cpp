#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      f32               gradient_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.normal = input.normal;
      output.color  = input.color;
      output.uv     = vector<f32, 2>{ scene.ubo->gradient_position, 0.0f };
      output.clip   = scene.ubo->projection * scene.ubo->model * input.position;

      const vector<f32, 4> eye = scene.ubo->model * input.position;
      output.eye_position      = eye.xyz;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, 0.0f, -5.0f };
      output.light_vector                 = normalize(light_position - input.position.xyz);
      return output;
}
