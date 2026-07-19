/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_GFX_SCALEDFONTMAC_H_
#define MOZILLA_GFX_SCALEDFONTMAC_H_

#ifdef MOZ_WIDGET_COCOA
#include <ApplicationServices/ApplicationServices.h>
#else
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#endif

#include <AvailabilityMacros.h>

#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)
#define MOZ_MAC_USE_PUBLIC_CORETEXT 1
#else
#define MOZ_MAC_USE_PUBLIC_CORETEXT 0
#endif

#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4) && !MOZ_MAC_USE_PUBLIC_CORETEXT
#define MOZ_MAC_USE_PHONY_CORETEXT 1
#else
#define MOZ_MAC_USE_PHONY_CORETEXT 0
#endif

#if MOZ_MAC_USE_PUBLIC_CORETEXT || MOZ_MAC_USE_PHONY_CORETEXT
#define MOZ_MAC_USE_CORETEXT 1
#else
#define MOZ_MAC_USE_CORETEXT 0
#endif

#if MOZ_MAC_USE_PHONY_CORETEXT
// For 10.4
typedef unsigned int PRUint32;
typedef int PRInt32;
#include "../thebes/PhonyCoreText.h"
#endif

#include "2D.h"

#include "ScaledFontBase.h"

namespace mozilla {
namespace gfx {

class ScaledFontMac : public ScaledFontBase
{
public:
  MOZ_DECLARE_REFCOUNTED_VIRTUAL_TYPENAME(ScaledFontMac)
#if MOZ_MAC_USE_CORETEXT
  ScaledFontMac(CGFontRef aFont, Float aSize);
#else
  ScaledFontMac(CGFontRef aFont, Float aSize, ATSFontRef aATSFont = kInvalidFont);
#endif
  virtual ~ScaledFontMac();

  virtual FontType GetType() const { return FontType::MAC; }
#ifdef USE_SKIA
  virtual SkTypeface* GetSkTypeface();
#endif
  virtual already_AddRefed<Path> GetPathForGlyphs(const GlyphBuffer &aBuffer, const DrawTarget *aTarget);
#ifndef USE_SKIA
  virtual void CopyGlyphsToBuilder(const GlyphBuffer &aBuffer, PathBuilder *aBuilder, const Matrix *aTransformHint);
#endif
  virtual bool GetFontFileData(FontFileDataOutput aDataCallback, void *aBaton);

#ifdef USE_CAIRO_SCALED_FONT
  cairo_font_face_t* GetCairoFontFace();
#endif

private:
  friend class DrawTargetCG;
  friend class DrawTargetSkia;
  CGFontRef mFont;
#if MOZ_MAC_USE_CORETEXT
  CTFontRef mCTFont; // only created if CTFontDrawGlyphs is available, otherwise null

  typedef void (CTFontDrawGlyphsFuncT)(CTFontRef,
                                       const CGGlyph[], const CGPoint[],
                                       size_t, CGContextRef);

  static bool sSymbolLookupDone;

public:
  // function pointer for CTFontDrawGlyphs, if available;
  // initialized the first time a ScaledFontMac is created,
  // so it will be valid by the time DrawTargetCG wants to use it
  static CTFontDrawGlyphsFuncT* CTFontDrawGlyphsPtr;
#else
  ATSFontRef mATSFont;
#endif
};

} // namespace gfx
} // namespace mozilla

#endif /* MOZILLA_GFX_SCALEDFONTMAC_H_ */
