#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> normal;
      vector<f32, 4>    light_pos;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> pos;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 4> tangent;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> light_vector;
      [[csl::location(2)]] vector<f32, 3> light_vector_b;
      [[csl::location(3)]] vector<f32, 3> light_dir;
      [[csl::location(4)]] vector<f32, 3> view_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      const vector<f32, 4> world           = scene.ubo->model * vector<f32, 4>{ input.pos.x, input.pos.y, input.pos.z, 1.0f };
      const vector<f32, 3> vertex_position = world.xyz;
      output.light_dir                     = normalize(scene.ubo->light_pos.xyz - vertex_position);

      const vector<f32, 3> bitangent = cross(input.normal, input.tangent.xyz);

      const matrix<f32, 3, 3> normal_matrix = mat3(scene.ubo->normal);
      const vector<f32, 3>    column_0       = normal_matrix * input.tangent.xyz;
      const vector<f32, 3>    column_1       = normal_matrix * bitangent;
      const vector<f32, 3>    column_2       = normal_matrix * input.normal;
      const matrix<f32, 3, 3> tbn            = mat3(column_0, column_1, column_2);

      output.light_vector = (scene.ubo->light_pos.xyz - vertex_position) * tbn;

      const vector<f32, 3> light_dist = scene.ubo->light_pos.xyz - input.pos;
      output.light_vector_b           = vector<f32, 3>{ dot(input.tangent.xyz, light_dist), dot(bitangent, light_dist), dot(input.normal, light_dist) };

      output.view_vector = vector<f32, 3>{ dot(input.tangent.xyz, input.pos), dot(bitangent, input.pos), dot(input.normal, input.pos) };

      output.uv   = input.uv;
      output.clip = scene.ubo->projection * scene.ubo->model * vector<f32, 4>{ input.pos.x, input.pos.y, input.pos.z, 1.0f };
      return output;
}
