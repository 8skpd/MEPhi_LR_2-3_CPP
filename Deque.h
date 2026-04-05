#ifndef DEQUE_H
#define DEQUE_H

#include "Sequence.h"
#include "ArraySequence.h"
#include "MyUtility.h"
#include <functional>
#include <iostream>

template <class T>
class Deque {
private:
    Sequence<T>* data;

public:
    Deque() { 
        data = new MutableArraySequence<T>(); 
    }
    Deque(T* items, int count) { 
        data = new MutableArraySequence<T>(items, count); 
    }
    Deque(Sequence<T>* seq) { 
        data = seq ? seq->Clone() : new MutableArraySequence<T>(); 
    }
    Deque(const Deque& other) { 
        data = other.data->Clone(); 
    }
    Deque& operator=(const Deque& other) {
        if(this != &other) { 
            delete data; 
            data = other.data->Clone(); 
        }
        return *this;
    }
    ~Deque() { 
        delete data; 
    }

    void PushFront(T item) { 
        data = data->Prepend(item); 
    }
    void PushBack(T item) { 
        data = data->Append(item); 
    }
    
    T PopFront() {
        if (data->GetLength() == 0) throw EmptyStructureException();
        
        T val = data->GetFirst();
        
        if (data->GetLength() == 1) {
            Sequence<T>* newEmptySeq = new MutableArraySequence<T>();
            delete data;
            data = newEmptySeq;
        } else {
            Sequence<T>* newSeq = data->GetSubsequence(1, data->GetLength() - 1);
            delete data;
            data = newSeq;
        }
        return val;
    }

    T PopBack() {
        if (data->GetLength() == 0) throw EmptyStructureException();
        
        T val = data->GetLast();

        if (data->GetLength() == 1) {
            Sequence<T>* newEmptySeq = new MutableArraySequence<T>();
            
            delete data;
            data = newEmptySeq;
        } else {
            Sequence<T>* newSeq = data->GetSubsequence(0, data->GetLength() - 2);
            delete data;
            data = newSeq;
        }
        return val;
    }

    T GetFront() const { 
        if(!data->GetLength()) throw EmptyStructureException(); 
        return data->GetFirst(); 
    }
    T GetBack() const { 
        if(!data->GetLength()) throw EmptyStructureException(); 
        return data->GetLast(); 
    }
    int GetLength() const { 
        return data->GetLength(); 
    }
    bool IsEmpty() const { 
        return data->GetLength() == 0; 
    }

    T operator[](int index) const { 
        return data->Get(index); 
    }
    T& operator[](int index) { 
        return (*data)[index]; 
    }
    
    bool operator==(const Deque& other) const {
        if(data->GetLength() != other.data->GetLength()) return false;
        for(int i=0; i<data->GetLength(); ++i) if(data->Get(i) != other.data->Get(i)) return false;
        return true;
    }
    
    Deque<T> operator+(const Deque<T>& other) const {
        Deque<T> result;
        // Очищаем данные нового дека (так как конструктор создал пустую последовательность)
        delete result.data;
        
        // Клонируем левую часть
        Sequence<T>* newSeq = this->data->Clone();
        
        // Дописываем правую часть
        for (int i = 0; i < other.data->GetLength(); ++i) {
            newSeq = newSeq->Append(other.data->Get(i));
        }
        
        result.data = newSeq;
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Deque& dq) {
        os << "[ ";
        for(int i=0; i<dq.GetLength(); ++i) {
            os << dq.data->Get(i);
            if(i < dq.GetLength()-1) os << ", ";
        }
        os << " ]";
        return os;
    }

    // Приведение типа для вызова шаблонных методов
    template<typename Func>
    Deque Map(Func f) const {
        BaseArraySequence<T>* baseSeq = dynamic_cast<BaseArraySequence<T>*>(data);
        if(!baseSeq) throw std::runtime_error("Map requires ArraySequence");
        Sequence<T>* mapped = baseSeq->Map(f);
        Deque res; 
        delete res.data; 
        res.data = mapped; 
        return res;
    }

