#include "SceneHierarchyPanel.hpp"

#include <Scene/Scene.hpp>
#include <Scene/GameObject.hpp>
#include <Scene/Components.hpp>

#include <imgui-docking/imgui.h>

#include "InspectorPanel.hpp" // TODO: Use event system 


static Scene* activeScene = 0;
static GameObject* selection;

static ImGuiTreeNodeFlags base_flags;

void SceneHierarchyPanel_Initialize(Scene* scene) {
    activeScene = scene;
    base_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;
    //base_flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;
    base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    base_flags |= ImGuiTreeNodeFlags_SpanFullWidth;
}

// TODO: Draw Node Hierarchy
/*
void drawTree(GameObject* gameObject) {
    TagComponent& tc = gameObject->GetComponent<TagComponent>();
    ImGuiTreeNodeFlags treeNodeFlags = base_flags;
    if(gameObject == selection)
        treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
    if(ImGui::TreeNodeEx((const void*)gameObject->handle, treeNodeFlags, "%s", tc.c_str())) {
        //RPR_INFO("size %d: \n", gameObject->children.size);
        for(u32 i = 0; i < gameObject->children.size; i++) {
            drawTree(gameObject->children.data[i]);
        }
        ImGui::TreePop();
    }
    if(ImGui::IsItemClicked() && activeScene->root != gameObject)  {
        selection = gameObject;
        InspectorPanel_SetSelectedGameObject(selection);
    }
}
*/

void drawTreeNode(GameObject* gameObject) { // TODO: Think about using just handles + scene in POD, or try own solution for hierarchy
    TagComponent& tc = gameObject->GetComponent<TagComponent>();

    ImGuiTreeNodeFlags treeNodeFlags = base_flags;
    if(gameObject == selection)
        treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
    if(ImGui::TreeNodeEx((const void*)gameObject->handle, treeNodeFlags, "%s", tc.c_str())) {
        ImGui::TreePop();
    }
    if(ImGui::IsItemClicked() && activeScene->root != gameObject)  {
        selection = gameObject;
        RPR_CLIENT_INFO("Selecting GameObject %u", gameObject->handle);
        InspectorPanel_SetSelectedGameObject(selection);
    }

    bool deleted = false; // otherwise ImGui will crash
    if(ImGui::BeginPopupContextItem()) {
        if(ImGui::MenuItem("Delete Entity")) 
            deleted = true;
        ImGui::EndPopup();
    }

    if(deleted) {
        RPR_CLIENT_INFO("\n---");
        RPR_CLIENT_INFO("Scene before deletion: ");
        for(u32 i = 0; i < activeScene->root->children.size; i++) {
            RPR_CLIENT_INFO("GameObject %u", activeScene->root->children.data[i]->handle);
        }

        RPR_CLIENT_INFO("Deleting GameObject %u", gameObject->handle);
        List_Remove(&activeScene->root->children, gameObject); // TODO: not efficient
        selection = nullptr;
        InspectorPanel_SetSelectedGameObject(nullptr);
        GameObject_Destroy(activeScene, gameObject);

        RPR_CLIENT_INFO("Scene root now only contains: ");
        for(u32 i = 0; i < activeScene->root->children.size; i++) {
            RPR_CLIENT_INFO("GameObject %u", activeScene->root->children.data[i]->handle);
        }
    }
}

void SceneHierarchyPanel_OnImGuiRender() {
    ImGui::Begin("Scene Hierarchy");

    //RPR_INFO("--- SceneHierarchyPanel_OnImGuiRender --- ");
    if(activeScene->root == nullptr) {
        RPR_ERROR("SceneHierarchyPanel_OnImGuiRender root node is null");
        return;
    }
    //ImGui::Text("Root");
    

    /*
    ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;
    base_flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;
    base_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    base_flags |= ImGuiTreeNodeFlags_SpanFullWidth;
    if(ImGui::TreeNodeEx("Root", base_flags)) {
        for(u32 i = 0; i < activeScene->root->children.size; i++) {
            GameObject* current = activeScene->root->children.data[i];
            TagComponent& tc = current->GetComponent<TagComponent>();
            //RPR_INFO("%s", tc.c_str());
            ImGui::TreeNodeEx((const void*)current->handle, base_flags, "%s", tc.c_str());
            if(ImGui::IsItemClicked())
                selection = current;

            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    */
    //if(ImGui::TreeNodeEx("Root")) {
    //        drawTree(activeScene->root);
    //    ImGui::TreePop();
    //}
    //drawTree(activeScene->root);

    for(u32 i = 0; i < activeScene->root->children.size; i++) {
        drawTreeNode(activeScene->root->children.data[i]);
    }


    if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
        selection = nullptr;
        InspectorPanel_SetSelectedGameObject(nullptr);
    }

        
        
    ImGui::End();
}

