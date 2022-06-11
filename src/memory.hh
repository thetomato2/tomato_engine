#ifndef TOMATO_MEMORY_HH_
#define TOMATO_MEMORY_HH_

#include "core.hh"

namespace tom
{

struct memory_arena
{
    szt size;
    u8 *base;
    szt used;
};

inline void init_arena(memory_arena *arena, const szt size, void *base)
{
    arena->size = size;
    arena->base = scast(byt *, base);
    arena->used = 0;
}

inline void *push_size(memory_arena *arena, szt size)
{
    TOM_ASSERT((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return result;
}

inline void zero_size(void *ptr, szt size)
{
#if USE_CRT
    memset(ptr, 0, size);
#else
    // TODO: profile this for performance
    byt *byte = scast(byt *, ptr);
    while (size--) {
        *byte++ = 0;
    }
#endif
}

#define PUSH_STRUCT(arena, type)       (type *)push_size(arena, sizeof(type))
#define PUSH_ARRAY(arena, count, type) (type *)push_size(arena, (count * sizeof(type)))
#define ZERO_STRUCT(inst)              zero_size(&(inst), sizeof(inst))

}  // namespace tom

#endif  // !TOMATO_MEMORY_HPP_