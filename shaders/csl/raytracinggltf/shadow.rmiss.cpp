#include <csl/csl.h>

using namespace csl;

[[csl::ray_payload_in(2)]] bool shadowed;

[[csl::miss]] void shadow_miss_main()
{
      shadowed = false;
}
