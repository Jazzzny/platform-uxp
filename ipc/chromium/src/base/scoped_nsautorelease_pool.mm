// Copyright (c) 2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/scoped_nsautorelease_pool.h"

#import <Foundation/Foundation.h>

#include "base/logging.h"

namespace base {

ScopedNSAutoreleasePool::ScopedNSAutoreleasePool()
    : autorelease_pool_([[NSAutoreleasePool alloc] init]) {
  DCHECK(autorelease_pool_);
}

ScopedNSAutoreleasePool::~ScopedNSAutoreleasePool() {
#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  [autorelease_pool_ drain];
#else
  [autorelease_pool_ release];
#endif
}

// Cycle the internal pool, allowing everything there to get cleaned up and
// start anew.
void ScopedNSAutoreleasePool::Recycle() {
#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  [autorelease_pool_ drain];
#else
  [autorelease_pool_ release];
#endif
  autorelease_pool_ = [[NSAutoreleasePool alloc] init];
  DCHECK(autorelease_pool_);
}

}  // namespace base
