#include <csl/csl.h>

using namespace csl;

struct MatrixReference
{
      matrix<f32, 4, 4> matrix;
};

struct PushConstants
{
      const MatrixReference* scene_data;
      const MatrixReference* model_data;
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

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.normal = input.normal;
      output.color  = input.color;
      output.uv     = input.uv;
      output.clip   = push.scene_data->matrix * push.model_data->matrix * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      return output;
}
