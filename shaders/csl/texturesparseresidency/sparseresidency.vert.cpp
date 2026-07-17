#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    view_position;
      f32               lod_bias;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] f32            lod_bias;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv       = input.uv;
      output.lod_bias = scene.ubo->lod_bias;
      output.clip     = scene.ubo->projection * scene.ubo->model * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      return output;
}
