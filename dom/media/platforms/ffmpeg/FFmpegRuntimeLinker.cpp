/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FFmpegRuntimeLinker.h"
#include "FFmpegLibWrapper.h"
#include "mozilla/ArrayUtils.h"
#include "FFmpegLog.h"
#include "prlink.h"

#ifdef XP_DARWIN
  #include <dlfcn.h>
  #include <libgen.h>
  #include <mach-o/dyld.h>
  #include <AvailabilityMacros.h>
#endif /* XP_DARWIN */

namespace mozilla
{

FFmpegRuntimeLinker::LinkStatus FFmpegRuntimeLinker::sLinkStatus =
  LinkStatus_INIT;
const char* FFmpegRuntimeLinker::sLinkStatusLibraryName = "";

template <int V> class FFmpegDecoderModule
{
public:
  static already_AddRefed<PlatformDecoderModule> Create(FFmpegLibWrapper*);
};

static FFmpegLibWrapper sLibAV;

#ifdef XP_DARWIN
static void*
DlopenFFmpegLibrary(const char* aName, const char* aExecDir)
{
  void* handle = dlopen(aName, RTLD_NOW | RTLD_LOCAL);
  char* fullPath = nullptr;
  if (!handle &&
      asprintf(&fullPath, "%s/%s", aExecDir, aName) > 0 && fullPath) {
    handle = dlopen(fullPath, RTLD_NOW | RTLD_LOCAL);
  }

#if !defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
  if (!handle) {
    handle = dlopen(aName, RTLD_NOW | RTLD_GLOBAL);
  }
  if (!handle && fullPath) {
    handle = dlopen(fullPath, RTLD_NOW | RTLD_GLOBAL);
  }
#endif

  free(fullPath);
  return handle;
}
#endif

static const char* sLibs[] = {
#if defined(XP_DARWIN)
  "libavcodec.61.dylib",
  "libavcodec.60.dylib",
  "libavcodec.59.dylib",
  "libavcodec.58.dylib",
#else
  "libavcodec.so.61",
  "libavcodec.so.60",
  "libavcodec.so.59",
  "libavcodec.so.58",
  "libavcodec-ffmpeg.so.58",
#endif
};

#if defined(XP_DARWIN) && \
    (!defined(MAC_OS_X_VERSION_10_4) || \
     MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4)
static const char* sAVUtilLibs[] = {
  "libavutil.59.dylib",
  "libavutil.58.dylib",
  "libavutil.57.dylib",
  "libavutil.56.dylib",
};
static_assert(ArrayLength(sLibs) == ArrayLength(sAVUtilLibs),
              "libavcodec and libavutil candidates must stay paired");
#endif

/* static */ bool
FFmpegRuntimeLinker::Init()
{
  if (sLinkStatus != LinkStatus_INIT) {
    return sLinkStatus == LinkStatus_SUCCEEDED;
  }

  // While going through all possible libs, this status will be updated with a
  // more precise error if possible.
  sLinkStatus = LinkStatus_NOT_FOUND;

  #ifdef XP_DARWIN // Explanation below.
  char execPath[PATH_MAX];
  execPath[0] = '\0';
  uint32_t pathlen = PATH_MAX;
  _NSGetExecutablePath(execPath, &pathlen);
  char *execDir = dirname(execPath);
  #endif /* XP_DARWIN */

  for (size_t i = 0; i < ArrayLength(sLibs); i++) {
    const char* lib = sLibs[i];
#ifdef XP_DARWIN
    /* Loading FFMPEG on Mac OS X (macOS is a typo) fails because mozilla
      searches for symbols defined in libavutil with a handle to libavcodec.
      This is due to the fact that NSPR uses NSAddressOfSymbol & cie who limits
      its researches only to libavcodec and not its dependencies.  We don't have
      this issue with dlsym().  */
    sLibAV.mAVCodecLib = DlopenFFmpegLibrary(lib, execDir);
    if (sLibAV.mAVCodecLib) {
#if !defined(MAC_OS_X_VERSION_10_4) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
      // Panther's dlcompat does not search a handle's dependencies in dlsym().
      // Keep an explicit libavutil handle for all symbols owned by that library.
      sLibAV.mAVUtilLib = DlopenFFmpegLibrary(sAVUtilLibs[i], execDir);
      if (!sLibAV.mAVUtilLib) {
        FFMPEG_LOG("%s loaded, but its paired %s did not",
                   lib, sAVUtilLibs[i]);
        sLibAV.Unlink();
      }
#else
      sLibAV.mAVUtilLib = sLibAV.mAVCodecLib;
#endif
    }
#else
    PRLibSpec lspec;
    lspec.type = PR_LibSpec_Pathname;
    lspec.value.pathname = lib;
    sLibAV.mAVCodecLib = PR_LoadLibraryWithFlags(lspec, PR_LD_NOW | PR_LD_LOCAL);
#endif /* XP_DARWIN */
    if (sLibAV.mAVCodecLib) {
#ifndef XP_DARWIN
      sLibAV.mAVUtilLib = sLibAV.mAVCodecLib;
#endif
      switch (sLibAV.Link()) {
        case FFmpegLibWrapper::LinkResult::Success:
          sLinkStatus = LinkStatus_SUCCEEDED;
          sLinkStatusLibraryName = lib;
          return true;
        case FFmpegLibWrapper::LinkResult::NoProvidedLib:
          MOZ_ASSERT_UNREACHABLE("Incorrectly-setup sLibAV");
          break;
        case FFmpegLibWrapper::LinkResult::NoAVCodecVersion:
          if (sLinkStatus > LinkStatus_INVALID_CANDIDATE) {
            sLinkStatus = LinkStatus_INVALID_CANDIDATE;
            sLinkStatusLibraryName = lib;
          }
          break;
        case FFmpegLibWrapper::LinkResult::UnknownFFMpegVersion:
        case FFmpegLibWrapper::LinkResult::MissingFFMpegFunction:
          if (sLinkStatus > LinkStatus_INVALID_FFMPEG_CANDIDATE) {
            sLinkStatus = LinkStatus_INVALID_FFMPEG_CANDIDATE;
            sLinkStatusLibraryName = lib;
          }
          break;
      }
    }
  }

  FFMPEG_LOG("H264/AAC codecs unsupported without [");
  for (size_t i = 0; i < ArrayLength(sLibs); i++) {
    FFMPEG_LOG("%s %s", i ? "," : " ", sLibs[i]);
  }
  FFMPEG_LOG(" ]\n");

  return false;
}

/* static */ already_AddRefed<PlatformDecoderModule>
FFmpegRuntimeLinker::CreateDecoderModule()
{
  if (!Init()) {
    return nullptr;
  }
  RefPtr<PlatformDecoderModule> module;
  switch (sLibAV.mVersion) {
    case 58: module = FFmpegDecoderModule<58>::Create(&sLibAV); break;
    case 59: module = FFmpegDecoderModule<59>::Create(&sLibAV); break;
    case 60: module = FFmpegDecoderModule<60>::Create(&sLibAV); break;
    case 61: module = FFmpegDecoderModule<61>::Create(&sLibAV); break;
    default: module = nullptr;
  }
  return module.forget();
}

/* static */ const char*
FFmpegRuntimeLinker::LinkStatusString()
{
  switch (sLinkStatus) {
    case LinkStatus_INIT:
      return "Libavcodec not initialized yet";
    case LinkStatus_SUCCEEDED:
      return "Libavcodec linking succeeded";
    case LinkStatus_INVALID_FFMPEG_CANDIDATE:
      return "Invalid FFMpeg libavcodec candidate";
    case LinkStatus_INVALID_CANDIDATE:
      return "Invalid libavcodec candidate";
    case LinkStatus_NOT_FOUND:
      return "Libavcodec not found";
  }
  MOZ_ASSERT_UNREACHABLE("Unknown sLinkStatus value");
  return "?";
}

} // namespace mozilla
