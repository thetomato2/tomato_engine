#ifndef TOMATO_FILE_IO_HPP_
#define TOMATO_FILE_IO_HPP_

#include "core.hpp"

namespace tom
{

    
// This will automatically free file memory when leaving scope
struct scoped_file
{
    scoped_file();
    scoped_file(const char *file_path);
    ~scoped_file();

    scoped_file(std::nullptr_t);
    scoped_file &operator=(std::nullptr_t);
    // Constructor/Assignment that allows move semantics
    scoped_file(scoped_file &&moving) noexcept;
    scoped_file &operator=(scoped_file &&moving) noexcept;

    // Remove compiler generated copy semantics.
    scoped_file(scoped_file const &) = delete;
    scoped_file &operator=(scoped_file const &) = delete;

    // Access to smart pointer state
    void *get() const { return data; }
    explicit operator bool() const { return data; }

    // Modify object state
    void *release() noexcept;
    void swap(scoped_file &src) noexcept;
    void reset();

    const char *file_path;
    szt size;
    void *data;
};

scoped_file read_entire_file(const char *file_path);
bool write_entire_file(const char *file_path, u64 memory_size, void *memory);

// NOTE: scoped pointer that uses VirtualAlloc and VirtualFree
}  // namespace tom

#endif  // !TOMATO_FILE_IO_HPP_