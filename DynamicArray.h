#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "Exceptions.h"

template <class T>
class DynamicArray {
private:
    T* data;
    int size;

public:
    T& operator[](int index) {
        if (index < 0 || index >= size) throw IndexOutOfRange();
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) throw IndexOutOfRange();
        return data[index];
    }

    DynamicArray(T* items, int count) {
        size = count;
        data = new T[size];
        for (int i = 0; i < size; i++) {
            data[i] = items[i];
        }
    }

    DynamicArray(int size) {
        this->size = size;
        data = new T[size];
        for (int i = 0; i < size; i++) {
            data[i] = T();
        }
    }

    DynamicArray(const DynamicArray<T>& other) {
        size = other.size;
        data = new T[size];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    ~DynamicArray() {
        delete[] data;
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new T[size];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    T Get(int index) const {
        if (index < 0 || index >= size) throw IndexOutOfRange();
        return data[index];
    }

    DynamicArray<T>& Set(int index, T value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        data[index] = value;
        return *this;
    }

    size_t GetSize() const {
        return size;
    }

    // Методы для прямого доступа к объектам
    // T* Data() { return data; }
    // const T* Data() const { return data; }

    size_t Resize(int newSize) {
        if (newSize < 0) throw IndexOutOfRange();
        if (newSize == size) return size;

        T* newData = new T[newSize];
        
        // Копируем старые данные
        int countToCopy = (newSize < size) ? newSize : size;
        for (int i = 0; i < countToCopy; i++) {
            newData[i] = data[i];
        }

        // Инициализация новых элементов нулем
        // Если newSize > size, то элементы от size до newSize-1 должны быть 0
        for (int i = size; i < newSize; i++) {
            newData[i] = T(); // T() для int равно 0, для double 0.0 и т.д.
        }

        delete[] data;
        data = newData;
        size = newSize;
        return size;
    }
};

#endif