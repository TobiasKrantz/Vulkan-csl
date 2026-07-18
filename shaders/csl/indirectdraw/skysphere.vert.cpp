#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
      [[csl::location(2)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv = input.uv;

      // Skysphere stays centered: apply only the rotation part of modelview.
      const vector<f32, 3> rotated = mat3(scene.ubo->modelview) * input.position.xyz;
      output.clip                  = scene.ubo->projection * vector<f32, 4>{ rotated.x, rotated.y, rotated.z, 1.0f };
      return output;
}
