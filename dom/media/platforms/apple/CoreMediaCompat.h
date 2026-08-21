/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_CoreMediaCompat_h
#define mozilla_CoreMediaCompat_h

// CoreMedia and VideoToolbox are private frameworks on OS X 10.6 and its SDK
// does not ship their headers.  This is the complete CoreMedia ABI surface
// used by the dynamically-linked Apple decoder.

#include <CoreFoundation/CoreFoundation.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t CMTimeValue;
typedef int32_t CMTimeScale;
typedef uint32_t CMTimeFlags;
typedef int64_t CMTimeEpoch;

typedef struct {
  CMTimeValue value;
  CMTimeScale timescale;
  CMTimeFlags flags;
  CMTimeEpoch epoch;
} CMTime;

typedef const struct opaqueCMFormatDescription* CMFormatDescriptionRef;
typedef CMFormatDescriptionRef CMVideoFormatDescriptionRef;
typedef struct OpaqueCMBlockBuffer* CMBlockBufferRef;
typedef struct opaqueCMSampleBuffer* CMSampleBufferRef;

typedef uint32_t CMVideoCodecType;
typedef uint32_t CMBlockBufferFlags;
typedef CFIndex CMItemCount;

enum {
  kCMVideoCodecType_H264 = 'avc1'
};

typedef struct {
  uint32_t version;
  void* (*AllocateBlock)(void* refCon, size_t sizeInBytes);
  void (*FreeBlock)(void* refCon, void* doomedMemoryBlock,
                    size_t sizeInBytes);
  void* refCon;
} CMBlockBufferCustomBlockSource;

typedef struct {
  CMTime duration;
  CMTime presentationTimeStamp;
  CMTime decodeTimeStamp;
} CMSampleTimingInfo;

typedef OSStatus (*CMSampleBufferMakeDataReadyCallback)(
    CMSampleBufferRef sampleBuffer, void* makeDataReadyRefCon);

CMTime CMTimeMake(int64_t value, int32_t timescale);

OSStatus CMVideoFormatDescriptionCreate(
    CFAllocatorRef allocator, CMVideoCodecType codecType, int32_t width,
    int32_t height, CFDictionaryRef extensions,
    CMVideoFormatDescriptionRef* formatDescriptionOut);

OSStatus CMBlockBufferCreateWithMemoryBlock(
    CFAllocatorRef structureAllocator, void* memoryBlock, size_t blockLength,
    CFAllocatorRef blockAllocator,
    const CMBlockBufferCustomBlockSource* customBlockSource,
    size_t offsetToData, size_t dataLength, CMBlockBufferFlags flags,
    CMBlockBufferRef* blockBufferOut);

OSStatus CMSampleBufferCreate(
    CFAllocatorRef allocator, CMBlockBufferRef dataBuffer, Boolean dataReady,
    CMSampleBufferMakeDataReadyCallback makeDataReadyCallback,
    void* makeDataReadyRefCon, CMFormatDescriptionRef formatDescription,
    CMItemCount numSamples, CMItemCount numSampleTimingEntries,
    const CMSampleTimingInfo* sampleTimingArray,
    CMItemCount numSampleSizeEntries, const size_t* sampleSizeArray,
    CMSampleBufferRef* sampleBufferOut);

#ifdef __cplusplus
}
#endif

#endif // mozilla_CoreMediaCompat_h
