#include "PostProcessingPanel.hpp"

static bool bloom = false;
static bool ssao = false;
static bool colorCorrect = false;

#include <imgui-docking/imgui.h>

void PostProcessingPanel_Initialize() {

}

void PostProcessingPanel_OnImGuiRender() {
    ImGui::Begin("Post Processing");

    ImGui::Checkbox("Bloom", &bloom);
    ImGui::Checkbox("SSAO", &ssao);
    ImGui::Checkbox("Color Correct", &colorCorrect);

    ImGui::End();
}

b8 PostProcessingPanel_GetBloom() {
    return bloom;
}

b8 PostProcessingPanel_GetSSAO() {
    return ssao;
}

b8 PostProcessingPanel_GetColorCorrect() {
    return colorCorrect;
}