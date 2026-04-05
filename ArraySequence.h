#ifndef ARRAY_SEQUENCE_H
#define ARRAY_SEQUENCE_H

#include "Sequence.h"
#include "DynamicArray.h"

template <class T> class MutableArraySequence;
template <class T> class ImmutableArraySequence;

template <class T>
class BaseArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* items;

    virtual BaseArraySequence<T>* Instance() = 0;
    virtual BaseArraySequence<T>* Clone() const override = 0;

    // Вспомогательные методы изменения
    BaseArraySequence<T>* AppendInternal(T item) {
        if (!items) items = new DynamicArray<T>(0);
        items->Resize(items->GetSize() + 1);
        items->Set(items->GetSize() - 1, item);
        return this;
    }
    
    BaseArraySequence<T>* InsertAtInternal(T item, int index) {
        int size = items->GetSize();
        if (index < 0 || index >= size) throw IndexOutOfRange();
        items->Resize(size + 1);
        for (int i = size - 1; i >= index; i--) items->Set(i + 1, items->Get(i));
        items->Set(index, item);
        return this;
    }

    BaseArraySequence<T>* PrependInternal(T item) {
        if (!items) items = new DynamicArray<T>(0);
        int size = items->GetSize();
        items->Resize(size + 1);
        for(int i = size - 1; i >= 0; --i) items->Set(i+1, items->Get(i));
        items->Set(0, item);
        return this;
    }

public:
    BaseArraySequence() : items(new DynamicArray<T>(0)) {}
    BaseArraySequence(T* arr, int count) : items(new DynamicArray<T>(arr, count)) {}
    BaseArraySequence(const BaseArraySequence& other) : items(new DynamicArray<T>(*other.items)) {}
    ~BaseArraySequence() override { delete items; }

    // Реализация интерфейса Sequence
    T Get(int index) const override {
         return items->Get(index);
    }
    int GetLength() const override { 
        return items->GetSize(); 
    }
    T GetFirst() const override { 
        if(!items->GetSize()) throw EmptyStructureException(); 
        return items->Get(0); 
    }
    T GetLast() const override { 
        if(!items->GetSize()) throw EmptyStructureException(); 
        return items->Get(items->GetSize()-1); 
    }
    
    T& operator[](int index) override { 
        return (*items)[index]; 
    }
    const T& operator[](int index) const override { 
        return (*items)[index]; 
    }

    Sequence<T>* Append(T item) override { 
        return Instance()->AppendInternal(item); 
    }
    Sequence<T>* Prepend(T item) override { 
        return Instance()->PrependInternal(item); 
    }
    Sequence<T>* InsertAt(T item, int index) override { 
        if(index < 0 || index > items->GetSize()) throw IndexOutOfRange();
        return Instance()->InsertAtInternal(item, index); 
    }
    
    Sequence<T>* Concat(Sequence<T>* list) override {
        BaseArraySequence<T>* inst = Instance();
        for(int i=0; i<list->GetLength(); ++i) inst->AppendInternal(list->Get(i));
        return inst;
    }

    Sequence<T>* GetSubsequence(int start, int end) override {
        if(start < 0 || end >= items->GetSize() || start > end) throw IndexOutOfRange();
        BaseArraySequence<T>* res = static_cast<BaseArraySequence<T>*>(this->CreateEmpty());
        delete res->items; 
        DynamicArray<T>* newItems = new DynamicArray<T>(end - start + 1);
        for(int i=0; i <= end-start; ++i) newItems->Set(i, items->Get(start+i));
        res->items = newItems;
        return res;
    }

    T TryGet(int index, bool& success) override {
        try { 
            success=true; 
            return Get(index); 
        } catch(...) { 
            success=false; 
            return T(); 
        }
    }

    T TryFirst(bool& success) override {
        try { 
            success=true; 
            return GetFirst(); 
        } catch(...) { 
            success=false; 
            return T(); 
        }
    }

    T TryLast(bool& success) override {
        try { 
            success=true; 
            return GetLast(); 
        } catch(...) { 
            success=false; 
            return T(); 
        }
    }

    typename Sequence<T>::IEnumerator* GetEnumerator() override {
        return new typename Sequence<T>::SequenceEnumerator(this, this->items->GetSize());
    }

    // Объявления шаблонных методов
    template<typename Func>
    Sequence<T>* Map(Func f);

    template<typename Func>
    Sequence<T>* Where(Func predicate);

    template<typename Func, typename U>
    U Reduce(Func f, U initial);

    template<typename U>
    Sequence<Pair<T, U>>* Zip(Sequence<U>* other);

    virtual Sequence<T>* CreateEmpty() const override = 0;
};

// Наследники
template <class T>
class MutableArraySequence : public BaseArraySequence<T> {
protected:

    BaseArraySequence<T>* Instance() override { 
        return this; 
    }

    BaseArraySequence<T>* Clone() const override { 
        return new MutableArraySequence<T>(*this); 
    }

public:
    MutableArraySequence() : BaseArraySequence<T>() {}
    MutableArraySequence(T* arr, int count) : BaseArraySequence<T>(arr, count) {}
    Sequence<T>* CreateEmpty() const override { return new MutableArraySequence<T>(); }
};

template <class T>
class ImmutableArraySequence : public BaseArraySequence<T> {
protected:
    BaseArraySequence<T>* Instance() override { 
        return this->Clone(); 
    }
    BaseArraySequence<T>* Clone() const override { 
        return new ImmutableArraySequence<T>(*this); 
    }
public:
    ImmutableArraySequence() : BaseArraySequence<T>() {}
    ImmutableArraySequence(T* arr, int count) : BaseArraySequence<T>(arr, count) {}
    Sequence<T>* CreateEmpty() const override { 
        return new ImmutableArraySequence<T>(); 
    }
};

// Реализация шаблонов

template <class T>
template<typename Func>
Sequence<T>* BaseArraySequence<T>::Map(Func f) {
    Sequence<T>* result = this->CreateEmpty();
    if(!result) return nullptr;
    for(int i=0; i<this->items->GetSize(); ++i) {
        Sequence<T>* next = result->Append(f(this->items->Get(i)));
        if(next != result) delete result;
        result = next;
        if(!result) return nullptr;
    }
    return result;
}

template <class T>
template<typename Func>
Sequence<T>* BaseArraySequence<T>::Where(Func predicate) {
    Sequence<T>* result = this->CreateEmpty();
    if(!result) return nullptr;
    for(int i=0; i<this->items->GetSize(); ++i) {
        if(predicate(this->items->Get(i))) {
            Sequence<T>* next = result->Append(this->items->Get(i));
            if(next != result) delete result;
            result = next;
            if(!result) return nullptr;
        }
    }
    return result;
}

template <class T>
template<typename Func, typename U>
U BaseArraySequence<T>::Reduce(Func f, U initial) {
    U res = initial;
    for(int i=0; i<this->items->GetSize(); ++i) res = f(res, this->items->Get(i));
    return res;
}

template <class T>
template<typename U>
Sequence<Pair<T, U>>* BaseArraySequence<T>::Zip(Sequence<U>* other) {
    int len = (this->items->GetSize() < other->GetLength()) ? this->items->GetSize() : other->GetLength();
    Pair<T, U>* arr = new Pair<T, U>[len];
    for(int i=0; i<len; ++i) arr[i] = MakePair(this->items->Get(i), other->Get(i));
    MutableArraySequence<Pair<T, U>>* res = new MutableArraySequence<Pair<T, U>>(arr, len);
    delete[] arr;
    return res;
}

#endif