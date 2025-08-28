#pragma once 

#include "defines.hpp"

#include <string> // TODO: String 

void Filesystem_Initialize(); 
void Filesystem_Shutdown();


RPR_API std::string Filesystem_OpenFile(const char* filter); 
RPR_API std::string Filesystem_SaveFile(const char* filter);

RPR_API const char* Filesystem_GetCWD();