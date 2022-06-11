#ifndef TOMATO_ARRAY_HH_
#define TOMATO_ARRAY_HH_

#include "core.hh"

namespace tom
{

// --------------------------------------------------------------------------------------------
/// C-array wrapper
// --------------------------------------------------------------------------------------------
template<typename T, szt Size>
class array
{
public:
    array() { _size = Size; }
    ~array() {}

    array(const array &other)
    {
        for (szt i = 0; i < _size; ++i) {
            if (i < other._size) {
                _data[i] = other._data[i];
            }
        }
    }

    array &operator=(const array &lhs)
    {
        for (szt i = 0; i < _size; ++i) {
            if (i < lhs._size) {
                _data[i] = lhs._data[i];
            }
        }
    }

    array(array &&) = delete;
    array &operator=(array &&) = delete;

    // NOTE: does a placement new
    template<class... Args>
    void emplace_at(szt i, Args const &...args)
    {
        TOM_ASSERT(i < _size);
        if (i > _size) return;
        new (&_data[i]) T(args...);
    }

    szt size() const { return _size; }
    T *data() { return _data; }

    T *begin() { return _data; }
    T *end() { return _data + _size; }
    T const *begin() const { return _data; }
    T const *end() const { return _data + _size; }

    T &operator[](szt i)
    {
        TOM_ASSERT(i < _size);
        return _data[i];
    }

    T const &operator[](szt i) const
    {
        TOM_ASSERT(i < _size);
        return _data[i];
    }

private:
    T _data[Size];
    szt _size;
};

}  // namespace tom

#endif