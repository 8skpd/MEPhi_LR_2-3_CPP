#ifndef OPTION_H
#define OPTION_H

#include <iostream>
#include <stdexcept>

template <class T>
class Option {
private:
    bool hasValue;
    T value;

public:
    Option() : hasValue(false), value(T()) {}
    Option(const T& val) : hasValue(true), value(val) {}

    bool IsSome() const { return hasValue; }
    bool IsNone() const { return !hasValue; }

    T GetValue() const {
        if (!hasValue) throw std::runtime_error("Accessing None value in Option");
        return value;
    }

    explicit operator bool() const { return hasValue; }

    static Option<T> None() { return Option<T>(); }
    static Option<T> Some(const T& val) { return Option<T>(val); }
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Option<T>& opt) {
    if (opt.IsSome()) os << "Some(" << opt.GetValue() << ")";
    else os << "None";
    return os;
}

#endif