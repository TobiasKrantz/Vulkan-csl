#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> inverse_model;
      f32               lod_bias;
      i32               cube_map_index;
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

      // View is the model matrix with translation removed: mat4(mat3(model)) * vec4(p, 1) == vec4(mat3(model) * p, 1).
      const vector<f32, 3> rotated       = mat3(scene.ubo->model) * input.position;
      const vector<f32, 4> view_position = vector<f32, 4>{ rotated.x, rotated.y, rotated.z, 1.0f };
      output.clip                        = scene.ubo->projection * view_position;
      return output;
}
