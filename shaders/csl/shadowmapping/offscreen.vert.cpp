#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> depth_model_view_projection;
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
      [[csl::position]] vector<f32, 4> clip;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.clip = scene.ubo->depth_model_view_projection * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      return output;
}
