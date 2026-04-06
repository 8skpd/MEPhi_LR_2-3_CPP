#ifndef MY_UTILITY_H
#define MY_UTILITY_H

// Аналог std::pair
template <class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() : first(T1()), second(T2()) {}
    
    Pair(const T1& a, const T2& b) : first(a), second(b) {}
};

// Функция для удобного создания пары (аналог std::make_pair)
template <class T1, class T2>
Pair<T1, T2> MakePair(const T1& a, const T2& b) {
    return Pair<T1, T2>(a, b);
}

// Аналог std::swap
template <class T>
void Swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// Аналог std::tuple
template <typename... Types>
class Tuple;

// Базовый случай рекурсии (пустой кортеж)
template <>
class Tuple<> {
public:
    Tuple() {}
};

// Рекурсивный случай
// Наследуется от хвоста, хранит голову
template <typename Head, typename... Tail>
class Tuple<Head, Tail...> : public Tuple<Tail...> {
public:
    Head value;

    Tuple() : Tuple<Tail...>(), value(Head()) {}

    // Конструктор с передачей аргументов
    Tuple(const Head& h, const Tail&... t) 
        : Tuple<Tail...>(t...), value(h) {}
    
    // Конструктор с perfect forwarding
    template<typename H, typename... T>
    Tuple(H&& h, T&&... t) 
        : Tuple<Tail...>(std::forward<T>(t)...), value(std::forward<H>(h)) {}
};

// Общее объявление
template <size_t Index, typename TupleType>
struct TupleElement;

// Специализация: Индекс 0. Берем значение текущего уровня
// Подходит для Tuple<Head, Tail...>
template <typename Head, typename... Tail>
struct TupleElement<0, Tuple<Head, Tail...>> {
    static Head& get(Tuple<Head, Tail...>& t) {
        return t.value;
    }
    static const Head& get(const Tuple<Head, Tail...>& t) {
        return t.value;
    }
};

// Специализация: Индекс > 0. Рекурсивно идем в базовый класс (хвост)
// Подходит для Tuple<Head, Tail...> где нас интересует хвост
template <size_t Index, typename Head, typename... Tail>
struct TupleElement<Index, Tuple<Head, Tail...>> {
    static auto& get(Tuple<Head, Tail...>& t) {
        // Приводим к базовому классу (хвосту) и уменьшаем индекс
        using TailTuple = Tuple<Tail...>;
        return TupleElement<Index - 1, TailTuple>::get(
            static_cast<TailTuple&>(t)
        );
    }
    static const auto& get(const Tuple<Head, Tail...>& t) {
        using TailTuple = Tuple<Tail...>;
        return TupleElement<Index - 1, TailTuple>::get(
            static_cast<const TailTuple&>(t)
        );
    }
};

// Главная функция доступа get<N>(tuple)
template <size_t Index, typename... Types>
auto& get(Tuple<Types...>& t) {
    return TupleElement<Index, Tuple<Types...>>::get(t);
}

template <size_t Index, typename... Types>
const auto& get(const Tuple<Types...>& t) {
    return TupleElement<Index, Tuple<Types...>>::get(t);
}

// Функция MakeTuple
template <typename... Args>
inline Tuple<typename std::decay<Args>::type...> MakeTuple(Args&&... args) {
    return Tuple<typename std::decay<Args>::type...>(std::forward<Args>(args)...);
}

// Утилита для вывода
template <size_t Index = 0, typename... Types>
typename std::enable_if<Index == sizeof...(Types), void>::type
PrintTupleImpl(const Tuple<Types...>&, std::ostream&) {}

template <size_t Index = 0, typename... Types>
typename std::enable_if<Index < sizeof...(Types), void>::type
PrintTupleImpl(const Tuple<Types...>& t, std::ostream& os) {
    if (Index > 0) os << ", ";
    os << get<Index>(t);
    PrintTupleImpl<Index + 1>(t, os);
}

template <typename... Types>
std::ostream& operator<<(std::ostream& os, const Tuple<Types...>& t) {
    os << "(";
    PrintTupleImpl(t, os);
    os << ")";
    return os;
}

#endif