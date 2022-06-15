#ifndef TOMATO_VECTOR_HH_
#define TOMATO_VECTOR_HH_

#include "core.hh"

namespace tom
{

// ============================================================================================
// Simple custom vector class that stores data on the heap
// Useful for temporary allocations that you don't know the end size of
// TODO: make this good
// TODO: use new byte[sizeof(T)]?? do I fucking care?
// TODO: custom generic memory allocator
// TODO: start with no malloc
// TODO: option to use game memory block?
// TODO:  better grow algorithm?
// ============================================================================================

template<typename T>
class vector
{
public:
    vector()
    {
        _capacity = 0;
        _size     = 0;
        _buf      = nullptr;
    }

    vector(szt capacity)
    {
        _capacity = capacity;
        _size     = 0;
        _buf      = malloc(sizeof(T) * _capacity);
    }

    ~vector() { clear(); }

    vector(const vector &other)
    {
        _capacity = 2;
        _size     = 0;
        _buf      = malloc(sizeof(T) * _capacity);
        resize(other.size());
        memcpy(_buf, other.data(), sizeof(T) * _size);
    }

    vector &operator=(const vector &rhs)
    {
        clear();
        _capacity = rhs._capacity;
        _size     = rhs._size;
        _buf      = malloc(sizeof(T) * _size);
        memcpy(_buf, rhs.data(), sizeof(T) * _size);

        return *this;
    }

    vector(vector &&other)
    {
        _size      = other._size;
        _capacity  = other._capacity;
        _buf       = other._buf;
        other._buf = nullptr;
    }

    vector &operator=(vector &&rhs)
    {
        clear();
        _size     = rhs._size;
        _capacity = rhs._capacity;
        _buf      = rhs._buf;
        rhs._buf  = nullptr;

        return *this;
    }

    void push_back(const T &item)
    {
        if (_size >= _capacity) {
            grow();
        }
        // reinterpret_cast<T *>(_buf)[_size++] = item;
        T *loc = &(reinterpret_cast<T *>(_buf)[_size++]);
        new (loc) T(item);
    }

    T &back()
    {
        TOM_ASSERT(_size > 0);
        return *(reinterpret_cast<T *>(_buf) + _size - 1);
    }

    void pop_back()
    {
        if (_size != 0) reinterpret_cast<T *>(_buf)[--_size].~T();
    }

    template<class... Args>
    void emplace_back(Args const &...args)
    {
        if (_size >= _capacity) {
            grow();
        }
        T *loc = &(reinterpret_cast<T *>(_buf)[_size++]);
        new (loc) T(args...);
    }

    void resize(szt new_size)
    {
        if (new_size > _capacity) {
            grow(new_size);
        }

        if (new_size < _size) {
            auto data_ptr = reinterpret_cast<T *>(_buf);
            for (szt i = new_size; i < _size; ++i) {
                data_ptr[i].~T();
            }
        }

        _size = new_size;
    }

    void reserve(szt new_capacity)
    {
        if (new_capacity > _capacity) {
            grow(new_capacity);
        }
    }

    void clear()
    {
        if (_buf) {
            auto data_ptr = reinterpret_cast<T *>(_buf);
            for (szt i = 0; i < _size; ++i) {
                data_ptr[i].~T();
            }
            free(_buf);
            _buf = nullptr;
        }
        _size     = 0;
        _capacity = 0;
    }

    szt size() const { return _size; }
    szt capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }
    void *data() { return (void *)_buf; }
    void *data() const { return (void *)_buf; }

    T *begin() { return reinterpret_cast<T *>(_buf); }
    T *end() { return reinterpret_cast<T *>(_buf) + _size; }
    T const *begin() const { return reinterpret_cast<T *>(_buf); }
    T const *end() const { return reinterpret_cast<T *>(_buf) + _size; }

    T &operator[](szt i)
    {
        TOM_ASSERT(i < _size);
        return reinterpret_cast<T *>(_buf)[i];
    }

    T const &operator[](szt i) const
    {
        TOM_ASSERT(i < _size);
        return reinterpret_cast<T *>(_buf)[i];
    }

private:
    void grow()
    {
        if (_buf) {
            szt new_capacity = _capacity * 2;
            void *new_buf    = malloc(sizeof(T) * new_capacity);

            // NOTE: this isn't compeletly safe, ex: the class has a pointer to itself
            void *old_buf = _buf;
            memcpy(new_buf, old_buf, _size * sizeof(T));

            _buf      = new_buf;
            _capacity = new_capacity;

            free(old_buf);
        } else {
            TOM_ASSERT(_capacity == 0);
            TOM_ASSERT(_size == 0);

            _capacity = 1;
            _buf      = malloc(sizeof(T));
        }
    }

    void grow(szt new_capacity)
    {
        if (new_capacity > _capacity) {
            void *new_buf = malloc(sizeof(T) * new_capacity);

            // NOTE: this isn't compeletly safe, ex: the class has a pointer to itself
            void *old_buf = _buf;
            memcpy(new_buf, old_buf, _size * sizeof(T));

            _buf      = new_buf;
            _capacity = new_capacity;

            free(old_buf);
        }
    }

    void *_buf;
    szt _size;
    szt _capacity;
};

#ifdef NOT_FINISHED

// ============================================================================================
// Vector that grows inside an arena
// Seems dangerous if you don't put it at the end of the stack
// TODO: Arena allocator
// ============================================================================================
template<typename T, void *Start>
class arena_vector
{
public:
    arena_vector()
    {
        _size = 0;
        _data = Start;
    }

    ~arena_vector() {}

    arena_vector(const arena_vector &other)
    {
        _size = other._size;
        _data = Start;
        memcpy(_data, other.data(), sizeof(T) * _size);
    }

    arena_vector &operator=(const arena_vector &lhs)
    {
        _size = lhs._size;
        _data = Start;
        memcpy(_data, lhs.data(), sizeof(T) * _size);
    }
    arena_vector(arena_vector &&)            = delete;
    arena_vector &operator=(arena_vector &&) = delete;

    void push_back(const T &item) { reinterpret_cast<T *>(_data)[_size++] = item; }

    T *back()
    {
        TOM_ASSERT(_size > 0);
        return reinterpret_cast<T *>(_data) + _size - 1;
    }

    void pop_back()
    {
        if (_size != 0) reinterpret_cast<T *>(_data)[--_size].~T();
    }

    template<class... Args>
    void emplace_back(Args const &...args)
    {
        T *loc = &(reinterpret_cast<T *>(_data)[_size++]);
        new (loc) T(args...);
    }

    void clear()
    {
        auto data_ptr = reinterpret_cast<T *>(_data);
        for (szt i = 0; i < _size; ++i) {
            data_ptr[i].~T();
        }
        _size = 0;
    }

    szt size() const { return _size; }
    bool empty() const { return _size == 0; }
    void *data() { return (void *)_data; }
    void *data() const { return (void *)_data; }
    void *back() { return _data + sizeof(T) * _size; }

    T *begin() { return reinterpret_cast<T *>(_data); }
    T *end() { return reinterpret_cast<T *>(_data) + _size; }
    T const *begin() const { return reinterpret_cast<T *>(_data); }
    T const *end() const { return reinterpret_cast<T *>(_data) + _size; }

    T &operator[](szt i)
    {
        TOM_ASSERT(i < _size);
        return reinterpret_cast<T *>(_data)[i];
    }

    T const &operator[](szt i) const
    {
        TOM_ASSERT(i < _size);
        return reinterpret_cast<T *>(_data)[i];
    }

private:
    void *_data;
    szt _size;
};

#endif

}  // namespace tom
#endif
