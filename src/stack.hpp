#ifndef TOMATO_STACK_HPP_
#define TOMATO_STACK_HPP_

#include "core.hpp"

/* ===========================================================================================
* Static sized stack that sits on the... stack.
*You can access its members with the [] operator, just be careful about deleting stuff
*============================================================================================*/

namespace tom
{

template<typename T, szt Size>
class stack
{
public:
    stack()
    {
        _size = Size;
        _head = 0;
    }
    ~stack() {}

    stack(const stack &other)
    {
        for (szt i = 0; i < _size; ++i) {
            if (i < other._size) {
                _data[i] = other._data[i];
            }
        }
    }

    stack &operator=(const stack &lhs)
    {
        for (szt i = 0; i < _size; ++i) {
            if (i < lhs._size) {
                _data[i] = lhs._data[i];
            }
        }
    }

    stack(stack &&) = delete;
    stack &operator=(stack &&) = delete;

    // NOTE: does a placement new
    template<class... Args>
    T *push(Args const &...args)
    {
        T *result = nullptr;

        if (_head < _size) {
            if (_head != 0)
                result = new (&_data[++_head]) T(args...);
            else
                result = new (&_data[_head]) T(args...);
        } else {
            printf("Stack is full!\n");
            INVALID_CODE_PATH;
        }
        return result;
    }

    void pop()
    {
        if (_head == 0) {
            printf("Stack is empty!\n");
            return;
        }

        _data[_head--].~T();
    }

    T *front() { return *_data + _head; }
    szt head() const { return _head; }
    szt size() const { return _size; }
    T *data() { return _data; }

    T *begin() { return _data; }
    T *end() { return _data + _head; }
    T const *begin() const { return _data; }
    T const *end() const { return _data + _head; }

    T &operator[](szt i)
    {
        if (i > _head) {
            printf("Stack: tried to access data past head!\n");
            INVALID_CODE_PATH;
        }

        return _data[i];
    }

    T const &operator[](szt i) const
    {
        if (i > _head) {
            printf("Stack: tried to access data past head!\n");
            INVALID_CODE_PATH;
        }

        return _data[i];
    }

private:
    T _data[Size];
    szt _size;
    szt _head;
};

}  // namespace tom
#endif