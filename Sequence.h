#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "Exceptions.h"
#include "MyUtility.h"

template <class T>
class Sequence {
public:

    // Вложенные классы итераторов
    class IEnumerator {
    public:
        virtual bool MoveNext() = 0;
        virtual T GetCurrent() const = 0;
        virtual void Reset() = 0;
        virtual ~IEnumerator() {}
    };

    class IEnumerable {
    public:
        virtual IEnumerator* GetEnumerator() = 0;
        virtual ~IEnumerable() {}
    };

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

    virtual T TryGet(int index, bool& success) = 0;
    virtual T TryFirst(bool& success) = 0;
    virtual T TryLast(bool& success) = 0;
    
    virtual T& operator[](int index) = 0;
    virtual const T& operator[](int index) const = 0;

    virtual Sequence<T>* Clone() const = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;

    // Возвращаем вложенный тип
    virtual IEnumerator* GetEnumerator() {
        return new SequenceEnumerator(this, this->GetLength());
    }

    template<typename U>
    Sequence<Pair<T, U>>* Zip(Sequence<U>* other);

    virtual ~Sequence() {}

protected:

    class SequenceEnumerator : public IEnumerator {
    private:
        const Sequence* sequence;
        int currentIndex;
        int length;

    public:
        SequenceEnumerator(const Sequence* seq, int len) 
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
};

#endif