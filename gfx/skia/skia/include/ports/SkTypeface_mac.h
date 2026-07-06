/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkTypeface_mac_DEFINED
#define SkTypeface_mac_DEFINED

#include "SkTypeface.h"

#if defined(SK_BUILD_FOR_MAC) || defined(SK_BUILD_FOR_IOS)

#include <CoreFoundation/CoreFoundation.h>

#ifdef SK_BUILD_FOR_MAC
#import <ApplicationServices/ApplicationServices.h>
#if !defined(MAC_OS_X_VERSION_10_5) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5)
#include <float.h>
#include <limits.h>
#ifndef CGFLOAT_DEFINED
typedef float CGFloat;
#define CGFLOAT_MIN FLT_MIN
#define CGFLOAT_MAX FLT_MAX
#define CGFLOAT_IS_DOUBLE 0
#define CGFLOAT_DEFINED 1
#endif
#ifndef NSINTEGER_DEFINED
typedef int NSInteger;
typedef unsigned int NSUInteger;
#define NSIntegerMax    LONG_MAX
#define NSIntegerMin    LONG_MIN
#define NSUIntegerMax   ULONG_MAX
#define NSINTEGER_DEFINED 1
#endif
#include "../../../../thebes/PhonyCoreText.h"
#endif
#endif

#ifdef SK_BUILD_FOR_IOS
#include <CoreText/CoreText.h>
#endif

/**
 *  Like the other Typeface create methods, this returns a new reference to the
 *  corresponding typeface for the specified CTFontRef. The caller must call
 *  unref() when it is finished.
 *
 *  The CFTypeRef parameter, if provided, will be kept referenced for the
 *  lifetime of the SkTypeface. This was introduced as a means to work around
 *  https://crbug.com/413332 .
 */
SK_API extern SkTypeface* SkCreateTypefaceFromCTFont(CTFontRef, CFTypeRef = NULL);

/**
 *  Returns the platform-specific CTFontRef handle for a
 *  given SkTypeface. Note that the returned CTFontRef gets
 *  released when the source SkTypeface is destroyed.
 *
 *  This method is deprecated. It may only be used by Blink Mac
 *  legacy code in special cases related to text-shaping
 *  with AAT fonts, clipboard handling and font fallback.
 *  See https://code.google.com/p/skia/issues/detail?id=3408
 */
SK_API extern CTFontRef SkTypeface_GetCTFontRef(const SkTypeface* face);

#endif  // defined(SK_BUILD_FOR_MAC) || defined(SK_BUILD_FOR_IOS)
#endif  // SkTypeface_mac_DEFINED
