#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Exceptions.h"
#include "MyUtility.h"
#include "Option.h"

template <class T> class Sequence;
template <class T> class BaseArraySequence;
template <class T> class BaseListSequence;
template <class T> class MutableArraySequence;
template <class T> class MutableListSequence;

template <class T>
class IEnumerator {
public:
    virtual bool MoveNext() = 0;
    virtual T GetCurrent() const = 0;
    virtual void Reset() = 0;
    virtual ~IEnumerator() {}
};

template <class T>
class IEnumerable {
public:
    virtual IEnumerator<T>* GetEnumerator() = 0;
    virtual ~IEnumerable() {}
};

template <class T>
class SequenceEnumerator : public IEnumerator<T> {
private:
    const Sequence<T>* sequence;
    int currentIndex;
    int length;

public:
    SequenceEnumerator(const Sequence<T>* seq, int len) 
        : sequence(seq), currentIndex(-1), length(len) {}

    bool MoveNext() override {
        currentIndex++;
        return currentIndex < length;
    }

    T GetCurrent() const override {
        if (currentIndex < 0 || currentIndex >= length) {
            throw IndexOutOfRange(); 
        }
        return sequence->Get(currentIndex);
    }

    void Reset() override {
        currentIndex = -1;
    }

    ~SequenceEnumerator() override {}
};

template <class T>
class Sequence : public IEnumerable<T> {
public:

    using IEnumerator = ::IEnumerator<T>;
    using IEnumerable = ::IEnumerable<T>;
    using SequenceEnumerator = ::SequenceEnumerator<T>;

    // Основной интерфейс
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;
    virtual Sequence<T>* GetSubsequence(int start, int end) = 0;

    virtual Option<T> TryGet(int index) const = 0;
    virtual Option<T> TryFirst() const = 0;
    virtual Option<T> TryLast() const = 0;
    
    virtual T& operator[](int index) = 0;
    virtual const T& operator[](int index) const = 0;

    virtual Sequence<T>* Clone() const = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;

    IEnumerator* GetEnumerator() override {
        return new SequenceEnumerator(this, this->GetLength());
    }

    template<typename U>
    Sequence<Pair<T, U>>* Zip(Sequence<U>* other);

    Sequence<T>* EnsureArray() {
        // Пытаемся привести текущий объект к массиву
        BaseArraySequence<T>* asArray = dynamic_cast<BaseArraySequence<T>*>(this);
        if (asArray != nullptr) {
            return asArray; 
        }

        // Если не массив, создаем новый массив и копируем данные
        // Используем MutableArraySequence напрямую, так как мы сделали forward declaration
        Sequence<T>* newArray = new MutableArraySequence<T>();
        int len = this->GetLength();
        for (int i = 0; i < len; ++i) {
            newArray = newArray->Append(this->Get(i));
        }
        return newArray;
    }

    /**
     * @brief Гарантирует, что последовательность является списком
     */
    Sequence<T>* EnsureList() {
        // Пытаемся привести текущий объект к списку
        BaseListSequence<T>* asList = dynamic_cast<BaseListSequence<T>*>(this);
        if (asList != nullptr) {
            return asList; 
        }

        // Если не список, создаем новый список и копируем данные
        Sequence<T>* newList = new MutableListSequence<T>();
        int len = this->GetLength();
        for (int i = 0; i < len; ++i) {
            newList = newList->Append(this->Get(i));
        }
        return newList;
    }

    /**
     * @brief Конвертирует в массив (синоним EnsureArray для удобства)
     */
    Sequence<T>* ToArray() {
        return EnsureArray();
    }

    /**
     * @brief Конвертирует в список (синоним EnsureList для удобства)
     */
    Sequence<T>* ToList() {
        return EnsureList();
    }

    virtual ~Sequence() {}
};

#endif