#include <csl/csl.h>

using namespace csl;

struct UboView
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
};

struct UboInstance
{
      matrix<f32, 4, 4> model;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const UboView*     view;
      [[csl::uniform_buffer(0, 1)]] const UboInstance* instance;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> color;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = input.color;

      const matrix<f32, 4, 4> model_view = scene.view->view * scene.instance->model;
      const vector<f32, 4>    local      = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                        = scene.view->projection * model_view * local;
      return output;
}
