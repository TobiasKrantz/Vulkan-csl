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
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 3> color;
      [[csl::location(3)]] vector<f32, 3> instance_position;
      [[csl::location(4)]] f32            instance_scale;
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

      output.color  = input.color;
      output.normal = input.normal;

      const vector<f32, 3> world = input.position.xyz * input.instance_scale + input.instance_position;
      const vector<f32, 4> pos   = vector<f32, 4>{ world.x, world.y, world.z, 1.0f };

      output.clip = scene.ubo->projection * scene.ubo->modelview * pos;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, 10.0f, 50.0f };
      output.light_vector                 = light_position - pos.xyz;
      output.view_vector                  = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - pos.xyz;
      return output;
}
