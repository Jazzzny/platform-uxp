/*
 *  Copyright 2008 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import "webrtc/base/scoped_autorelease_pool.h"

namespace rtc {

ScopedAutoreleasePool::ScopedAutoreleasePool() {
  pool_ = [[NSAutoreleasePool alloc] init];
}

ScopedAutoreleasePool::~ScopedAutoreleasePool() {
#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  [pool_ drain];
#else
  [pool_ release];
#endif
}

}  // namespace rtc
