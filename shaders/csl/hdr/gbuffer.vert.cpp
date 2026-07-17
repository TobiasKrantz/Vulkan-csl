#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      matrix<f32, 4, 4> inverse_modelview;
      f32               exposure;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> uvw;
      [[csl::location(1)]] vector<f32, 3> position;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      [[csl::constant(0)]] const i32 type = 0;

      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uvw                 = input.position;
      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };

      switch (type)
      {
      case 0:
            output.position = mat3(scene.ubo->modelview) * input.position;
            output.clip     = scene.ubo->projection * vector<f32, 4>{ output.position.x, output.position.y, output.position.z, 1.0f };
            break;
      case 1:
            output.position = (scene.ubo->modelview * local).xyz;
            output.clip     = scene.ubo->projection * scene.ubo->modelview * local;
            break;
      }

      output.position = (scene.ubo->modelview * local).xyz;
      output.normal   = mat3(scene.ubo->modelview) * input.normal;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, -5.0f, 5.0f };
      output.light_vector                 = light_position - output.position;
      output.view_vector                  = -output.position;
      return output;
}
