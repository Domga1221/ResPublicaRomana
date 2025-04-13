#include "LayerStack.hpp"

#include "Memory/Vector.hpp"

struct LayerStack {
    Vector<Layer*> layers;
};

static LayerStack layerStack;

void LayerStack_Initialize() {

}

void LayerStack_Shutdown() {
    for(int i = 0; i < layerStack.layers.Size(); i++) {
        MEMORY_Free(layerStack.layers[i], sizeof(Layer), MEMORY_TAG_LAYER);
    }
    layerStack.layers.Clear();
}

void LayerStack_PushLayer(Layer* layer) {
    layerStack.layers.PushBack(layer);
}
   
void LayerStack_PopLayer(Layer* layer) {
    std::vector<Layer*>::iterator iterator = std::find(layerStack.layers.Begin(), layerStack.layers.End(), layer);
    layerStack.layers.Erase(iterator);
}

void LayerStack_Update() {
    for(int i = 0; i < layerStack.layers.Size(); i++) {
        layerStack.layers[i]->OnUpdate();
    }
}