#ifndef LIST_SEQUENCE_H
#define LIST_SEQUENCE_H

#include "Sequence.h"
#include "Option.h"
#include "LinkedList.h"
#include "MyUtility.h"
#include <iostream>

template <class T> class MutableListSequence;
template <class T> class ImmutableListSequence;

template <class T>
class BaseListSequence : public Sequence<T> {
protected:
    LinkedList<T>* list;

    // Виртуальные методы фабрики и клонирования
    virtual BaseListSequence<T>* Instance() = 0;
    virtual BaseListSequence<T>* Clone() const override = 0;

    // Защищенные методы изменения (для внутреннего использования)
    BaseListSequence<T>* AppendInternal(T item) {
        list->Append(item);
        return this;
    }

    BaseListSequence<T>* PrependInternal(T item) {
        list->Prepend(item);
        return this;
    }

    BaseListSequence<T>* InsertAtInternal(T item, int index) {
        list->InsertAt(item, index);
        return this;
    }

public:
    // Конструкторы
    BaseListSequence() {
        list = new LinkedList<T>();
    }

    BaseListSequence(T* items, int count) {
        list = new LinkedList<T>(items, count);
    }

    BaseListSequence(const BaseListSequence<T>& other) {
        list = new LinkedList<T>(*other.list);
    }

    virtual ~BaseListSequence() {
        delete list;
    }

    T& operator[](int index) override {
        throw std::logic_error("Operator[] write not supported for ListSequence efficiently");
    }

    const T& operator[](int index) const override {
        static T temp; 
        temp = list->Get(index); 
        return temp;
    }

    // Итератор
    typename Sequence<T>::IEnumerator* GetEnumerator() override {
        return new typename Sequence<T>::SequenceEnumerator(this, this->list->GetLength());
    }

    // Шаблонные методы
    template<typename Func>
    Sequence<T>* Map(Func f);

    template<typename Func>
    Sequence<T>* Where(Func predicate);

    template<typename Func, typename U>
    U Reduce(Func f, U initial);

    template<typename U>
    Sequence<Pair<T, U>>* Zip(Sequence<U>* other);

    // Try-Semantics
    Option<T> TryGet(int index) const override {
        try {
            return Option<T>::Some(this->list->Get(index));
        } catch (...) {
            return Option<T>::None();
        }
    }

    Option<T> TryFirst() const override {
        if (this->list->GetLength() == 0) {
            return Option<T>::None();
        }
        return Option<T>::Some(this->list->GetFirst());
    }

    Option<T> TryLast() const override {
        if (this->list->GetLength() == 0) {
            return Option<T>::None();
        }
        return Option<T>::Some(this->list->GetLast());
    }

    // Реализация интерфейса Sequence через Instance()
    Sequence<T>* Append(T item) override {
        return this->Instance()->AppendInternal(item);
    }

    Sequence<T>* Prepend(T item) override {
        return this->Instance()->PrependInternal(item);
    }

    Sequence<T>* InsertAt(T item, int index) override {
        if (index < 0 || index > list->GetLength()) throw IndexOutOfRange();
        return this->Instance()->InsertAtInternal(item, index);
    }

    Sequence<T>* Concat(Sequence<T>* seq) override {
        BaseListSequence<T>* instance = this->Instance();
        int len = seq->GetLength();
        for (int i = 0; i < len; i++) {
            instance->list->Append(seq->Get(i));
        }
        return instance;
    }

    Sequence<T>* GetSubsequence(int start, int end) override {
        if (start < 0 || end >= list->GetLength() || start > end) throw IndexOutOfRange();
        
        // Создаем новую пустую последовательность того же типа
        BaseListSequence<T>* result = static_cast<BaseListSequence<T>*>(this->CreateEmpty());
        
        // Заменяем её внутренний список на подсписок
        delete result->list;
        result->list = list->GetSubList(start, end); // Предполагаем, что GetSubList возвращает новый LinkedList
        
        return result;
    }

