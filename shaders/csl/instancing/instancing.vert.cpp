#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_position;
      f32               local_speed;
      f32               global_speed;
};

struct Scene
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
      [[csl::location(4)]] vector<f32, 3> instance_position;
      [[csl::location(5)]] vector<f32, 3> instance_rotation;
      [[csl::location(6)]] f32            instance_scale;
      [[csl::location(7)]] i32            instance_texture_index;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> uv;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.color = input.color;
      output.uv    = vector<f32, 3>{ input.uv.x, input.uv.y, f32(input.instance_texture_index) };

      f32 s = sin(input.instance_rotation.x + scene.ubo->local_speed);
      f32 c = cos(input.instance_rotation.x + scene.ubo->local_speed);
      const matrix<f32, 3, 3> mx = mat3(vector<f32, 3>{ c, s, 0.0f }, vector<f32, 3>{ -s, c, 0.0f }, vector<f32, 3>{ 0.0f, 0.0f, 1.0f });

      s = sin(input.instance_rotation.y + scene.ubo->local_speed);
      c = cos(input.instance_rotation.y + scene.ubo->local_speed);
      const matrix<f32, 3, 3> my = mat3(vector<f32, 3>{ c, 0.0f, s }, vector<f32, 3>{ 0.0f, 1.0f, 0.0f }, vector<f32, 3>{ -s, 0.0f, c });

      s = sin(input.instance_rotation.z + scene.ubo->local_speed);
      c = cos(input.instance_rotation.z + scene.ubo->local_speed);
      const matrix<f32, 3, 3> mz = mat3(vector<f32, 3>{ 1.0f, 0.0f, 0.0f }, vector<f32, 3>{ 0.0f, c, s }, vector<f32, 3>{ 0.0f, -s, c });

      const matrix<f32, 3, 3> rotation = mz * my * mx;

      s = sin(input.instance_rotation.y + scene.ubo->global_speed);
      c = cos(input.instance_rotation.y + scene.ubo->global_speed);
      const matrix<f32, 4, 4> global_rotation =
          mat4(vector<f32, 4>{ c, 0.0f, s, 0.0f }, vector<f32, 4>{ 0.0f, 1.0f, 0.0f, 0.0f }, vector<f32, 4>{ -s, 0.0f, c, 0.0f }, vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });

      const vector<f32, 3> rotated   = input.position * rotation;
      const vector<f32, 3> scaled    = rotated * input.instance_scale + input.instance_position;
      const vector<f32, 4> position  = vector<f32, 4>{ scaled.x, scaled.y, scaled.z, 1.0f };

      output.clip   = scene.ubo->projection * scene.ubo->modelview * global_rotation * position;
      output.normal = mat3(scene.ubo->modelview * global_rotation) * inverse(rotation) * input.normal;

      const vector<f32, 3> world_input = input.position + input.instance_position;
      const vector<f32, 4> view_pos    = scene.ubo->modelview * vector<f32, 4>{ world_input.x, world_input.y, world_input.z, 1.0f };
      const vector<f32, 3> light       = mat3(scene.ubo->modelview) * scene.ubo->light_position.xyz;
      output.light_vector              = light - view_pos.xyz;
      output.view_vector               = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - view_pos.xyz;
      return output;
}
