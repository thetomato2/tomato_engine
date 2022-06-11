#ifndef TOMATO_SCOPE_HH_
#define TOMATO_SCOPE_HH_

#include "core.hh"

namespace tom
{

template<typename T>
class scoped_ptr
{
public:
    scoped_ptr() : data(nullptr) {}
    explicit scoped_ptr(T *data) : data(data) {}
    ~scoped_ptr() { delete data; }

    scoped_ptr(std::nullptr_t) : data(nullptr) {}
    scoped_ptr &operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }

    // Constructor/Assignment that allows move semantics
    scoped_ptr(scoped_ptr &&moving) noexcept { moving.swap(*this); }
    scoped_ptr &operator=(scoped_ptr &&moving) noexcept
    {
        moving.swap(*this);
        return *this;
    }

    // Constructor/Assignment for use with types derived from T
    template<typename U>
    scoped_ptr(scoped_ptr<U> &&moving)
    {
        scoped_ptr<T> tmp(moving.release());
        tmp.swap(*this);
    }
    template<typename U>
    scoped_ptr &operator=(scoped_ptr<U> &&moving)
    {
        scoped_ptr<T> tmp(moving.release());
        tmp.swap(*this);
        return *this;
    }

    // Remove compiler generated copy semantics.
    scoped_ptr(scoped_ptr const &) = delete;
    scoped_ptr &operator=(scoped_ptr const &) = delete;

    // Const correct access owned object
    T *operator->() const { return data; }
    T &operator*() const { return *data; }

    // Access to smart pointer state
    T *get() const { return data; }
    explicit operator bool() const { return data; }

    // Modify object state
    T *release() noexcept
    {
        T *result = nullptr;
        std::swap(result, data);
        return result;
    }
    void swap(scoped_ptr &src) noexcept { std::swap(data, src.data); }
    void reset()
    {
        T *tmp = release();
        delete tmp;
    }

private:
    T *data;
};

template<typename T>
void swap(scoped_ptr<T> &lhs, scoped_ptr<T> &rhs)
{
    lhs.swap(rhs);
}

class scoped_handle
{
public:
    scoped_handle() = default;
    scoped_handle(HANDLE handle) { _handle = handle; }
    ~scoped_handle() { close(); }

    scoped_handle(const scoped_handle &) = delete;
    void operator=(const scoped_handle &) = delete;
    explicit operator bool() const { return _handle != INVALID_HANDLE_VALUE; }

    HANDLE get() { return _handle; }
    void close()
    {
        if (_handle != INVALID_HANDLE_VALUE) {
            ::CloseHandle(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
    }
    void set(HANDLE handle) { _handle = handle; }

private:
    HANDLE _handle = INVALID_HANDLE_VALUE;
};

// NOTE: handles for FindFirstFile, FindFirstFileEx, FindFirstFileNameW,
// FindFirstFileNameTransactedW, FindFirstFileTransacted, FindFirstStreamTransactedW, or
// FindFirstStreamW functions.
class scoped_search_handle
{
public:
    scoped_search_handle() = default;
    scoped_search_handle(HANDLE handle) { _handle = handle; }
    ~scoped_search_handle() { close(); }

    scoped_search_handle(const scoped_search_handle &) = delete;
    void operator=(const scoped_search_handle &) = delete;
    explicit operator bool() const { return _handle != INVALID_HANDLE_VALUE; }

    HANDLE get() { return _handle; }
    void close()
    {
        if (_handle != INVALID_HANDLE_VALUE) {
            ::FindClose(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
    }
    void set(HANDLE handle) { _handle = handle; }

private:
    HANDLE _handle = INVALID_HANDLE_VALUE;
};

}  // namespace tom

#endif  // !TOMATO_SCOPE_HPP_