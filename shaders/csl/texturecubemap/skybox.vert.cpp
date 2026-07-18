#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> uvw;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uvw    = input.position;
      output.uvw.xy = output.uvw.xy * -1.0f;

      const vector<f32, 3> view_position = mat3(scene.ubo->model) * input.position;
      const vector<f32, 4> local         = vector<f32, 4>{ view_position.x, view_position.y, view_position.z, 1.0f };
      output.clip                        = scene.ubo->projection * local;
      return output;
}
