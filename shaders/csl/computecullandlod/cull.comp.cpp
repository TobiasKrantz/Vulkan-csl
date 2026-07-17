#include <csl/csl.h>

using namespace csl;

struct InstanceData
{
      vector<f32, 3> pos;
      f32            scale;
};

struct IndexedIndirectCommand
{
      u32 index_count;
      u32 instance_count;
      u32 first_index;
      i32 vertex_offset;
      u32 first_instance;
};

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    camera_pos;
      vector<f32, 4>    frustum_planes[6];
};

struct UboOut
{
      u32 draw_count;
      u32 lod_count[6];
};

struct Lod
{
      u32 first_index;
      u32 index_count;
      f32 distance;
      f32 pad0;
};

struct Resources
{
      [[csl::binding(0, 0)]] const InstanceData*     instances;
      [[csl::binding(0, 1)]] IndexedIndirectCommand* indirect_draws;
      [[csl::uniform_buffer(0, 2)]] const Ubo*       ubo;
      [[csl::binding(0, 3)]] UboOut*                 ubo_out;
      [[csl::binding(0, 4)]] const Lod*              lods;
};

[[csl::compute(16, 1, 1)]] void compute_main()
{
      [[csl::constant(0)]] const i32 max_lod_level = 5;

      const Resources res = resources<Resources>();

      const vector<u32, 3> id  = dispatch_id();
      const u32            idx = id.x + id.y * workgroup_count().x * 16u;

      const vector<f32, 3> instance_pos = res.instances[idx].pos;
      const vector<f32, 4> pos          = vector<f32, 4>{ instance_pos.x, instance_pos.y, instance_pos.z, 1.0f };

      u32 visible = 1u;
      for (i32 i = 0; i < 6; i++)
      {
            if (dot(pos, res.ubo->frustum_planes[i]) + 1.0f < 0.0f)
                  visible = 0u;
      }

      if (visible != 0u)
      {
            res.indirect_draws[idx].instance_count = 1u;

            atomic_add(&res.ubo_out[0].draw_count, Scope::Device, MemorySemantics::None, 1u);

            u32 lod_level = u32(max_lod_level);
            for (u32 i = 0u; i < u32(max_lod_level); i++)
            {
                  if (distance(res.instances[idx].pos, res.ubo->camera_pos.xyz) < res.lods[i].distance)
                  {
                        lod_level = i;
                        break;
                  }
            }
            res.indirect_draws[idx].first_index = res.lods[lod_level].first_index;
            res.indirect_draws[idx].index_count = res.lods[lod_level].index_count;

            atomic_add(&res.ubo_out[0].lod_count[lod_level], Scope::Device, MemorySemantics::None, 1u);
      }
      else
      {
            res.indirect_draws[idx].instance_count = 0u;
      }
}
