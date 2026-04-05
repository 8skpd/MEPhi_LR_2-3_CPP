#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

class Exception {
public:
    virtual const char* what() const = 0;
    virtual ~Exception() {}
};

class IndexOutOfRange : public Exception {
public:
    const char* what() const override {
        return "Index out of range";
    }
};

class EmptyStructureException : public Exception {
public:
    const char* what() const override {
        return "Structure is empty";
    }
};

#endif