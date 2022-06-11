#include "file_io.hh"
#include "scope.hh"

namespace tom
{

internal void free_file_memory(void *memory)
{
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    } else {
        INVALID_CODE_PATH;
    }
}

scoped_file read_entire_file(const char *file_path)
{
    scoped_file file = {};
    file.file_path   = file_path;

    scoped_handle file_handle =
        CreateFileA(file_path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (file_handle) {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(file_handle.get(), &fileSize)) {
            u32 fileSize32 = safe_truncate_u32_to_u64(fileSize.QuadPart);
            file.data      = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (file.data) {
                DWORD bytesRead;
                if (ReadFile(file_handle.get(), file.data, (DWORD)fileSize.QuadPart, &bytesRead,
                             0) &&
                    fileSize32 == bytesRead) {
                    file.size = fileSize32;
                } else {
                    file.reset();
                }
            } else {
                printf("ERROR-> Failed to read file contents!\n");
            }
        } else {
            printf("ERROR-> Failed to open file handle!\n");
        }
    }

    return file;
}

bool write_entire_file(const char *file_path, u64 memory_size, void *memory)
{
    scoped_handle file_handle = CreateFileA(file_path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (!file_handle) {
        printf("ERROR-> Failed to open file handle!\n");
        return false;
    }

    DWORD bytes_written;
    if (WriteFile(file_handle.get(), memory, (DWORD)memory_size, &bytes_written, 0)) {
        if (bytes_written != memory_size) {
            printf("ERROR-> Failed to fully write file contents!\n");
            return false;
        }
    } else {
        printf("ERROR-> Failed to write file contents!\n");
        return false;
    }

    return true;
}

scoped_file::scoped_file() : data(nullptr)
{
}

scoped_file::scoped_file(const char *file_path)
{
    printf("Reading file: %s\n", file_path);
    *this = read_entire_file(file_path);
}

scoped_file::~scoped_file()
{
    printf("Freeing file\n");
    VirtualFree(data, 0, MEM_RELEASE);
}

scoped_file::scoped_file(std::nullptr_t) : data(nullptr)
{
}

scoped_file &scoped_file::operator=(std::nullptr_t)
{
    reset();
    return *this;
}

// Constructor/Assignment that allows move semantics
scoped_file::scoped_file(scoped_file &&moving) noexcept
{
    moving.swap(*this);
}

scoped_file &scoped_file::operator=(scoped_file &&moving) noexcept
{
    moving.swap(*this);
    return *this;
}

// Modify object state
void *scoped_file::release() noexcept
{
    void *result = nullptr;
    std::swap(result, data);
    return result;
}
void scoped_file::swap(scoped_file &src) noexcept
{
    std::swap(data, src.data);
}

void scoped_file::reset()
{
    void *tmp = release();
    VirtualFree(data, 0, MEM_RELEASE);
}

}  // namespace tom
