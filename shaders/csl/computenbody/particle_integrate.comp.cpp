#include <csl/csl.h>

using namespace csl;

struct Particle
{
      vector<f32, 4> pos;
      vector<f32, 4> vel;
};

struct Ubo
{
      f32 delta_t;
      i32 particle_count;
};

struct Resources
{
      [[csl::binding(0, 0)]] Particle*         particles;
      [[csl::uniform_buffer(0, 1)]] const Ubo* ubo;
};

[[csl::compute(256, 1, 1)]] void compute_main()
{
      const Resources resources_ = resources<Resources>();

      i32 index = i32(dispatch_id().x);

      vector<f32, 4> position = resources_.particles[index].pos;
      vector<f32, 4> velocity = resources_.particles[index].vel;

      position += resources_.ubo->delta_t * velocity;
      resources_.particles[index].pos = position;
}
