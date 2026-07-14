
/* This is where the awesome is -- Apple's Tiger Core Text spread bare for*
   the world, like the hottest nerd centerfold ever. This also includes
   necessarily some of the private Core Graphics methods also in 10.4. */

/* Dear Apple, in case you are reading:

   - I intentionally did NOT look at the API headers from Leopard while
     working on this so that I would not be affected by them. This permits
     this to be distributed Tri-License MPL/GPL/LGPL honestly, because it
     is all my own hard, slavish work, and not an Apple proprietary file.
   - This only includes what I need to get this version of Mozilla to build.
     There are lots of other symbols defined that I don't need here.
   - Symbols were dumped initially with nm, and then otool to reconstruct
     the actual calling conventions and structs. Where I could just derive
     them from Mozilla code, they are just that (there were some gimmes), and
     therefore use NSPR types instead of C types.
   - Don't sue me. I love you. I want to have Steve's babies (if I were
     physically capable). I didn't steal your code. Did I mention Stevespawn?
   - Well, okay, maybe not his *babies*. How about cleaning out their old
     machines closet? I can give that old TAM a home. Get it? That was a
     really cool anime joke. Please don't sue people who can make really cool
     anime jokes.

Cameron Kaiser */

#ifndef __PHONYCORETEXT_H
#define __PHONYCORETEXT_H

#ifdef __cplusplus
#define __CKEXTERN extern "C"
#define __CKEXTERNBEGIN extern "C" {
#define __CKEXTERNEND }
#else
#define __CKEXTERN extern
#define __CKEXTERNBEGIN
#define __CKEXTERNEND
#endif

#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFCharacterSet.h>
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFSet.h>
#include <CoreServices/CoreServices.h>
#include <dlfcn.h>

__CKEXTERNBEGIN

/* Hidden bits and private methods in CoreGraphics that we need */
/* Unfortunately, we still have to use ATSUI workarounds for tables bleh */

/* typedef float CGFloat; */ /* uncomment if needed */
__CKEXTERN int CGFontGetUnitsPerEm(CGFontRef font);
__CKEXTERN bool CGFontGetGlyphAdvances(CGFontRef font, const CGGlyph glyphs[], size_t count, int advances[]);
__CKEXTERN bool CGFontGetGlyphBoundingBoxes(CGFontRef font, const CGGlyph glyphs[], size_t count, CGRect bboxes[]);
__CKEXTERN CGPathRef CGFontGetGlyphPath(CGFontRef fontRef, CGAffineTransform *textTransform, int unknown, CGGlyph glyph);
__CKEXTERN CGFontRef CGFontCreateWithDataProvider(CGDataProviderRef provider);

/* CoreText private framework */
/* Approximately in order as they appear in gfxCoreTextShaper */

/* Descriptors */

typedef const struct __CTFontDescriptor *CTFontDescriptorRef; /* opaque */
typedef uint32_t CTFontSymbolicTraits;
typedef uint32_t CTFontStylisticClass;
typedef uint32_t CTFontTableTag;
typedef uint32_t CTFontTableOptions;
typedef int CTFontOrientation;

enum {
  kCTFontDefaultOrientation = 0,
  kCTFontHorizontalOrientation = 0,
  kCTFontVerticalOrientation = 1
};

enum {
  kCTFontItalicTrait = (1 << 0),
  kCTFontBoldTrait = (1 << 1),
  kCTFontMonoSpaceTrait = (1 << 10),
  kCTFontColorGlyphsTrait = (1 << 13),
  kCTFontClassMaskTrait = 0xF0000000,
  kCTFontOldStyleSerifsClass = 0x10000000,
  kCTFontSlabSerifsClass = 0x50000000,
  kCTFontScriptsClass = 0x80000000
};

enum {
  kCTFontTableOptionNoOptions = 0
};

__CKEXTERN const CFStringRef kCTFontFeatureTypeIdentifierKey;
__CKEXTERN const CFStringRef kCTFontFeatureSelectorIdentifierKey;
__CKEXTERN const CFStringRef kCTFontFeatureSettingsAttribute;
__CKEXTERN const CFStringRef kCTFontFamilyNameAttribute;
__CKEXTERN const CFStringRef kCTFontTraitsAttribute;
__CKEXTERN const CFStringRef kCTFontSymbolicTrait;
__CKEXTERN const CFStringRef kCTFontWeightTrait;
__CKEXTERN const CFStringRef kCTFontWidthTrait;
__CKEXTERN const CFStringRef kCTFontSlantTrait;
__CKEXTERN const CFStringRef kCTFontStyleNameAttribute;
__CKEXTERN const CFStringRef kCTFontVariationAttribute;
__CKEXTERN const CFStringRef kCTFontVariationAxisNameKey;
__CKEXTERN const CFStringRef kCTFontVariationAxisIdentifierKey;
__CKEXTERN CTFontDescriptorRef CTFontDescriptorCreateWithAttributes(CFDictionaryRef attribs);
__CKEXTERN CTFontDescriptorRef CTFontDescriptorCreateWithNameAndSize(CFStringRef name, float size);
__CKEXTERN CFTypeRef CTFontDescriptorCopyAttribute(CTFontDescriptorRef descriptor, CFStringRef attribute);
__CKEXTERN CFArrayRef CTFontDescriptorCopyMatchingFontDescriptors(CTFontDescriptorRef descriptor, CFSetRef mandatoryAttributes);
#define CTFontDescriptorCreateMatchingFontDescriptors CTFontDescriptorCopyMatchingFontDescriptors
/* we do not have CTFontDescriptorCreateCopyWithAttributes, but we can't
   do ligatures anyway, see Runs */