    // Геттеры
    T Get(int index) const override {
        return list->Get(index);
    }

    int GetLength() const override {
        return list->GetLength();
    }

    T GetFirst() const override {
        if (list->GetLength() == 0) throw EmptyStructureException();
        return list->GetFirst();
    }

    T GetLast() const override {
        if (list->GetLength() == 0) throw EmptyStructureException();
        return list->GetLast();
    }

    // Фабричный метод (реализуется в наследниках)
    Sequence<T>* CreateEmpty() const override = 0;
};


template <class T>
class MutableListSequence : public BaseListSequence<T> {
protected:
    BaseListSequence<T>* Instance() override {
        return this; // Mutable: меняем себя
    }

    BaseListSequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }

public:
    MutableListSequence() : BaseListSequence<T>() {}
    MutableListSequence(T* items, int count) : BaseListSequence<T>(items, count) {}
    MutableListSequence(const MutableListSequence<T>& other) : BaseListSequence<T>(other) {}

    Sequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }
};

template <class T>
class ImmutableListSequence : public BaseListSequence<T> {
protected:
    BaseListSequence<T>* Instance() override {
        return this->Clone(); // Immutable: создаем копию и меняем её
    }

    BaseListSequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

public:
    ImmutableListSequence() : BaseListSequence<T>() {}
    ImmutableListSequence(T* items, int count) : BaseListSequence<T>(items, count) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : BaseListSequence<T>(other) {}

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }
};

// Шаблонные методы
template <class T>
template<typename Func>
Sequence<T>* BaseListSequence<T>::Map(Func f) {
    // 1. Создаем пустую последовательность того же типа
    Sequence<T>* result = this->CreateEmpty();
    if (result == nullptr) return nullptr;

    // 2. Проходим по элементам
    for (int i = 0; i < this->list->GetLength(); i++) {
        T newVal = f(this->list->Get(i));
        
        // 3. Добавляем через публичный Append с присваиванием результата!
        Sequence<T>* nextResult = result->Append(newVal);
        
        if (nextResult == nullptr) {
            delete result;
            return nullptr;
        }

        // Если указатель изменился (Immutable), удаляем старый объект
        if (nextResult != result) {
            delete result;
        }
        result = nextResult;
    }
    return result;
}

template <class T>
template<typename Func>
Sequence<T>* BaseListSequence<T>::Where(Func predicate) {
    Sequence<T>* result = this->CreateEmpty();
    if (result == nullptr) return nullptr;

    for (int i = 0; i < this->list->GetLength(); i++) {
        if (predicate(this->list->Get(i))) {
            Sequence<T>* nextResult = result->Append(this->list->Get(i));
            if (nextResult == nullptr) {
                delete result;
                return nullptr;
            }
            if (nextResult != result) {
                delete result;
            }
            result = nextResult;
        }
    }
    return result;
}

template <class T>
template<typename Func, typename U>
U BaseListSequence<T>::Reduce(Func f, U initial) {
    U result = initial;
    for (int i = 0; i < this->list->GetLength(); i++) {
        result = f(result, this->list->Get(i));
    }
    return result;
}

template <class T>
template<typename U>
Sequence<Pair<T, U>>* BaseListSequence<T>::Zip(Sequence<U>* other) {
    int minLen = (this->list->GetLength() < other->GetLength()) 
                 ? this->list->GetLength() 
                 : other->GetLength();
    
    Pair<T, U>* tempArr = new Pair<T, U>[minLen];
    for (int i = 0; i < minLen; i++) {
        tempArr[i] = MakePair(this->list->Get(i), other->Get(i));
    }

    // Создаем результат через массив, т.к. конструктор списка от массива есть
    MutableListSequence<Pair<T, U>>* zipRes = new MutableListSequence<Pair<T, U>>(tempArr, minLen);
    delete[] tempArr;
    return zipRes;
}

#endif