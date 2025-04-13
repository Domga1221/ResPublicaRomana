#pragma once 

#include "Memory.hpp"

#include <vector>
#include <iterator>


template<typename T>
class Vector {
    public:
        // Create
        Vector() { 
            //MEMORY_IncreaseMemoryUsage(sizeof(Vector))
            myVec = std::vector<T>(0);
        }

        Vector(int size) {
            myVec = std::vector<T>(size);
            MEMORY_IncreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
        }

        void Resize(int size) {
            MEMORY_DecreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
            myVec.resize(size);
            MEMORY_IncreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
        }

        void PushBack(T toPush) {
            MEMORY_DecreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
            myVec.push_back(toPush);
            MEMORY_IncreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
        }

        // Destroy
        ~Vector() {
            MEMORY_DecreaseMemoryUsage(myVec.capacity() * sizeof(T), MEMORY_TAG_VECTOR);
        }

        // Clear
        void Clear() {
            myVec.clear();
        }

        void ShrinkToFit() {
            myVec.shrink_to_fit();
        }

        // Getters
        i32 Size() {
            return myVec.size();
        }

        i32 Capacity() {
            return myVec.capacity();
        }

        T* Data() {
            return myVec.data();
        }

        // Operators
        T operator[] (i32 i) const {
            return myVec[i];
        }

        T& operator[] (i32 i) {
            return myVec[i];
        }

        typename std::vector<T>::iterator Begin() {
            return myVec.begin();
        }

        typename std::vector<T>::iterator End() {
            return myVec.end();
        }

        void Erase(typename std::vector<T>::iterator iterator) {
            myVec.erase(iterator);
        }

    private:
        std::vector<T> myVec;
};