/* Fonts */

typedef const struct __CTFont *CTFontRef; /* opaque */
__CKEXTERN CTFontRef CTFontCreateForString(CTFontRef currentFont, CFStringRef string, CFRange range);
__CKEXTERN const CFStringRef kCTFontAttributeName;
__CKEXTERN float CTFontGetSize(CTFontRef font);
__CKEXTERN CFIndex CTFontGetNumberOfGlyphs(CTFontRef font);
#define CTFontGetGlyphCount CTFontGetNumberOfGlyphs
__CKEXTERN CGFontRef CTFontGetGraphicsFont(CTFontRef font);
__CKEXTERN CFStringRef CTFontCopyFamilyName(CTFontRef font);
__CKEXTERN CFStringRef CTFontCopyDisplayName(CTFontRef font);
__CKEXTERN CFStringRef CTFontCopyPostScriptName(CTFontRef font);
__CKEXTERN CTFontDescriptorRef CTFontCopyFontDescriptor(CTFontRef font);
__CKEXTERN CTFontSymbolicTraits CTFontGetSymbolicTraits(CTFontRef font);
__CKEXTERN bool CTFontGetGlyphsForCharacters(CTFontRef font, const UniChar characters[], CGGlyph glyphs[], CFIndex count);
__CKEXTERN double CTFontGetAdvancesForGlyphs(CTFontRef font, CTFontOrientation orientation, const CGGlyph glyphs[], CGSize advances[], CFIndex count);
__CKEXTERN CGRect CTFontGetBoundingRectsForGlyphs(CTFontRef font, CTFontOrientation orientation, const CGGlyph glyphs[], CGRect boundingRects[], CFIndex count);
__CKEXTERN CGRect CTFontGetBoundingBox(CTFontRef font);
__CKEXTERN float CTFontGetAscent(CTFontRef font);
__CKEXTERN float CTFontGetDescent(CTFontRef font);
__CKEXTERN float CTFontGetLeading(CTFontRef font);
__CKEXTERN float CTFontGetXHeight(CTFontRef font);
__CKEXTERN float CTFontGetCapHeight(CTFontRef font);
__CKEXTERN float CTFontGetUnderlinePosition(CTFontRef font);
__CKEXTERN float CTFontGetUnderlineThickness(CTFontRef font);
__CKEXTERN float CTFontGetSlantAngle(CTFontRef font);
__CKEXTERN unsigned CTFontGetUnitsPerEm(CTFontRef font);
__CKEXTERN CFDataRef CTFontCopyTable(CTFontRef font, CTFontTableTag table, CTFontTableOptions options);
__CKEXTERN CFCharacterSetRef CTFontCopyCharacterSet(CTFontRef font);
__CKEXTERN CFArrayRef CTFontCopyVariationAxes(CTFontRef font);

/* Lines */

typedef const struct __CTLine *CTLineRef; /* opaque */
__CKEXTERN CTLineRef CTLineCreateWithAttributedString(CFAttributedStringRef str);
__CKEXTERN CFArrayRef CTLineGetGlyphRuns(CTLineRef line);

/* Runs */

typedef const struct __CTRun *CTRunRef; /* opaque */
__CKEXTERN CFIndex CTRunGetGlyphCount(CTRunRef run);
__CKEXTERN CFRange CTRunGetStringRange(CTRunRef run);
__CKEXTERN const CGGlyph *CTRunGetGlyphsPtr(CTRunRef run);
__CKEXTERN void CTRunGetGlyphs(CTRunRef run, CFRange range, CGGlyph glyphs[]);
/* we do not have CTRunGetPositionsPtr and friends; we cannot reposition.
   I might do an ATSUI workaround for this if I am really, really prodded.
   However, we can duplicate a lot of it with CTRunGetAdvancesPtr etc. */
__CKEXTERN const CGSize *CTRunGetAdvancesPtr(CTRunRef run);
__CKEXTERN void CTRunGetAdvances(CTRunRef run, CFRange range, CGSize advances[]);
__CKEXTERN const CFIndex *CTRunGetStringIndicesPtr(CTRunRef run);
__CKEXTERN void CTRunGetStringIndices(CTRunRef run, CFRange range, CFIndex si[]);
__CKEXTERN double CTRunGetTypographicBounds(CTRunRef run, CFRange range, float *x, float *y, float *z); /* I suspect ascent/descent/leading */

__CKEXTERNEND

