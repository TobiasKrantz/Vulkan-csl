#include <csl/csl.h>

using namespace csl;

struct PushConstants
{
      matrix<f32, 4, 4> mvp;
      vector<f32, 3>    color;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      if (input.color.x == 1.0f && input.color.y == 0.0f && input.color.z == 0.0f)
            output.color = push.color;
      else
            output.color = input.color;

      const vector<f32, 4> local    = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                   = push.mvp * local;
      const vector<f32, 4> position = push.mvp * local;

      output.normal = mat3(push.mvp) * input.normal;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      output.light_vector                 = light_position - position.xyz;
      output.view_vector                  = -position.xyz;
      return output;
}
