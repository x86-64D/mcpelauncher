#include "fake_egl.h"
#include <mcpelauncher/linker.h>
#include <vector>

class FakeSwappyGL {
public:
    static void initHooks(std::vector<mcpelauncher_hook_t> &hooks);
};
