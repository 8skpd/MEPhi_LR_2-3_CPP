#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Exceptions.h"

template <class T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& val) : value(val), next(nullptr) {}
    };

    Node* head;
    int length;

    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        length = 0;
    }

    void copyFrom(const LinkedList<T>& other) {
        clear();
        Node* current = other.head;
        Node* tail = nullptr;
        while (current != nullptr) {
            Node* newNode = new Node(current->value);
            if (head == nullptr) {
                head = newNode;
            } else {
                tail->next = newNode;
            }
            tail = newNode;
            current = current->next;
            length++;
        }
    }

public:
    LinkedList() : head(nullptr), length(0) {}

    LinkedList(T* items, int count) : head(nullptr), length(0) {
        Node* tail = nullptr;
        for (int i = 0; i < count; i++) {
            Node* newNode = new Node(items[i]);
            if (head == nullptr) {
                head = newNode;
            } else {
                tail->next = newNode;
            }
            tail = newNode;
            length++;
        }
    }

    LinkedList(const LinkedList<T>& other) : head(nullptr), length(0) {
        copyFrom(other);
    }

    ~LinkedList() {
        clear();
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this != &other) {
            copyFrom(other);
        }
        return *this;
    }

    size_t GetLength() const {
        return length;
    }

    T GetFirst() const {
        if (head == nullptr) throw EmptyStructureException();
        return head->value;
    }

    T GetLast() const {
        if (head == nullptr) throw EmptyStructureException();
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        return current->value;
    }

    T Get(int index) const {
        if (index < 0 || index >= length) throw IndexOutOfRange();
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->value;
    }

    void Append(T item) {
        Node* newNode = new Node(item);
        if (head == nullptr) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
        length++;
    }

    void Prepend(T item) {
        Node* newNode = new Node(item);
        newNode->next = head;
        head = newNode;
        length++;
    }

    void InsertAt(T item, int index) {
        if (index < 0 || index >= length) throw IndexOutOfRange();
        if (index == 0) {
            Prepend(item);
            return;
        }
        Node* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        Node* newNode = new Node(item);
        newNode->next = current->next;
        current->next = newNode;
        length++;
    }

    LinkedList<T>* GetSubList(int start, int end) const {
        if (start < 0 || end >= length || start > end) throw IndexOutOfRange();
        LinkedList<T>* result = new LinkedList<T>();
        Node* current = head;
        for (int i = 0; i < start; i++) {
            current = current->next;
        }
        for (int i = start; i <= end; i++) {
            result->Append(current->value);
            current = current->next;
        }
        return result;
    }

    LinkedList<T>* Concat(LinkedList<T>* list) const {
        LinkedList<T>* result = new LinkedList<T>(*this);
        Node* current = list->head;
        while (current != nullptr) {
            result->Append(current->value);
            current = current->next;
        }
        return result;
    }
};

#endif