#include "print.hpp"

#include "defines.hpp"

#include "Core/Log.hpp"

void print(i32 a) {
    RPR_CRITICAL("test %d", 5);
    RPR_ERROR("test %s", "hello");
    RPR_WARN("test %f", 0.25f);
    RPR_INFO("test");
    RPR_DEBUG("test");
    RPR_TRACE("test");
}