    template<typename Func>
    Deque Where(Func predicate) const {
        BaseArraySequence<T>* baseSeq = dynamic_cast<BaseArraySequence<T>*>(data);
        if(!baseSeq) throw std::runtime_error("Where requires ArraySequence");
        Sequence<T>* filtered = baseSeq->Where(predicate);
        Deque res; 
        delete res.data; 
        res.data = filtered; 
        return res;
    }

    template<typename Func, typename U>
    U Reduce(Func f, U initial) const {
        BaseArraySequence<T>* baseSeq = dynamic_cast<BaseArraySequence<T>*>(data);
        if(!baseSeq) throw std::runtime_error("Reduce requires ArraySequence");
        return baseSeq->Reduce(f, initial);
    }

    bool ContainsSubsequence(const Deque& sub) const {
        int n = data->GetLength(), m = sub.data->GetLength();
        if(m > n) return false;
        for(int i=0; i<=n-m; ++i) {
            bool ok = true;
            for(int j=0; j<m; ++j) if(data->Get(i+j) != sub.data->Get(j)) { ok=false; break; }
            if(ok) return true;
        }
        return false;
    }

    void Sort() {
        int len = data->GetLength();
        if (len <= 1) return;
        T** ptrs = new T*[len];

        for (int i = 0; i < len; ++i) {
            ptrs[i] = &(*data)[i]; 
        }

        for (int i = 0; i < len - 1; ++i) {
            for (int j = 0; j < len - i - 1; ++j) {
                if (ptrs[j] == nullptr || ptrs[j+1] == nullptr) continue;
                
                if (*ptrs[j] > *ptrs[j + 1]) {
                    T* temp = ptrs[j];
                    ptrs[j] = ptrs[j + 1];
                    ptrs[j + 1] = temp;
                }
            }
        }

        Sequence<T>* sortedSeq = data->CreateEmpty();

        for (int i = 0; i < len; ++i) {
            sortedSeq = sortedSeq->Append(*ptrs[i]);
        }

        delete data;
        data = sortedSeq;
        delete[] ptrs;
    }

    void Merge(Deque& other) {
        Sequence<T>* res = data->CreateEmpty();
        Sequence<T>* s1 = data->Clone();
        Sequence<T>* s2 = other.data->Clone();
        while(s1->GetLength() && s2->GetLength()) {
            if(s1->GetFirst() <= s2->GetFirst()) {
                res = res->Append(s1->GetFirst());
                Sequence<T>* tmp = (s1->GetLength()==1) ? s1->CreateEmpty() : s1->GetSubsequence(1, s1->GetLength()-1);
                delete s1; 
                s1 = tmp;
            } else {
                res = res->Append(s2->GetFirst());
                Sequence<T>* tmp = (s2->GetLength()==1) ? s2->CreateEmpty() : s2->GetSubsequence(1, s2->GetLength()-1);
                delete s2; 
                s2 = tmp;
            }
        }
        while(s1->GetLength()) {
            res = res->Append(s1->GetFirst());
            Sequence<T>* tmp = (s1->GetLength()==1) ? s1->CreateEmpty() : s1->GetSubsequence(1, s1->GetLength()-1);
            delete s1; 
            s1 = tmp;
        }
        while(s2->GetLength()) {
            res = res->Append(s2->GetFirst());
            Sequence<T>* tmp = (s2->GetLength()==1) ? s2->CreateEmpty() : s2->GetSubsequence(1, s2->GetLength()-1);
            delete s2; 
            s2 = tmp;
        }
        delete data; 
        data = res; 
        delete s1; 
        delete s2;
    }

    int CountInversions() const {
        int cnt = 0, len = data->GetLength();
        for(int i=0; i<len; ++i)
            for(int j=i+1; j<len; ++j)
                if(data->Get(i) > data->Get(j)) cnt++;
        return cnt;
    }

    static Deque<int> Range(int start, int end) {
        if(start > end) return Deque<int>();
        int cnt = end - start + 1;
        int* arr = new int[cnt];
        for(int i=0; i<cnt; ++i) arr[i] = start + i;
        Deque res(arr, cnt);
        delete[] arr;
        return res;
    }
};

#endif