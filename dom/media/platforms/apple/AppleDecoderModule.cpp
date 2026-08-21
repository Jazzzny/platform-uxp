/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "AppleATDecoder.h"
#include "AppleCMLinker.h"
#include "AppleDecoderModule.h"
#include "AppleVDADecoder.h"
#include "AppleVDALinker.h"
#include "AppleVTDecoder.h"
#include "AppleVTLinker.h"
#include "MacIOSurfaceImage.h"
#include "mozilla/DebugOnly.h"
#include "mozilla/Logging.h"
#include "mozilla/gfx/gfxVars.h"
#include "nsCocoaFeatures.h"

namespace mozilla {

bool AppleDecoderModule::sInitialized = false;
bool AppleDecoderModule::sIsCoreMediaAvailable = false;
bool AppleDecoderModule::sIsVTAvailable = false;
bool AppleDecoderModule::sIsVDAAvailable = false;
bool AppleDecoderModule::sCanUseHardwareVideoDecoder = true;

AppleDecoderModule::AppleDecoderModule()
{
}

AppleDecoderModule::~AppleDecoderModule()
{
}

/* static */
void
AppleDecoderModule::Init()
{
  if (sInitialized) {
    return;
  }

  // Ensure IOSurface framework is loaded.
  MacIOSurfaceLib::LoadLibrary();
  const bool loaded = MacIOSurfaceLib::isInit();

  // dlopen VideoDecodeAcceleration.framework if it's available on <10.9
  if (loaded && !nsCocoaFeatures::OnMavericksOrLater()) {
    sIsVDAAvailable = AppleVDALinker::Link();
  } else {
    sIsVDAAvailable = false;
  }

  // dlopen CoreMedia.framework if it's available.
  sIsCoreMediaAvailable = AppleCMLinker::Link();
  // dlopen VideoToolbox.framework if it's available.
  // We must link both CM and VideoToolbox framework to allow for proper
  // paired Link/Unlink calls
  bool haveVideoToolbox = loaded && AppleVTLinker::Link();
  sIsVTAvailable = sIsCoreMediaAvailable && haveVideoToolbox;

  sCanUseHardwareVideoDecoder = loaded &&
    gfx::gfxVars::CanUseHardwareVideoDecoding();

  sInitialized = true;
}

nsresult
AppleDecoderModule::Startup()
{
  if (!sInitialized) {
    return NS_ERROR_FAILURE;
  }
  if (nsCocoaFeatures::OnMavericksOrLater() ? !sIsVTAvailable
                                            : !sIsVDAAvailable) {
    return NS_ERROR_FAILURE;
  }
  return NS_OK;
}

already_AddRefed<MediaDataDecoder>
AppleDecoderModule::CreateVideoDecoder(const CreateDecoderParams& aParams)
{
  RefPtr<MediaDataDecoder> decoder;

  if (!nsCocoaFeatures::OnMavericksOrLater()) {
    if (!sIsVDAAvailable) {
      return nullptr;
    }
    decoder =
      AppleVDADecoder::CreateVDADecoder(aParams.VideoConfig(),
                                        aParams.mTaskQueue,
                                        aParams.mCallback,
                                        aParams.mImageContainer);
    if (decoder) {
      return decoder.forget();
    }
    // VideoToolbox is software-only on these systems and performs worse than
    // ffvpx. Returning nullptr lets PDMFactory try its next decoder module.
    return nullptr;
  }

  if (sIsVTAvailable) {
    decoder =
      new AppleVTDecoder(aParams.VideoConfig(),
                         aParams.mTaskQueue,
                         aParams.mCallback,
                         aParams.mImageContainer);
  }
  return decoder.forget();
}

already_AddRefed<MediaDataDecoder>
AppleDecoderModule::CreateAudioDecoder(const CreateDecoderParams& aParams)
{
  RefPtr<MediaDataDecoder> decoder =
    new AppleATDecoder(aParams.AudioConfig(),
                       aParams.mTaskQueue,
                       aParams.mCallback);
  return decoder.forget();
}

bool
AppleDecoderModule::SupportsMimeType(const nsACString& aMimeType,
                                     DecoderDoctorDiagnostics* aDiagnostics) const
{
  const bool supportsVideo = nsCocoaFeatures::OnMavericksOrLater()
                               ? sIsVTAvailable
                               : sIsVDAAvailable;
  return (sIsCoreMediaAvailable &&
          (aMimeType.EqualsLiteral("audio/mpeg") ||
           aMimeType.EqualsLiteral("audio/mp4a-latm"))) ||
    (supportsVideo &&
     (aMimeType.EqualsLiteral("video/mp4") ||
      aMimeType.EqualsLiteral("video/avc")));
}

PlatformDecoderModule::ConversionRequired
AppleDecoderModule::DecoderNeedsConversion(const TrackInfo& aConfig) const
{
  if (aConfig.IsVideo()) {
    return ConversionRequired::kNeedAVCC;
  } else {
    return ConversionRequired::kNeedNone;
  }
}

} // namespace mozilla
