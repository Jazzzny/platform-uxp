/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsPrintSettingsX.h"
#include "nsObjCExceptions.h"

#include "plbase64.h"
#include "plstr.h"

#include "nsCocoaUtils.h"

#include "mozilla/Preferences.h"

using namespace mozilla;

#define MAC_OS_X_PAGE_SETUP_PREFNAME    "print.macosx.pagesetup-2"
#define COCOA_PAPER_UNITS_PER_INCH      72.0

NS_IMPL_ISUPPORTS_INHERITED(nsPrintSettingsX, nsPrintSettings, nsPrintSettingsX)

nsPrintSettingsX::nsPrintSettingsX()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK;

  mPrintInfo = [[NSPrintInfo sharedPrintInfo] copy];
  mWidthScale = COCOA_PAPER_UNITS_PER_INCH;
  mHeightScale = COCOA_PAPER_UNITS_PER_INCH;

  NS_OBJC_END_TRY_ABORT_BLOCK;
}

nsPrintSettingsX::nsPrintSettingsX(const nsPrintSettingsX& src)
{
  *this = src;
}

nsPrintSettingsX::~nsPrintSettingsX()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK;

  [mPrintInfo release];

  NS_OBJC_END_TRY_ABORT_BLOCK;
}

nsPrintSettingsX& nsPrintSettingsX::operator=(const nsPrintSettingsX& rhs)
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_RETURN;

  if (this == &rhs) {
    return *this;
  }

  nsPrintSettings::operator=(rhs);

  [mPrintInfo release];
  mPrintInfo = [rhs.mPrintInfo copy];

  return *this;

  NS_OBJC_END_TRY_ABORT_BLOCK_RETURN(*this);
}

nsresult nsPrintSettingsX::Init()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  InitUnwriteableMargin();
  InitAdjustedPaperSize();

  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

// Should be called whenever the page format changes.
NS_IMETHODIMP nsPrintSettingsX::InitUnwriteableMargin()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  PMPaper paper;
  PMPaperMargins paperMargin;
  PMPageFormat pageFormat = GetPMPageFormat();
  ::PMGetPageFormatPaper(pageFormat, &paper);
  ::PMPaperGetMargins(paper, &paperMargin);
  mUnwriteableMargin.top    = NS_POINTS_TO_INT_TWIPS(paperMargin.top);
  mUnwriteableMargin.left   = NS_POINTS_TO_INT_TWIPS(paperMargin.left);
  mUnwriteableMargin.bottom = NS_POINTS_TO_INT_TWIPS(paperMargin.bottom);
  mUnwriteableMargin.right  = NS_POINTS_TO_INT_TWIPS(paperMargin.right);

  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

NS_IMETHODIMP nsPrintSettingsX::InitAdjustedPaperSize()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  PMPageFormat pageFormat = GetPMPageFormat();

  PMRect paperRect;
  ::PMGetAdjustedPaperRect(pageFormat, &paperRect);

  mAdjustedPaperWidth = paperRect.right - paperRect.left;
  mAdjustedPaperHeight = paperRect.bottom - paperRect.top;

  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

void
nsPrintSettingsX::SetCocoaPrintInfo(NSPrintInfo* aPrintInfo)
{
  if (mPrintInfo != aPrintInfo) {
    [mPrintInfo release];
    mPrintInfo = [aPrintInfo retain];
  }
}

NS_IMETHODIMP nsPrintSettingsX::ReadPageFormatFromPrefs()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  nsAutoCString encodedData;
  nsresult rv =
    Preferences::GetCString(MAC_OS_X_PAGE_SETUP_PREFNAME, &encodedData);
  if (NS_FAILED(rv)) {
    return rv;
  }

  // decode the base64
  char* decodedData = PL_Base64Decode(encodedData.get(), encodedData.Length(), nullptr);
  NSData* data = [NSData dataWithBytes:decodedData length:strlen(decodedData)];
  if (!data)
    return NS_ERROR_FAILURE;

  PMPageFormat newPageFormat;
#if defined(MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
  OSStatus status = ::PMPageFormatCreateWithDataRepresentation((CFDataRef)data, &newPageFormat);
#elif defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  OSStatus status = ::PMUnflattenPageFormatWithCFData((CFDataRef)data, &newPageFormat);
#else
  Handle pageFormatHandle = nullptr;
  OSStatus status = ::PtrToHand([data bytes], &pageFormatHandle, [data length]);
  if (status == noErr) {
    status = ::PMUnflattenPageFormat(pageFormatHandle, &newPageFormat);
  }
  if (pageFormatHandle) {
    ::DisposeHandle(pageFormatHandle);
  }
#endif
  if (status == noErr) {
    SetPMPageFormat(newPageFormat);
  }
  InitUnwriteableMargin();

  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

NS_IMETHODIMP nsPrintSettingsX::WritePageFormatToPrefs()
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  PMPageFormat pageFormat = GetPMPageFormat();
  if (pageFormat == kPMNoPageFormat)
    return NS_ERROR_NOT_INITIALIZED;

  NSData* data = nil;
#if defined(MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
  OSStatus err = ::PMPageFormatCreateDataRepresentation(pageFormat, (CFDataRef*)&data, kPMDataFormatXMLDefault);