/*
 * Tiger's private CoreText uses double for CTFont sizes, but Leopard's public
 * ABI uses CGFloat. On 32-bit Leopard CGFloat is float, so a 10.4-built binary
 * that directly calls the Tiger signature will pass the wrong argument layout
 * and crash inside CoreText. Route size-taking CTFont constructors through
 * dlsym wrappers so the same binary can call the Tiger private ABI on 10.4 and
 * the public CGFloat ABI on 10.5+.
 */

static inline int
PhonyCoreTextUseLeopardABI()
{
  static int sUseLeopardABI = -1;
  if (sUseLeopardABI < 0) {
    SInt32 systemVersion = 0;
    sUseLeopardABI =
      (Gestalt(gestaltSystemVersion, &systemVersion) == noErr &&
       systemVersion >= 0x1050);
  }
  return sUseLeopardABI;
}

static inline void*
PhonyCoreTextLookup(const char* aName, void** aCachedSymbol)
{
  if (!*aCachedSymbol) {
    *aCachedSymbol = dlsym(RTLD_DEFAULT, aName);
  }
  return *aCachedSymbol;
}

static inline CTFontRef
PhonyCTFontCreateWithPlatformFont(ATSFontRef aFont, double aSize,
                                  const CGAffineTransform* aMatrix,
                                  CTFontDescriptorRef aAttributes)
{
  static void* sFunc = NULL;
  void* fn = PhonyCoreTextLookup("CTFontCreateWithPlatformFont", &sFunc);
  if (!fn) {
    return NULL;
  }
  if (PhonyCoreTextUseLeopardABI()) {
    typedef CTFontRef (*LeopardFunc)(ATSFontRef, float,
                                     const CGAffineTransform*,
                                     CTFontDescriptorRef);
    return ((LeopardFunc)fn)(aFont, (float)aSize, aMatrix, aAttributes);
  }
  typedef CTFontRef (*TigerFunc)(ATSFontRef, double,
                                 const CGAffineTransform*,
                                 CTFontDescriptorRef);
  return ((TigerFunc)fn)(aFont, aSize, aMatrix, aAttributes);
}

static inline CTFontRef
PhonyCTFontCreateWithGraphicsFont(CGFontRef aFont, double aSize,
                                  const CGAffineTransform* aMatrix,
                                  CTFontDescriptorRef aAttributes)
{
  static void* sFunc = NULL;
  void* fn = PhonyCoreTextLookup("CTFontCreateWithGraphicsFont", &sFunc);
  if (!fn) {
    return NULL;
  }
  if (PhonyCoreTextUseLeopardABI()) {
    typedef CTFontRef (*LeopardFunc)(CGFontRef, float,
                                     const CGAffineTransform*,
                                     CTFontDescriptorRef);
    return ((LeopardFunc)fn)(aFont, (float)aSize, aMatrix, aAttributes);
  }
  typedef CTFontRef (*TigerFunc)(CGFontRef, double,
                                 const CGAffineTransform*,
                                 CTFontDescriptorRef);
  return ((TigerFunc)fn)(aFont, aSize, aMatrix, aAttributes);
}

static inline CTFontRef
PhonyCTFontCreateWithFontDescriptor(CTFontDescriptorRef aDescriptor,
                                    double aSize,
                                    const CGAffineTransform* aMatrix)
{
  static void* sFunc = NULL;
  void* fn = PhonyCoreTextLookup("CTFontCreateWithFontDescriptor", &sFunc);
  if (!fn) {
    return NULL;
  }
  if (PhonyCoreTextUseLeopardABI()) {
    typedef CTFontRef (*LeopardFunc)(CTFontDescriptorRef, float,
                                     const CGAffineTransform*);
    return ((LeopardFunc)fn)(aDescriptor, (float)aSize, aMatrix);
  }
  typedef CTFontRef (*TigerFunc)(CTFontDescriptorRef, double,
                                 const CGAffineTransform*);
  return ((TigerFunc)fn)(aDescriptor, aSize, aMatrix);
}

static inline CTFontRef
PhonyCTFontCreateWithName(CFStringRef aName, double aSize,
                          const CGAffineTransform* aMatrix)
{
  static void* sFunc = NULL;
  void* fn = PhonyCoreTextLookup("CTFontCreateWithName", &sFunc);
  if (!fn) {
    return NULL;
  }
  if (PhonyCoreTextUseLeopardABI()) {
    typedef CTFontRef (*LeopardFunc)(CFStringRef, float,
                                     const CGAffineTransform*);
    return ((LeopardFunc)fn)(aName, (float)aSize, aMatrix);
  }
  typedef CTFontRef (*TigerFunc)(CFStringRef, double,
                                 const CGAffineTransform*);
  return ((TigerFunc)fn)(aName, aSize, aMatrix);
}

#define CTFontCreateWithPlatformFont PhonyCTFontCreateWithPlatformFont
#define CTFontCreateWithGraphicsFont PhonyCTFontCreateWithGraphicsFont
#define CTFontCreateWithFontDescriptor PhonyCTFontCreateWithFontDescriptor
#define CTFontCreateWithName PhonyCTFontCreateWithName
#endif /* __PHONYCORETEXT_H */
