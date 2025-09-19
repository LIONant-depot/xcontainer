include(${CMAKE_BINARY_DIR}/_deps/xcmake_tools/Common.cmake)

DefineInterfaceComponent(xcontainer "dependencies/xcore"
  "source/xcontainer.h"
  "source/xcontainer_basics.h"
  "source/xcontainer_fixedsize_memory_pool.h"
  "source/xcontainer_function.h"
  "source/xcontainer_lock.h"
  "source/xcontainer_lockless_pool.h"
  "source/xcontainer_lockless_queue.h"
  "source/xcontainer_small_vector.h"
  "source/xcontainer_unique_ptr.h"
  "source/xcontainer_unordered_lockless_map.h"
  "README.md"
)

