#ifndef CONVERTERS_H
#define CONVERTERS_H

#include "Sequence.h"
#include "ArraySequence.h"
#include "ListSequence.h"

/**
 * @brief Конвертирует любую последовательность в MutableArraySequence.
 * Создает новую последовательность и копирует в неё все элементы.
 */
template <class T>
Sequence<T>* ConvertToArraySequence(Sequence<T>* source) {
    if (source == nullptr) {
        return new MutableArraySequence<T>();
    }
    Sequence<T>* result = new MutableArraySequence<T>();

    int len = source->GetLength();
    for (int i = 0; i < len; ++i) {
        result = result->Append(source->Get(i));
    }

    return result;
}

/**
 * @brief Конвертирует любую последовательность в MutableListSequence.
 * Создает новую последовательность и копирует в неё все элементы.
 */
template <class T>
Sequence<T>* ConvertToListSequence(Sequence<T>* source) {
    if (source == nullptr) {
        return new MutableListSequence<T>();
    }
    Sequence<T>* result = new MutableListSequence<T>();

    int len = source->GetLength();
    for (int i = 0; i < len; ++i) {
        result = result->Append(source->Get(i));
    }

    return result;
}

/**
 * @brief Универсальный конвертер: меняет тип реализации на противоположный.
 * Если был Массив станет Список.
 * Если был Список станет Массив.
 * Если тип неизвестен конвертирует в Массив.
 */
template <class T>
Sequence<T>* ConvertToOpposite(Sequence<T>* source) {
    if (source == nullptr) {
        return nullptr;
    }

    // Проверяем, является ли источник массивом
    if (dynamic_cast<BaseArraySequence<T>*>(source) != nullptr) {
        return ConvertToListSequence(source);
    }

    // Проверяем, является ли источник списком
    if (dynamic_cast<BaseListSequence<T>*>(source) != nullptr) {
        return ConvertToArraySequence(source);
    }

    // По умолчанию (или если это какой-то другой тип) конвертируем в массив
    return ConvertToArraySequence(source);
}

#endif // CONVERTERS_H