#include <csl/csl.h>

using namespace csl;

struct Instance
{
      matrix<f32, 4, 4> model;
      f32               array_index;
};

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      Instance          instance[8];
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const u32 index = instance_index();

      output.uv = vector<f32, 3>{ input.uv.x, input.uv.y, scene.ubo->instance[index].array_index };

      const matrix<f32, 4, 4> model_view = scene.ubo->view * scene.ubo->instance[index].model;
      const vector<f32, 4>    local      = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                        = scene.ubo->projection * model_view * local;
      return output;
}
