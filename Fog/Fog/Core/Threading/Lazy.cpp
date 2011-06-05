// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lazy.h>
#include <Fog/Core/Threading/Thread.h>

namespace Fog {

// ============================================================================
// [Fog::LazyBase]
// ============================================================================

LazyBase::LazyBase() : _ptr(NULL)
{
}

LazyBase::~LazyBase()
{
  // Pointer should be NULL here
  FOG_ASSERT(_ptr == NULL);
}

void* LazyBase::_get()
{
  void* v = AtomicCore<void*>::get(&_ptr);

  // Already created, just return it.
  if (v != NULL && v != (void*)STATE_CREATING_NOW) return v;

  // Create instance.
  if (AtomicCore<void*>::cmpXchg(&_ptr, (void*)NULL, (void*)STATE_CREATING_NOW))
  {
    v = _create();
    AtomicCore<void*>::set(&_ptr, v);
    return v;
  }

  // Race - rare, but possible!
  while ((v = AtomicCore<void*>::get(&_ptr)) == (void*)STATE_CREATING_NOW)
  {
    Thread::_yield();
  }

  return v;
}

} // Fog namespace