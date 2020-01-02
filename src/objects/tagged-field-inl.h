// Copyright 2019 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_TAGGED_FIELD_INL_H_
#define V8_OBJECTS_TAGGED_FIELD_INL_H_

#include "src/objects/tagged-field.h"

#include "src/common/ptr-compr-inl.h"

namespace v8 {
namespace internal {

// static
template <typename T, int kFieldOffset>
Address TaggedField<T, kFieldOffset>::address(HeapObject host, int offset) {
  return host.address() + kFieldOffset + offset;
}

// static
template <typename T, int kFieldOffset>
Tagged_t* TaggedField<T, kFieldOffset>::location(HeapObject host, int offset) {
  return reinterpret_cast<Tagged_t*>(address(host, offset));
}

// static
template <typename T, int kFieldOffset>
template <typename TOnHeapAddress>
Address TaggedField<T, kFieldOffset>::tagged_to_full(
    TOnHeapAddress on_heap_addr, Tagged_t tagged_value) {
#ifdef V8_COMPRESS_POINTERS
  if (kIsSmi) {
    return DecompressTaggedSigned(tagged_value);
  } else if (kIsHeapObject) {
    return DecompressTaggedPointer(on_heap_addr, tagged_value);
  } else {
    return DecompressTaggedAny(on_heap_addr, tagged_value);
  }
#else
  return tagged_value;
#endif
}

// static
template <typename T, int kFieldOffset>
Tagged_t TaggedField<T, kFieldOffset>::full_to_tagged(Address value) {
#ifdef V8_COMPRESS_POINTERS
  return CompressTagged(value);
#else
  return value;
#endif
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::load(HeapObject host, int offset) {
  Tagged_t value = *location(host, offset);
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(host.ptr(), value));
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::load(IsolateRoot isolate, HeapObject host,
                                     int offset) {
  Tagged_t value = *location(host, offset);
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(isolate, value));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::store(HeapObject host, T value) {
#ifdef V8_ATOMIC_OBJECT_FIELD_WRITES
  Relaxed_Store(host, value);
#else
  Address ptr = value.ptr();
  if (kFieldOffset == HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  *location(host) = full_to_tagged(ptr);
#endif
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::store(HeapObject host, int offset, T value) {
#ifdef V8_ATOMIC_OBJECT_FIELD_WRITES
  Relaxed_Store(host, offset, value);
#else
  Address ptr = value.ptr();
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  *location(host, offset) = full_to_tagged(ptr);
#endif
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::Relaxed_Load(HeapObject host, int offset) {
  AtomicTagged_t value = AsAtomicTagged::Relaxed_Load(location(host, offset));
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(host.ptr(), value));
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::Relaxed_Load(IsolateRoot isolate,
                                             HeapObject host, int offset) {
  AtomicTagged_t value = AsAtomicTagged::Relaxed_Load(location(host, offset));
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(isolate, value));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Relaxed_Store_No_Pack(HeapObject host,
                                                         T value) {
  AsAtomicTagged::Relaxed_Store(location(host), full_to_tagged(value.ptr()));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Relaxed_Store(HeapObject host, T value) {
  Address ptr = value.ptr();
  if (kFieldOffset == HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  AsAtomicTagged::Relaxed_Store(location(host), full_to_tagged(ptr));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Relaxed_Store(HeapObject host, int offset,
                                                 T value) {
  Address ptr = value.ptr();
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  AsAtomicTagged::Relaxed_Store(location(host, offset), full_to_tagged(ptr));
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::Acquire_Load(HeapObject host, int offset) {
  AtomicTagged_t value = AsAtomicTagged::Acquire_Load(location(host, offset));
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(host.ptr(), value));
}

// static
template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::Acquire_Load_No_Unpack(IsolateRoot isolate, HeapObject host,
                                                       int offset) {
  AtomicTagged_t value = AsAtomicTagged::Acquire_Load(location(host, offset));
  return T(tagged_to_full(isolate, value));
}

template <typename T, int kFieldOffset>
T TaggedField<T, kFieldOffset>::Acquire_Load(IsolateRoot isolate,
                                             HeapObject host, int offset) {
  AtomicTagged_t value = AsAtomicTagged::Acquire_Load(location(host, offset));
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    value = Internals::UnPackMapWord(value);
  return T(tagged_to_full(isolate, value));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Release_Store(HeapObject host, T value) {
  Address ptr = value.ptr();
  if (kFieldOffset == HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  AsAtomicTagged::Release_Store(location(host), full_to_tagged(ptr));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Release_Store_No_Pack(HeapObject host,
                                                         T value) {
  Address ptr = value.ptr();
  AsAtomicTagged::Release_Store(location(host), full_to_tagged(ptr));
}

// static
template <typename T, int kFieldOffset>
void TaggedField<T, kFieldOffset>::Release_Store(HeapObject host, int offset,
                                                 T value) {
  Address ptr = value.ptr();
  if (kFieldOffset + offset ==
      HeapObject::kMapOffset)  // TODO(steveblackburn) perf?
    ptr = Internals::PackMapWord(ptr);
  AsAtomicTagged::Release_Store(location(host, offset), full_to_tagged(ptr));
}

// static
template <typename T, int kFieldOffset>
Tagged_t TaggedField<T, kFieldOffset>::Release_CompareAndSwap(HeapObject host,
                                                              T old, T value) {
  // DCHECK(kFieldOffset != HeapObject::kMapOffset);   // this could be forwarding ptr
  Tagged_t old_value = full_to_tagged(old.ptr());
  Tagged_t new_value = full_to_tagged(value.ptr());
  Tagged_t result = AsAtomicTagged::Release_CompareAndSwap(
      location(host), old_value, new_value);
  return result;
}

}  // namespace internal
}  // namespace v8

#endif  // V8_OBJECTS_TAGGED_FIELD_INL_H_
