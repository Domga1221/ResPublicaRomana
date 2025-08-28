#pragma once 

#include "defines.hpp"

void PostProcessingPanel_Initialize(); 
void PostProcessingPanel_Shutdown();
void PostProcessingPanel_OnImGuiRender();

b8 PostProcessingPanel_GetBloom();
b8 PostProcessingPanel_GetSSAO();
b8 PostProcessingPanel_GetColorCorrect();
