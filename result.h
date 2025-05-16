#pragma once
#include <variant>
using namespace std;

template<typename T, typename E>
class Result {
    variant<T, E> value;
public:
    Result(const T& val) : value(val) {}
    Result(const E& err) : value(err) {}

    bool is_ok() const { return holds_alternative<T>(value); }
    T unwrap() const { return get<T>(value); }
    E error() const { return get<E>(value); }
};
