#include <csl/csl.h>

using namespace csl;

struct Particle
{
      vector<f32, 4> pos;
      vector<f32, 4> vel;
      vector<f32, 4> uv;
      vector<f32, 4> normal;
};

struct Ubo
{
      f32            delta_t;
      f32            particle_mass;
      f32            spring_stiffness;
      f32            damping;
      f32            rest_dist_h;
      f32            rest_dist_v;
      f32            rest_dist_d;
      f32            sphere_radius;
      vector<f32, 4> sphere_pos;
      vector<f32, 4> gravity;
      vector<i32, 2> particle_count;
};

struct Resources
{
      [[csl::binding(0, 0)]] const Particle*   particle_in;
      [[csl::binding(0, 1)]] Particle*         particle_out;
      [[csl::uniform_buffer(0, 2)]] const Ubo* params;
};

struct PushConstants
{
      u32 calculate_normals;
};

static vector<f32, 3> spring_force(vector<f32, 3> p0, vector<f32, 3> p1, f32 rest_dist, f32 stiffness)
{
      vector<f32, 3> dist = p0 - p1;
      return normalize(dist) * stiffness * (length(dist) - rest_dist);
}

[[csl::compute(10, 10, 1)]] void compute_main(PushConstants push)
{
      const Resources resources_ = resources<Resources>();

      vector<u32, 3> id = dispatch_id();

      u32 count_x = u32(resources_.params->particle_count.x);
      u32 count_y = u32(resources_.params->particle_count.y);

      u32 index = id.y * count_x + id.x;
      if (index > count_x * count_y)
            return;

      vector<f32, 3> force = resources_.params->gravity.xyz * resources_.params->particle_mass;

      vector<f32, 3> pos = resources_.particle_in[index].pos.xyz;
      vector<f32, 3> vel = resources_.particle_in[index].vel.xyz;

      f32 stiffness = resources_.params->spring_stiffness;
      f32 rest_h    = resources_.params->rest_dist_h;
      f32 rest_v    = resources_.params->rest_dist_v;
      f32 rest_d    = resources_.params->rest_dist_d;

      if (id.x > 0)
            force += spring_force(resources_.particle_in[index - 1].pos.xyz, pos, rest_h, stiffness);
      if (id.x < count_x - 1)
            force += spring_force(resources_.particle_in[index + 1].pos.xyz, pos, rest_h, stiffness);
      if (id.y < count_y - 1)
            force += spring_force(resources_.particle_in[index + count_x].pos.xyz, pos, rest_v, stiffness);
      if (id.y > 0)
            force += spring_force(resources_.particle_in[index - count_x].pos.xyz, pos, rest_v, stiffness);
      if ((id.x > 0) && (id.y < count_y - 1))
            force += spring_force(resources_.particle_in[index + count_x - 1].pos.xyz, pos, rest_d, stiffness);
      if ((id.x > 0) && (id.y > 0))
            force += spring_force(resources_.particle_in[index - count_x - 1].pos.xyz, pos, rest_d, stiffness);
      if ((id.x < count_x - 1) && (id.y < count_y - 1))
            force += spring_force(resources_.particle_in[index + count_x + 1].pos.xyz, pos, rest_d, stiffness);
      if ((id.x < count_x - 1) && (id.y > 0))
            force += spring_force(resources_.particle_in[index - count_x + 1].pos.xyz, pos, rest_d, stiffness);

      force += (-resources_.params->damping * vel);

      f32            delta_t = resources_.params->delta_t;
      vector<f32, 3> f       = force * (1.0f / resources_.params->particle_mass);

      vector<f32, 3> new_pos = pos + vel * delta_t + 0.5f * f * delta_t * delta_t;
      resources_.particle_out[index].pos = vector<f32, 4>{ new_pos.x, new_pos.y, new_pos.z, 1.0f };
      vector<f32, 3> new_vel = vel + f * delta_t;
      resources_.particle_out[index].vel = vector<f32, 4>{ new_vel.x, new_vel.y, new_vel.z, 0.0f };

      vector<f32, 3> sphere_dist = resources_.particle_out[index].pos.xyz - resources_.params->sphere_pos.xyz;
      if (length(sphere_dist) < resources_.params->sphere_radius + 0.01f)
      {
            resources_.particle_out[index].pos.xyz = resources_.params->sphere_pos.xyz + normalize(sphere_dist) * (resources_.params->sphere_radius + 0.01f);
            resources_.particle_out[index].vel     = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
      }

      if (push.calculate_normals == 1)
      {
            vector<f32, 3> normal = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
            vector<f32, 3> a;
            vector<f32, 3> b;
            vector<f32, 3> c;
            if (id.y > 0)
            {
                  if (id.x > 0)
                  {
                        a = resources_.particle_in[index - 1].pos.xyz - pos;
                        b = resources_.particle_in[index - count_x - 1].pos.xyz - pos;
                        c = resources_.particle_in[index - count_x].pos.xyz - pos;
                        normal += cross(a, b) + cross(b, c);
                  }
                  if (id.x < count_x - 1)
                  {
                        a = resources_.particle_in[index - count_x].pos.xyz - pos;
                        b = resources_.particle_in[index - count_x + 1].pos.xyz - pos;
                        c = resources_.particle_in[index + 1].pos.xyz - pos;
                        normal += cross(a, b) + cross(b, c);
                  }
            }
            if (id.y < count_y - 1)
            {
                  if (id.x > 0)
                  {
                        a = resources_.particle_in[index + count_x].pos.xyz - pos;
                        b = resources_.particle_in[index + count_x - 1].pos.xyz - pos;
                        c = resources_.particle_in[index - 1].pos.xyz - pos;
                        normal += cross(a, b) + cross(b, c);
                  }
                  if (id.x < count_x - 1)
                  {
                        a = resources_.particle_in[index + 1].pos.xyz - pos;
                        b = resources_.particle_in[index + count_x + 1].pos.xyz - pos;
                        c = resources_.particle_in[index + count_x].pos.xyz - pos;
                        normal += cross(a, b) + cross(b, c);
                  }
            }
            vector<f32, 3> unit = normalize(normal);
            resources_.particle_out[index].normal = vector<f32, 4>{ unit.x, unit.y, unit.z, 0.0f };
      }
}
