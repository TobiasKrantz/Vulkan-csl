#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_pos;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> pos;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv     = input.uv;
      output.normal = input.normal;

      const vector<f32, 4> eye_pos = scene.ubo->modelview * vector<f32, 4>{ input.pos.x, input.pos.y, input.pos.z, 1.0f };
      output.clip                  = scene.ubo->projection * eye_pos;

      const vector<f32, 4> pos          = vector<f32, 4>{ input.pos.x, input.pos.y, input.pos.z, 1.0f };
      const vector<f32, 3> light_pos    = scene.ubo->light_pos.xyz;
      output.light_vector               = light_pos - pos.xyz;
      output.view_vector                = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - pos.xyz;
      return output;
}
