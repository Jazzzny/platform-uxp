/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "SingleRwFifo.h"

#include <assert.h>

#if defined(_MSC_VER)
#include <windows.h>
#endif

static int
UpdatePos(int pos, int capacity)
{
  return (pos + 1) % capacity;
}

namespace webrtc {

namespace subtle {

#if defined(_MSC_VER)
inline void
MemoryBarrier()
{
  ::MemoryBarrier();
}
#elif defined(__GNUC__) || defined(__clang__)
inline void
MemoryBarrier()
{
  __sync_synchronize();
}
#else
#error Add an implementation of MemoryBarrier() for this platform!
#endif

}  // namespace subtle

SingleRwFifo::SingleRwFifo(int capacity)
  : capacity_(capacity)
  , size_(0)
  , read_pos_(0)
  , write_pos_(0)
{
  queue_.reset(new int8_t*[capacity_]);
}

SingleRwFifo::~SingleRwFifo()
{
}

void
SingleRwFifo::Push(int8_t* mem)
{
  assert(mem);

  // Ensure that there is space for the new data in the FIFO. There is only one
  // writer, so the other thread is guaranteed only to decrease the size.
  const int free_slots = capacity() - size();
  if (free_slots <= 0) {
    // Size can be queried outside of Push. The caller is expected to ensure
    // that Push will be successful before calling it.
    assert(false);
    return;
  }
  queue_[write_pos_] = mem;
  // Ensure that |size_| is updated after the stored pointer is visible.
  subtle::MemoryBarrier();
  ++size_;
  write_pos_ = UpdatePos(write_pos_, capacity());
}

int8_t*
SingleRwFifo::Pop()
{
  int8_t* ret_val = nullptr;
  if (size() <= 0) {
    // Size can be queried outside of Pop. The caller is expected to ensure
    // that Pop will be successful before calling it.
    assert(false);
    return ret_val;
  }
  ret_val = queue_[read_pos_];
  // Ensure that |size_| is updated after the stored pointer is read.
  subtle::MemoryBarrier();
  --size_;
  read_pos_ = UpdatePos(read_pos_, capacity());
  return ret_val;
}

}  // namespace webrtc
