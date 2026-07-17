#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    light_position;
      f32               outline_width;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(2)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4> clip;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 3> extruded = input.position.xyz + input.normal * scene.ubo->outline_width;
      const vector<f32, 4> position = vector<f32, 4>{ extruded.x, extruded.y, extruded.z, input.position.w };
      output.clip                   = scene.ubo->projection * scene.ubo->model * position;
      return output;
}
