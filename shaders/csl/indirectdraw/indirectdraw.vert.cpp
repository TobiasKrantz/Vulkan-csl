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

      const f32               sx = sin(input.instance_rotation.x);
      const f32               cx = cos(input.instance_rotation.x);
      const matrix<f32, 4, 4> mx = mat4(vector<f32, 4>{ cx, sx, 0.0f, 0.0f }, vector<f32, 4>{ -sx, cx, 0.0f, 0.0f }, vector<f32, 4>{ 0.0f, 0.0f, 1.0f, 0.0f },
                                        vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });

      const f32               sy = sin(input.instance_rotation.y);
      const f32               cy = cos(input.instance_rotation.y);
      const matrix<f32, 4, 4> my = mat4(vector<f32, 4>{ cy, 0.0f, sy, 0.0f }, vector<f32, 4>{ 0.0f, 1.0f, 0.0f, 0.0f }, vector<f32, 4>{ -sy, 0.0f, cy, 0.0f },
                                        vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });

      const f32               sz = sin(input.instance_rotation.z);
      const f32               cz = cos(input.instance_rotation.z);
      const matrix<f32, 4, 4> mz = mat4(vector<f32, 4>{ 1.0f, 0.0f, 0.0f, 0.0f }, vector<f32, 4>{ 0.0f, cz, sz, 0.0f }, vector<f32, 4>{ 0.0f, -sz, cz, 0.0f },
                                        vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f });

      const matrix<f32, 4, 4> rotation = mz * my * mx;

      output.normal = input.normal * mat3(rotation);

      const vector<f32, 3> scaled   = input.position * input.instance_scale + input.instance_position;
      const vector<f32, 4> position = vector<f32, 4>{ scaled.x, scaled.y, scaled.z, 1.0f } * rotation;

      output.clip = scene.ubo->projection * scene.ubo->modelview * position;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, -5.0f, 0.0f };
      output.light_vector                 = light_position - position.xyz;
      output.view_vector                  = -position.xyz;
      return output;
}
