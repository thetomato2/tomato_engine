#ifndef TOMATO_FILE_IO_HPP_
#define TOMATO_FILE_IO_HPP_

#include "core.hpp"

namespace tom
{
struct read_file_result
{
    szt content_size;
    void *contents;
};

read_file_result read_entire_file(const char *file_name);
bool write_entire_file(const char *file_name, u64 memory_size, void *memory);

}  // namespace tom

#endif  // !TOMATO_FILE_IO_HPP_