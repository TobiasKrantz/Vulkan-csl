#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection[2];
      matrix<f32, 4, 4> modelview[2];
      vector<f32, 4>    light_position;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
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
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const u32 view = view_index();

      output.color  = input.color;
      output.normal = mat3(scene.ubo->modelview[view]) * input.normal;

      const vector<f32, 4> local     = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 4> world     = scene.ubo->modelview[view] * local;
      const vector<f32, 3> light     = (scene.ubo->modelview[view] * scene.ubo->light_position).xyz;

      output.light_vector = light - world.xyz;
      output.view_vector  = -world.xyz;
      output.clip         = scene.ubo->projection[view] * world;
      return output;
}
