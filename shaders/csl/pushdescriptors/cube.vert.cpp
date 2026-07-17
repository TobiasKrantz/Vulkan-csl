#include <csl/csl.h>

using namespace csl;

struct Camera
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
};

struct Model
{
      matrix<f32, 4, 4> local;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Camera* camera;
      [[csl::uniform_buffer(0, 1)]] const Model*  model;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Resources resources_ = resources<Resources>();
      VertexInput     input      = stage_input<VertexInput>();
      VertexOutput    output;

      output.normal = input.normal;
      output.color  = input.color;
      output.uv     = input.uv;
      output.clip   = resources_.camera->projection * resources_.camera->view * resources_.model->local * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      return output;
}
