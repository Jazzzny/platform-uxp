/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-*/
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "OSXRunLoopSingleton.h"
#include <mozilla/StaticMutex.h>

#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>

static bool gRunLoopSet = false;
static mozilla::StaticMutex gMutex;

void mozilla_set_coreaudio_notification_runloop_if_needed()
{
  mozilla::StaticMutexAutoLock lock(gMutex);
  if (gRunLoopSet) {
    return;
  }

  /* This is needed so that AudioUnit listeners get called on this thread, and
   * not the main thread. If we don't do that, they are not called, or a crash
   * occur, depending on the OSX version. */
  CFRunLoopRef run_loop = nullptr;

  OSStatus r;
#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  AudioObjectPropertyAddress runloop_address = {
    kAudioHardwarePropertyRunLoop,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
  };

  r = AudioObjectSetPropertyData(kAudioObjectSystemObject,
                                 &runloop_address,
                                 0, NULL, sizeof(CFRunLoopRef), &run_loop);
#else
  r = AudioHardwareSetProperty(kAudioHardwarePropertyRunLoop,
                               sizeof(CFRunLoopRef), &run_loop);
#endif
  if (r != noErr) {
    NS_WARNING("Could not make global CoreAudio notifications use their own thread.");
  }

  gRunLoopSet = true;
}
