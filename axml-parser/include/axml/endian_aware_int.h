#pragma once

#include <cstdint>

namespace axml {

template <typename T, bool LE = true>
class eint {

private:
    T value;

public:
    static bool isLE() {
        short x = 1;
        return ((char*) &x)[0] == 1;
    }

    T get() const {
        if (isLE() == LE)
            return value;
        T val2 = value;
        char* p = (char*) &val2;
        for (int i = 0; i < sizeof(T) / 2; i++)
            p[sizeof(T) - 1 - i] = p[i];
        return val2;
    }

    constexpr operator T() const {
        return get();
    }

};

using eint8 = eint<int8_t>;
using eint16 = eint<int16_t>;
using eint32 = eint<int32_t>;

using euint8 = eint<uint8_t>;
using euint16 = eint<uint16_t>;
using euint32 = eint<uint32_t>;

}