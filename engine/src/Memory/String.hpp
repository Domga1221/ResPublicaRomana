#pragma once 

#include "defines.hpp"

typedef struct String {
    u32 length;
    char* sequence;
} String;

RPR_API void String_Create(String* s, const char* sequence);
RPR_API void String_Destroy(String* s);

RPR_API void String_Append(String* s, const char* toAppend);

