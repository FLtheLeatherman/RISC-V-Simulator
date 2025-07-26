#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "Utility.hpp"

template<typename T>
class Register {
private:
    T val, nxt;
public:
    T get_val();
    T get_nxt();
    void set_val(T);
    void set_nxt(T);
    void tick();
    operator T();
    void operator =(T RHS);
};

template<typename T>
T Register<T>::get_val() {
    return val;
}
template<typename T>
T Register<T>::get_nxt() {
    return nxt;
}
template<typename T>
void Register<T>::set_val(T v) {
    val = v;
}
template<typename T>
void Register<T>::set_nxt(T v) {
    nxt = v;
}
template<typename T>
void Register<T>::tick() {
    val = nxt;
}
template<typename T>
Register<T>::operator T() {
    return val;
}
template<typename T>
void Register<T>::operator =(T RHS) {
    nxt = RHS;
}

#endif // REGISTER_HPP