#pragma once 

#include "defines.hpp"

struct Layer {
    void (*OnAttach)(void);
    void (*OnDetach)(void);
    void (*OnUpdate)(void);
    // TODO: OnEvent

    void (*OnImGuiRender)(void);
};