#elif defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  OSStatus err = ::PMFlattenPageFormatToCFData(pageFormat, (CFDataRef*)&data);
#else
  Handle pageFormatHandle = nullptr;
  OSStatus err = ::PMFlattenPageFormat(pageFormat, &pageFormatHandle);
  if (err == noErr && pageFormatHandle) {
    ::HLock(pageFormatHandle);
    data = [NSData dataWithBytes:*pageFormatHandle
                           length:GetHandleSize(pageFormatHandle)];
    ::HUnlock(pageFormatHandle);
  }
  if (pageFormatHandle) {
    ::DisposeHandle(pageFormatHandle);
  }
#endif
  if (err != noErr)
    return NS_ERROR_FAILURE;

  nsAutoCString encodedData;
  encodedData.Adopt(PL_Base64Encode((char*)[data bytes], [data length], nullptr));
  if (!encodedData.get())
    return NS_ERROR_OUT_OF_MEMORY;

  return Preferences::SetCString(MAC_OS_X_PAGE_SETUP_PREFNAME, encodedData);

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

nsresult nsPrintSettingsX::_Clone(nsIPrintSettings **_retval)
{
  NS_ENSURE_ARG_POINTER(_retval);
  *_retval = nullptr;

  nsPrintSettingsX *newSettings = new nsPrintSettingsX(*this);
  if (!newSettings)
    return NS_ERROR_FAILURE;
  *_retval = newSettings;
  NS_ADDREF(*_retval);
  return NS_OK;
}

NS_IMETHODIMP nsPrintSettingsX::_Assign(nsIPrintSettings *aPS)
{
  nsPrintSettingsX *printSettingsX = static_cast<nsPrintSettingsX*>(aPS);
  if (!printSettingsX)
    return NS_ERROR_UNEXPECTED;
  *this = *printSettingsX;
  return NS_OK;
}

PMPrintSettings
nsPrintSettingsX::GetPMPrintSettings()
{
  if ([mPrintInfo respondsToSelector:@selector(PMPrintSettings)])
    return (PMPrintSettings)[mPrintInfo PMPrintSettings]; // 10.5+

  if ([mPrintInfo respondsToSelector:@selector(pmPrintSettings)])
    return (PMPrintSettings)[mPrintInfo pmPrintSettings]; // 10.4

  NS_ASSERTION(PR_FALSE, "no way of getting PMPrintSettings from NSPrintInfo");
  PMPrintSettings printSettings;
  PMCreatePrintSettings(&printSettings);
  return printSettings;
}

PMPrintSession
nsPrintSettingsX::GetPMPrintSession()
{
  if ([mPrintInfo respondsToSelector:@selector(PMPrintSession)])
    return (PMPrintSession)[mPrintInfo PMPrintSession]; // 10.5+

  if ([mPrintInfo respondsToSelector:@selector(_pmPrintSession)])
    return (PMPrintSession)[mPrintInfo _pmPrintSession]; // 10.4

  NS_ASSERTION(PR_FALSE, "no way of getting PMPrintSession from NSPrintInfo");
  PMPrintSession printSession;
  PMCreateSession(&printSession);
  return printSession;
}

PMPageFormat
nsPrintSettingsX::GetPMPageFormat()
{
  if ([mPrintInfo respondsToSelector:@selector(PMPageFormat)])
    return (PMPageFormat)[mPrintInfo PMPageFormat]; // 10.5+

  if ([mPrintInfo respondsToSelector:@selector(pmPageFormat)])
    return (PMPageFormat)[mPrintInfo pmPageFormat]; // 10.4

  NS_ASSERTION(PR_FALSE, "no way of getting PMPageFormat from NSPrintInfo");
  PMPageFormat pageFormat;
  PMCreatePageFormat(&pageFormat);
  return pageFormat;
}

void
nsPrintSettingsX::SetPMPageFormat(PMPageFormat aPageFormat)
{
  PMPageFormat oldPageFormat = GetPMPageFormat();
  ::PMCopyPageFormat(aPageFormat, oldPageFormat);
  [mPrintInfo updateFromPMPageFormat];
}

void
nsPrintSettingsX::SetInchesScale(float aWidthScale, float aHeightScale)
{
  if (aWidthScale > 0 && aHeightScale > 0) {
    mWidthScale = aWidthScale;
    mHeightScale = aHeightScale;
  }
}

void
nsPrintSettingsX::GetInchesScale(float *aWidthScale, float *aHeightScale)
{
  *aWidthScale = mWidthScale;
  *aHeightScale = mHeightScale;
}

NS_IMETHODIMP nsPrintSettingsX::SetPaperWidth(double aPaperWidth)
{
  mPaperWidth = aPaperWidth;
  mAdjustedPaperWidth = aPaperWidth * mWidthScale;
  return NS_OK;
}

NS_IMETHODIMP nsPrintSettingsX::SetPaperHeight(double aPaperHeight)
{
  mPaperHeight = aPaperHeight;
  mAdjustedPaperHeight = aPaperHeight * mHeightScale;
  return NS_OK;
}

NS_IMETHODIMP
nsPrintSettingsX::GetEffectivePageSize(double *aWidth, double *aHeight)
{
  *aWidth  = NS_INCHES_TO_TWIPS(mAdjustedPaperWidth / mWidthScale);
  *aHeight = NS_INCHES_TO_TWIPS(mAdjustedPaperHeight / mHeightScale);
  return NS_OK;
}

void nsPrintSettingsX::SetAdjustedPaperSize(double aWidth, double aHeight)
{
  mAdjustedPaperWidth = aWidth;
  mAdjustedPaperHeight = aHeight;
}

void nsPrintSettingsX::GetAdjustedPaperSize(double *aWidth, double *aHeight)
{
  *aWidth = mAdjustedPaperWidth;
  *aHeight = mAdjustedPaperHeight;
}
