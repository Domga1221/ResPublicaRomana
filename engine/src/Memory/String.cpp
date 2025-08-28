#include "String.hpp"

#include "Memory.hpp"

void String_Create(String* s, const char* sequence) {
    u32 seqLength = strlen(sequence);
    s->sequence = (char*)MEMORY_Allocate(sizeof(char) * (seqLength + 1), MEMORY_TAG_STRING);
    s->length = seqLength;

    MEMORY_Copy(s->sequence, sequence, seqLength);
    s->sequence[seqLength] = '\0';
}

void String_Destroy(String* s) {
    MEMORY_Free(s->sequence, sizeof(char) * (s->length + 1), MEMORY_TAG_STRING);
    s->sequence = 0;
    s->length = 0;
}

void String_Append(String* s, const char* toAppend) { 
    u32 len = strlen(toAppend);
    char* newChar = (char*)MEMORY_Allocate(sizeof(char) * (s->length + len + 1), MEMORY_TAG_STRING);
    MEMORY_Copy(newChar, s->sequence, s->length);
    MEMORY_Copy((void*)(newChar + len), toAppend, len);
    newChar[s->length + len] = '\0';
    
    MEMORY_Free(s->sequence, sizeof(char) * s->length + 1, MEMORY_TAG_STRING);
    
    s->sequence = newChar;
    s->length = s->length + len;
}
