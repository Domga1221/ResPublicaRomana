#include "LayerStack.hpp"

#include "Memory/List.hpp"

struct LayerStack {
    List<Layer*> layers;
};

static LayerStack layerStack;

void LayerStack_Initialize() {
    List_Create(&layerStack.layers);
}

void LayerStack_Shutdown() {
    RPR_DEBUG("LayerStack_Shutdown");
    
    for(int i = 0; i < layerStack.layers.size; i++) {
        MEMORY_Free(layerStack.layers.data[i], sizeof(Layer), MEMORY_TAG_LAYER);
    }
    List_Destroy(&layerStack.layers);
}

void LayerStack_PushLayer(Layer* layer) {
    List_PushBack(&layerStack.layers, layer);
    layer->OnAttach();
}
   
void LayerStack_PopLayer(Layer* layer) {
    // TODO:
    //std::vector<Layer*>::iterator iterator = std::find(layerStack.layers.Begin(), layerStack.layers.End(), layer);
    //layerStack.layers.Erase(iterator);
    //layer->OnDetach();
}

void LayerStack_Update(f32 deltaTime) {
    for(int i = 0; i < layerStack.layers.size; i++) {
        layerStack.layers.data[i]->OnUpdate(deltaTime);
    }
}

void LayerStack_ImGuiRender(ImGuiContext* context) {
    for(int i = 0; i < layerStack.layers.size; i++) {
        layerStack.layers.data[i]->OnImGuiRender(context);
    }
}