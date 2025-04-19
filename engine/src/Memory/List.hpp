#pragma once

#include "Memory.hpp"
#include "Core/Log.hpp"

template<typename T>
struct List {
	unsigned int size = 0;
	unsigned int capacity = 0;
	unsigned int stride = 0;
	T* data;
};

template<typename T>
void List_Create(List<T>* list) {
	list->size = 0;
	list->capacity = 0;
	list->stride = sizeof(T);
	list->data = 0;
}

template<typename T>
void List_Destroy(List<T>* list) {
	if (list->data != 0) {
		MEMORY_Free(list->data, list->capacity * list->stride, MEMORY_TAG_LIST);
	}
    MEMORY_Set(list, 0, sizeof(List<T>));
}

template<typename T>
void List_Resize(List<T>* list) {
	unsigned int capacity = list->capacity;
	unsigned int size = list->size;
	unsigned int stride = list->stride;
	T* data = list->data;

	if (capacity == 0) 
		capacity = 1;
    T* temp = (T*)MEMORY_Allocate(capacity * 2 * stride, MEMORY_TAG_LIST);
	if (data != 0) {
        MEMORY_Copy(temp, data, size * stride);
        MEMORY_Free(data, capacity * stride, MEMORY_TAG_LIST);
	}

	list->data = temp;
	list->capacity = capacity * 2;
}

template<typename T>
void List_PushBack(List<T>* list, T item) {
	unsigned int capacity = list->capacity;
	unsigned int size = list->size;
	//unsigned int stride = list->stride;

	if (size >= capacity) {
		List_Resize(list);
	}

	T* data = list->data;
	data[size] = item;
	list->size = size + 1;
}

template<typename T>
void List_RemoveAt(List<T>* list, unsigned int index) {
	unsigned int size = list->size;
	unsigned int stride = list->stride;
	T* data = list->data;
	if (index >= size) {
        RPR_WARN("List_RemoveAt: trying to removae at invalid position: %d, list->size: %d", index, size);
		return;
	}

	// copy, if element wasn't removed at the end
	if (index != size - 1) {
        MEMORY_Move((void*)(data + index), (const void*)(data + index + 1), (size - index) * stride);
	}
	list->size = size - 1;
}

// TODO: Remove by value
// TODO: Insert at
// TODO: Remove at end
// TODO: Copy one list to another 
// TODO: List struct as opaque struct or private members for list struct 
// TODO: Change memory tag