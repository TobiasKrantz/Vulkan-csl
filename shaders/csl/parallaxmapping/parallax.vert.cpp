#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> view;
      matrix<f32, 4, 4> model;
      vector<f32, 4>    light_position;
      vector<f32, 4>    camera_position;
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
      [[csl::location(3)]] vector<f32, 4> tangent;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> tangent_light_position;
      [[csl::location(2)]] vector<f32, 3> tangent_view_position;
      [[csl::location(3)]] vector<f32, 3> tangent_frag_position;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 4> local = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.clip                = scene.ubo->projection * scene.ubo->view * scene.ubo->model * local;

      const vector<f32, 3> world_frag_position = (scene.ubo->model * local).xyz;
      output.uv                                = input.uv;

      const vector<f32, 3> n = normalize(mat3(scene.ubo->model) * input.normal);
      const vector<f32, 3> t = normalize(mat3(scene.ubo->model) * input.tangent.xyz);
      const vector<f32, 3> b = normalize(cross(n, t));

      const matrix<f32, 3, 3> tbn = transpose(mat3(t, b, n));

      output.tangent_light_position = tbn * scene.ubo->light_position.xyz;
      output.tangent_view_position  = tbn * scene.ubo->camera_position.xyz;
      output.tangent_frag_position  = tbn * world_frag_position;
      return output;
}
