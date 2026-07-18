#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      matrix<f32, 4, 4> view;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct MeshVertex
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::mesh(1, 1, 1, 3, 1)]] void mesh_main()
{
      const Resources resources_ = resources<Resources>();

      const vector<f32, 4> offset = vector<f32, 4>{ 0.0f, 0.0f, f32(dispatch_id().x), 0.0f };

      set_mesh_outputs(3, 1);
      const matrix<f32, 4, 4> mvp = resources_.ubo->projection * resources_.ubo->view * resources_.ubo->model;

      MeshVertex first;
      first.position = mvp * (vector<f32, 4>{ 0.0f, -1.0f, 0.0f, 1.0f } + offset);
      first.color    = vector<f32, 4>{ 0.0f, 1.0f, 0.0f, 1.0f };
      set_mesh_vertex(0, first);

      MeshVertex second;
      second.position = mvp * (vector<f32, 4>{ -1.0f, 1.0f, 0.0f, 1.0f } + offset);
      second.color    = vector<f32, 4>{ 0.0f, 0.0f, 1.0f, 1.0f };
      set_mesh_vertex(1, second);

      MeshVertex third;
      third.position = mvp * (vector<f32, 4>{ 1.0f, 1.0f, 0.0f, 1.0f } + offset);
      third.color    = vector<f32, 4>{ 1.0f, 0.0f, 0.0f, 1.0f };
      set_mesh_vertex(2, third);

      set_mesh_indices(workgroup_index(), vector<u32, 3>{ 0u, 1u, 2u });
}
