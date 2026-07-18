#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> normal;
      matrix<f32, 4, 4> view;
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

// Skybox uses the view rotation with translation removed: mat4(mat3(view)).
// mat4(mat3(V)) * q is algebraically vec4(mat3(V) * q.xyz, q.w).
[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uvw = input.position;

      const vector<f32, 4> local    = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 4> world    = scene.ubo->model * local;
      const vector<f32, 3> rotated  = mat3(scene.ubo->view) * world.xyz;
      output.clip                   = scene.ubo->projection * vector<f32, 4>{ rotated.x, rotated.y, rotated.z, world.w };
      return output;
}
