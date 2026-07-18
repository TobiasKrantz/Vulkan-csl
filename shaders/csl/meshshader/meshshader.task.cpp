#include <csl/csl.h>

using namespace csl;

[[csl::task(1, 1, 1)]] void task_main()
{
      emit_mesh_tasks(3, 1, 1);
}
