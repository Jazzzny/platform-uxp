/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsString.h"
#include "MacHelpers.h"
#include "nsObjCExceptions.h"

#import <Foundation/Foundation.h>

namespace mozilla {

nsresult
GetSelectedCityInfo(nsAString& aCountryCode)
{
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  // Can be replaced with [[NSLocale currentLocale] countryCode] once we build
  // with the 10.12 SDK.
#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  id countryCode = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
#else
  NSString* appleLocale =
    [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleLocale"];
  NSRange separator = [appleLocale rangeOfString:@"_"];
  id countryCode = separator.location == NSNotFound ? nil :
    [appleLocale substringFromIndex:separator.location + 1];
  separator = [countryCode rangeOfString:@"."];
  if (separator.location != NSNotFound) {
    countryCode = [countryCode substringToIndex:separator.location];
  }
#endif

  if (![countryCode isKindOfClass:[NSString class]]) {
    return NS_ERROR_FAILURE;
  }

  const char* countryCodeUTF8 = [(NSString*)countryCode UTF8String];

  if (!countryCodeUTF8) {
    return NS_ERROR_FAILURE;
  }

  AppendUTF8toUTF16(countryCodeUTF8, aCountryCode);
  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

} // namespace Mozilla
