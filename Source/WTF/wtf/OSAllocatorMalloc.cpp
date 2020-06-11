// This is an implementation of OSAllocator that doesn't use virtual memory.
// It is meant to be used on platforms that do not provide an mmap function.

#include "config.h"
#include <wtf/OSAllocator.h>

#if OS(UNIX)

#include <cassert>
#include <stdlib.h>

namespace WTF {

void* OSAllocator::reserveUncommitted(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
    return reserveAndCommit(bytes, usage, writable, executable, includesGuardPages);
}

void* OSAllocator::reserveAndCommit(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
    assert(!executable && "Allocating executable memory is not supported on this platform.");
    return malloc(bytes);
}

void OSAllocator::commit(void* address, size_t bytes, bool writable, bool executable)
{
    assert(!executable && "Allocating executable memory is not supported on this platform.");
    // Memory is committed on reserve, so nothing to do.
}

void OSAllocator::decommit(void* address, size_t bytes)
{
    // Memory is decomitted on release, so nothing to do.
}

void OSAllocator::hintMemoryNotNeededSoon(void* address, size_t bytes)
{
}

void OSAllocator::releaseDecommitted(void* address, size_t bytes)
{
    free(address);
}

} // namespace WTF

#endif // OS(UNIX)
