#include <csl/csl.h>

using namespace csl;

struct Particle
{
      vector<f32, 2> pos;
      vector<f32, 2> vel;
      vector<f32, 4> gradient_pos;
};

struct Ubo
{
      f32 delta_t;
      f32 dest_x;
      f32 dest_y;
      i32 particle_count;
};

struct Resources
{
      [[csl::binding(0, 0)]] const Particle* particles_in;
      [[csl::binding(0, 1)]] Particle*       particles_out;
      [[csl::uniform_buffer(0, 2)]] const Ubo* ubo;
};

static vector<f32, 2> attraction(vector<f32, 2> pos, vector<f32, 2> attract_pos)
{
      vector<f32, 2> delta           = attract_pos - pos;
      const f32      damp            = 0.5f;
      f32            damped_dot      = dot(delta, delta) + damp;
      f32            inverse_dist    = 1.0f / sqrt(damped_dot);
      f32            inverse_cubed   = inverse_dist * inverse_dist * inverse_dist;
      return delta * inverse_cubed * 0.0035f;
}

static vector<f32, 2> repulsion(vector<f32, 2> pos, vector<f32, 2> attract_pos)
{
      vector<f32, 2> delta           = attract_pos - pos;
      f32            target_distance = sqrt(dot(delta, delta));
      return delta * (1.0f / (target_distance * target_distance * target_distance)) * -0.000035f;
}

[[csl::compute(256, 1, 1)]] void compute_main()
{
      const Resources resources_ = resources<Resources>();

      u32 index = dispatch_id().x;
      if (index >= u32(resources_.ubo->particle_count))
            return;

      vector<f32, 2> velocity = resources_.particles_in[index].vel;
      vector<f32, 2> position = resources_.particles_in[index].pos;
      vector<f32, 4> gradient = resources_.particles_in[index].gradient_pos;

      vector<f32, 2> dest = vector<f32, 2>{ resources_.ubo->dest_x, resources_.ubo->dest_y };

      velocity += repulsion(position, dest) * 0.05f;

      position += velocity * resources_.ubo->delta_t;

      if ((position.x < -1.0f) || (position.x > 1.0f) || (position.y < -1.0f) || (position.y > 1.0f))
            velocity = (-velocity * 0.1f) + attraction(position, dest) * 12.0f;
      else
            resources_.particles_out[index].pos = position;

      resources_.particles_out[index].vel            = velocity;
      resources_.particles_out[index].gradient_pos.x = gradient.x + 0.02f * resources_.ubo->delta_t;
      if (resources_.particles_out[index].gradient_pos.x > 1.0f)
            resources_.particles_out[index].gradient_pos.x -= 1.0f;
}
