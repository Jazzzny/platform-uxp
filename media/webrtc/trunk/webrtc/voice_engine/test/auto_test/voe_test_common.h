/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VOICE_ENGINE_VOE_TEST_COMMON_H_
#define WEBRTC_VOICE_ENGINE_VOE_TEST_COMMON_H_

#define TEST_LOG printf
#define TEST_LOG_ERROR printf
#define TEST_LOG_FLUSH fflush(NULL)

// Read WEBRTC_VOICE_ENGINE_XXX_API compiler flags
#include "webrtc/engine_configurations.h"

// Time in ms to test each packet size for each codec
#define CODEC_TEST_TIME 400

#endif  // WEBRTC_VOICE_ENGINE_VOE_TEST_COMMON_H_
