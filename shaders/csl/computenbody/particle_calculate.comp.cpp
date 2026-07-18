#include <csl/csl.h>

using namespace csl;

constexpr u32 SHARED_DATA_SIZE = 1024;
constexpr u32 WORKGROUP_SIZE   = 256;

struct Particle
{
      vector<f32, 4> pos;
      vector<f32, 4> vel;
};

struct Ubo
{
      f32 delta_t;
      i32 particle_count;
      f32 gravity;
      f32 power;
      f32 soften;
};

struct Resources
{
      [[csl::binding(0, 0)]] Particle*         particles;
      [[csl::uniform_buffer(0, 1)]] const Ubo* ubo;
};

[[csl::compute(256, 1, 1)]] void compute_main()
{
      const Resources resources_ = resources<Resources>();

      [[csl::shared]] vector<f32, 4> shared_data[SHARED_DATA_SIZE];

      u32 index = dispatch_id().x;
      u32 count = u32(resources_.ubo->particle_count);
      if (index >= count)
            return;

      u32            local_id     = workgroup_thread_id().x;
      vector<f32, 4> position     = resources_.particles[index].pos;
      vector<f32, 3> acceleration = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      f32 gravity = resources_.ubo->gravity;
      f32 soften  = resources_.ubo->soften;
      f32 power   = resources_.ubo->power;

      for (u32 i = 0; i < count; i += SHARED_DATA_SIZE)
      {
            u32 sample_index = i + local_id;
            if (sample_index < count)
                  shared_data[local_id] = resources_.particles[sample_index].pos;
            else
                  shared_data[local_id] = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };

            memory_barrier(Scope::Workgroup, MemorySemantics::AcquireRelease | MemorySemantics::WorkgroupMemory);
            workgroup_barrier();

            for (u32 j = 0; j < WORKGROUP_SIZE; j++)
            {
                  vector<f32, 4> other  = shared_data[j];
                  vector<f32, 3> offset = other.xyz - position.xyz;
                  acceleration += gravity * offset * other.w / pow(dot(offset, offset) + soften, power);
            }

            memory_barrier(Scope::Workgroup, MemorySemantics::AcquireRelease | MemorySemantics::WorkgroupMemory);
            workgroup_barrier();
      }

      vector<f32, 4> velocity  = resources_.particles[index].vel;
      vector<f32, 3> new_vel   = velocity.xyz + resources_.ubo->delta_t * acceleration;
      f32            gradient  = velocity.w + 0.1f * resources_.ubo->delta_t;
      if (gradient > 1.0f)
            gradient -= 1.0f;

      resources_.particles[index].vel = vector<f32, 4>{ new_vel.x, new_vel.y, new_vel.z, gradient };
}
