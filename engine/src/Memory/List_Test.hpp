#pragma once 

#include "Memory/List.hpp"

#include <iostream>
#include <string>
void List_Test() {
    List<int> list;
    List_Create(&list);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    std::string memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    std::cout << "---\n";
    List_PushBack(&list, 1);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    std::cout << "---\n";
    List_PushBack(&list, 2);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    
    std::cout << "---\n";
    List_PushBack(&list, 3);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    std::cout << "---\n";
    List_PushBack(&list, 4);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);

    std::cout << "---\n";
    List_PushBack(&list, 5);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }




    std::cout << "---\nRemoving test: \n";
    List_RemoveAt(&list, 0);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    
    std::cout << "---\n";
    List_RemoveAt(&list, 1);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 4);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 3);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 2);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 1);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 0);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }

    memoryUsageString = MEMORY_GetMemoryUsageString();
    RPR_DEBUG("%s", memoryUsageString);
    std::cout << "---\n";
    List_RemoveAt(&list, 0);
    std::cout << "data: " << list.data << " size: " << list.size << " capacity: " << list.capacity << " stride: " << list.stride << "\n";
    for (int i = 0; i < list.size; i++) {
        std::cout << list.data[i] << "\n";
    }
    std::cout << "list.capacity: sizeof(int): " << list.capacity * sizeof(int) << "\n";

    List_Destroy(&list);

}