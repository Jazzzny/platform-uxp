/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "updaterfileutils_osx.h"

#include <Cocoa/Cocoa.h>

#if !defined(MAC_OS_X_VERSION_10_5) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
typedef int NSInteger;
typedef unsigned int NSUInteger;

#define NSIntegerMax    LONG_MAX
#define NSIntegerMin    LONG_MIN
#define NSUIntegerMax   ULONG_MAX

#define NSINTEGER_DEFINED 1
#endif

#if defined(MAC_OS_X_VERSION_10_4) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
#define RELEASE_AUTORELEASE_POOL(pool) [(pool) drain]
#else
#define RELEASE_AUTORELEASE_POOL(pool) [(pool) release]
#endif

bool IsRecursivelyWritable(const char* aPath)
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSString* rootPath = [NSString stringWithUTF8String:aPath];
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSError* error = nil;
  NSArray* subPaths =
    [fileManager subpathsOfDirectoryAtPath:rootPath
                                     error:&error];
  NSMutableArray* paths =
    [NSMutableArray arrayWithCapacity:[subPaths count] + 1];
  [paths addObject:@""];
  [paths addObjectsFromArray:subPaths];

  if (error) {
    RELEASE_AUTORELEASE_POOL(pool);
    return false;
  }

  for (NSUInteger i=0; i < [paths count]; i++) {
    NSString* currPath = (NSString*)[paths objectAtIndex:i];
    NSString* child = [rootPath stringByAppendingPathComponent:currPath];

    NSDictionary* attributes =
      [fileManager attributesOfItemAtPath:child
                                    error:&error];
    if (error) {
      RELEASE_AUTORELEASE_POOL(pool);
      return false;
    }

    // Don't check for writability of files pointed to by symlinks, as they may
    // not be descendants of the root path.
    if ([attributes fileType] != NSFileTypeSymbolicLink &&
        [fileManager isWritableFileAtPath:child] == NO) {
      RELEASE_AUTORELEASE_POOL(pool);
      return false;
    }
  }

  RELEASE_AUTORELEASE_POOL(pool);
  return true;
}
