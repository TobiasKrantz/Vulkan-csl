#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    view_position;
      f32               lod_bias;
      i32               sampler_index;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] f32            lod_bias;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv       = input.uv * vector<f32, 2>{ 2.0f, 1.0f };
      output.lod_bias = scene.ubo->lod_bias;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      const vector<f32, 3> world = (scene.ubo->model * local).xyz;

      output.clip = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local;

      output.normal = mat3(inverse(transpose(scene.ubo->model))) * input.normal;

      const vector<f32, 3> light_position = vector<f32, 3>{ -30.0f, 0.0f, 0.0f };
      output.light_vector                 = world - light_position;
      output.view_vector                  = scene.ubo->view_position.xyz - world;
      return output;
}
