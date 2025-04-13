#pragma once 

#include "defines.hpp"

#include "Layer.hpp"

void LayerStack_Initialize();
void LayerStack_Shutdown();

void LayerStack_PushLayer(Layer* layer);
void LayerStack_PopLayer(Layer* layer);

void LayerStack_Update();
