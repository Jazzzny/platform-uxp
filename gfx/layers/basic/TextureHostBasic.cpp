/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "TextureHostBasic.h"

#ifdef XP_MACOSX
#include <AvailabilityMacros.h>
#endif

#if defined(XP_MACOSX) && defined(MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
#include "MacIOSurfaceTextureHostBasic.h"
#endif

using namespace mozilla::gl;
using namespace mozilla::gfx;

namespace mozilla {
namespace layers {

already_AddRefed<TextureHost>
CreateTextureHostBasic(const SurfaceDescriptor& aDesc,
                       ISurfaceAllocator* aDeallocator,
                       TextureFlags aFlags)
{
#if defined(XP_MACOSX) && defined(MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
  if (aDesc.type() == SurfaceDescriptor::TSurfaceDescriptorMacIOSurface) {
    const SurfaceDescriptorMacIOSurface& desc =
      aDesc.get_SurfaceDescriptorMacIOSurface();
    return MakeAndAddRef<MacIOSurfaceTextureHostBasic>(aFlags, desc);
  }
#endif
  return CreateBackendIndependentTextureHost(aDesc, aDeallocator, aFlags);
}

} // namespace layers
} // namespace mozilla
