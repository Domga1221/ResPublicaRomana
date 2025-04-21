#pragma once 

#include "defines.hpp"

struct ImGuiContext;
struct Layer {
    void (*OnAttach)(void);
    void (*OnDetach)(void);
    void (*OnUpdate)(f32);
    // TODO: OnEvent

    void (*OnImGuiRender)(ImGuiContext* context);
};