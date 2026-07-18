#include <csl/csl.h>

using namespace csl;

struct PnPatch
{
      f32 b210;
      f32 b120;
      f32 b021;
      f32 b012;
      f32 b102;
      f32 b201;
      f32 b111;
      f32 n110;
      f32 n011;
      f32 n101;
};

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      f32               tess_alpha;
      f32               tess_level;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct ControlInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct ControlOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 2> uv;
      [[csl::location(6)]] PnPatch        patch;
};

static f32 wij(vector<f32, 3> pi, vector<f32, 3> pj, vector<f32, 3> ni)
{
      return dot(pj - pi, ni);
}

static f32 vij(vector<f32, 3> pi, vector<f32, 3> pj, vector<f32, 3> ni, vector<f32, 3> nj)
{
      const vector<f32, 3> difference = pj - pi;
      const vector<f32, 3> normal_sum = ni + nj;
      return 2.0f * dot(difference, normal_sum) / dot(difference, difference);
}

[[csl::tess_control(3)]] void tess_control_main()
{
      const Resources resources_ = resources<Resources>();
      const u32       id         = u32(invocation_id());

      const ControlInput v0   = stage_input<ControlInput>(0u);
      const ControlInput v1   = stage_input<ControlInput>(1u);
      const ControlInput v2   = stage_input<ControlInput>(2u);
      const ControlInput self = stage_input<ControlInput>(id);

      const f32 p0 = v0.position[id];
      const f32 p1 = v1.position[id];
      const f32 p2 = v2.position[id];
      const f32 n0 = v0.normal[id];
      const f32 n1 = v1.normal[id];
      const f32 n2 = v2.normal[id];

      PnPatch patch;
      patch.b210 = (2.0f * p0 + p1 - wij(v0.position.xyz, v1.position.xyz, v0.normal) * n0) / 3.0f;
      patch.b120 = (2.0f * p1 + p0 - wij(v1.position.xyz, v0.position.xyz, v1.normal) * n1) / 3.0f;
      patch.b021 = (2.0f * p1 + p2 - wij(v1.position.xyz, v2.position.xyz, v1.normal) * n1) / 3.0f;
      patch.b012 = (2.0f * p2 + p1 - wij(v2.position.xyz, v1.position.xyz, v2.normal) * n2) / 3.0f;
      patch.b102 = (2.0f * p2 + p0 - wij(v2.position.xyz, v0.position.xyz, v2.normal) * n2) / 3.0f;
      patch.b201 = (2.0f * p0 + p2 - wij(v0.position.xyz, v2.position.xyz, v0.normal) * n0) / 3.0f;

      const f32 e = (patch.b210 + patch.b120 + patch.b021 + patch.b012 + patch.b102 + patch.b201) / 6.0f;
      const f32 v = (p0 + p1 + p2) / 3.0f;
      patch.b111 = e + (e - v) * 0.5f;
      patch.n110 = n0 + n1 - vij(v0.position.xyz, v1.position.xyz, v0.normal, v1.normal) * (p1 - p0);
      patch.n011 = n1 + n2 - vij(v1.position.xyz, v2.position.xyz, v1.normal, v2.normal) * (p2 - p1);
      patch.n101 = n2 + n0 - vij(v2.position.xyz, v0.position.xyz, v2.normal, v0.normal) * (p0 - p2);

      ControlOutput output;
      output.position = self.position;
      output.normal   = self.normal;
      output.uv       = self.uv;
      output.patch    = patch;
      set_tess_vertex(id, output);

      set_tess_level_outer(id, resources_.ubo->tess_level);
      set_tess_level_inner(0, resources_.ubo->tess_level);
}
