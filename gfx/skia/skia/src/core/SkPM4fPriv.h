/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkPM4fPriv_DEFINED
#define SkPM4fPriv_DEFINED

#include "SkColorPriv.h"
#include "SkPM4f.h"
#include "SkSRGB.h"

static inline Sk4f set_alpha(const Sk4f& px, float alpha) {
    return { px[0], px[1], px[2], alpha };
}

static inline float get_alpha(const Sk4f& px) {
    return px[3];
}


static inline Sk4f Sk4f_fromL32(uint32_t px) {
#ifdef SK_PMCOLOR_IS_ARGB
    return Sk4f(SkGetPackedR32(px), SkGetPackedG32(px),
                SkGetPackedB32(px), SkGetPackedA32(px)) * (1/255.0f);
#else
    return SkNx_cast<float>(Sk4b::Load(&px)) * (1/255.0f);
#endif
}

static inline Sk4f Sk4f_fromS32(uint32_t px) {
#ifdef SK_PMCOLOR_IS_ARGB
    return { sk_linear_from_srgb[SkGetPackedR32(px)],
             sk_linear_from_srgb[SkGetPackedG32(px)],
             sk_linear_from_srgb[SkGetPackedB32(px)],
                    (1/255.0f) * SkGetPackedA32(px) };
#else
    return { sk_linear_from_srgb[(px >>  0) & 0xff],
             sk_linear_from_srgb[(px >>  8) & 0xff],
             sk_linear_from_srgb[(px >> 16) & 0xff],
                    (1/255.0f) * (px >> 24)          };
#endif
}

static inline uint32_t Sk4f_toL32(const Sk4f& px) {
#ifdef SK_PMCOLOR_IS_ARGB
    Sk4b bytes = SkNx_cast<uint8_t>(Sk4f_round(px * 255.0f));
    return SkPackARGB_as_PMColor(bytes[3], bytes[0], bytes[1], bytes[2]);
#else
    uint32_t l32;
    SkNx_cast<uint8_t>(Sk4f_round(px * 255.0f)).store(&l32);
    return l32;
#endif
}

static inline uint32_t Sk4f_toS32(const Sk4f& px) {
    Sk4i  rgb = sk_linear_to_srgb(px),
         srgb = { rgb[0], rgb[1], rgb[2], (int)(255.0f * px[3] + 0.5f) };

#ifdef SK_PMCOLOR_IS_ARGB
    return SkPackARGB_as_PMColor(srgb[3], srgb[0], srgb[1], srgb[2]);
#else
    uint32_t s32;
    SkNx_cast<uint8_t>(srgb).store(&s32);
    return s32;
#endif
}


// SkColor handling:
//   SkColor has an ordering of (b, g, r, a) if cast to an Sk4f, so the code swizzles r and b to
// produce the needed (r, g, b, a) ordering.
static inline Sk4f Sk4f_from_SkColor(SkColor color) {
    return swizzle_rb(Sk4f_fromS32(color));
}

static inline void assert_unit(float x) {
    SkASSERT(0 <= x && x <= 1);
}

static inline float exact_srgb_to_linear(float srgb) {
    assert_unit(srgb);
    float linear;
    if (srgb <= 0.04045) {
        linear = srgb / 12.92f;
    } else {
        linear = powf((srgb + 0.055f) / 1.055f, 2.4f);
    }
    assert_unit(linear);
    return linear;
}

#endif
