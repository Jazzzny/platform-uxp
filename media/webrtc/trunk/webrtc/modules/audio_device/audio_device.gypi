# Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'targets': [
    {
      'target_name': 'audio_device',
      'type': 'static_library',
      'dependencies': [
        'webrtc_utility',
        '<(webrtc_root)/base/base.gyp:rtc_base_approved',
        '<(webrtc_root)/common.gyp:webrtc_common',
        '<(webrtc_root)/common_audio/common_audio.gyp:common_audio',
        '<(webrtc_root)/system_wrappers/system_wrappers.gyp:system_wrappers',
      ],
      'include_dirs': [
        '.',
        '../interface',
        'include',
        'dummy',  # Contains dummy audio device implementations.
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../interface',
          'include',
        ],
      },
      # TODO(xians): Rename files to e.g. *_linux.{ext}, remove sources in conditions section
      'sources': [
        'include/audio_device.h',
        'include/audio_device_defines.h',
        'audio_device_buffer.cc',
        'audio_device_buffer.h',
        'audio_device_generic.cc',
        'audio_device_generic.h',
        'audio_device_utility.cc',
        'audio_device_utility.h',
        'audio_device_impl.cc',
        'audio_device_impl.h',
        'audio_device_config.h',
        'dummy/audio_device_dummy.cc',
        'dummy/audio_device_dummy.h',
        'dummy/audio_device_utility_dummy.cc',
        'dummy/audio_device_utility_dummy.h',
        'dummy/file_audio_device.cc',
        'dummy/file_audio_device.h',
      ],
      'conditions': [
        ['build_with_mozilla==1', {
          'cflags_mozilla': [
            '$(NSPR_CFLAGS)',
          ],
        }],
        ['hardware_aec_ns==1', {
          'defines': [
            'WEBRTC_HARDWARE_AEC_NS',
          ],
        }],
        ['include_sndio_audio==1', {
          'include_dirs': [
            'sndio',
          ],
        }], # include_sndio_audio==1
        ['OS=="linux" or include_alsa_audio==1 or include_pulse_audio==1', {
          'include_dirs': [
            'linux',
          ],
        }], # OS=="linux" or include_alsa_audio==1 or include_pulse_audio==1
        ['OS=="mac"', {
          'include_dirs': [
            'mac',
          ],
        }], # OS==mac
        ['OS=="win"', {
          'include_dirs': [
            'win',
          ],
        }],
        ['include_internal_audio_device==0', {
          'defines': [
            'WEBRTC_DUMMY_AUDIO_BUILD',
          ],
        }],
        ['build_with_chromium==0', {
          'sources': [
            # Don't link these into Chrome since they contain static data.
            'dummy/file_audio_device_factory.cc',
            'dummy/file_audio_device_factory.h',
          ],
        }],
        ['include_internal_audio_device==1', {
          'sources': [
            'linux/audio_device_utility_linux.cc',
            'linux/audio_device_utility_linux.h',
            'linux/latebindingsymboltable_linux.cc',
            'linux/latebindingsymboltable_linux.h',
            'mac/audio_device_mac.cc',
            'mac/audio_device_mac.h',
            'mac/audio_device_utility_mac.cc',
            'mac/audio_device_utility_mac.h',
            'mac/audio_mixer_manager_mac.cc',
            'mac/audio_mixer_manager_mac.h',
            'mac/portaudio/pa_memorybarrier.h',
            'mac/portaudio/pa_ringbuffer.c',
            'mac/portaudio/pa_ringbuffer.h',
            'win/audio_device_core_win.cc',
            'win/audio_device_core_win.h',
            'win/audio_device_wave_win.cc',
            'win/audio_device_wave_win.h',
            'win/audio_device_utility_win.cc',
            'win/audio_device_utility_win.h',
            'win/audio_mixer_manager_win.cc',
            'win/audio_mixer_manager_win.h',
          ],
          'conditions': [
            ['OS=="linux"', {
              'link_settings': {
                'libraries': [
                  '-ldl','-lX11',
                ],
              },
            }],
            ['include_sndio_audio==1', {
              'link_settings': {
                'libraries': [
                  '-lsndio',
                ],
              },
              'sources': [
                'sndio/audio_device_sndio.cc',
                'sndio/audio_device_sndio.h',
                'sndio/audio_device_utility_sndio.cc',
                'sndio/audio_device_utility_sndio.h',
              ],
            }],
            ['include_alsa_audio==1', {
              'cflags_mozilla': [
                '$(MOZ_ALSA_CFLAGS)',
              ],
              'defines': [
                'LINUX_ALSA',
              ],
              'sources': [
                'linux/alsasymboltable_linux.cc',
                'linux/alsasymboltable_linux.h',
                'linux/audio_device_alsa_linux.cc',
                'linux/audio_device_alsa_linux.h',
                'linux/audio_mixer_manager_alsa_linux.cc',
                'linux/audio_mixer_manager_alsa_linux.h',
              ],
            }],
            ['include_pulse_audio==1', {
              'cflags_mozilla': [
                '$(MOZ_PULSEAUDIO_CFLAGS)',
              ],
              'defines': [
                'LINUX_PULSE',
              ],
              'sources': [
                'linux/audio_device_pulse_linux.cc',
                'linux/audio_device_pulse_linux.h',
                'linux/audio_mixer_manager_pulse_linux.cc',
                'linux/audio_mixer_manager_pulse_linux.h',
                'linux/pulseaudiosymboltable_linux.cc',
                'linux/pulseaudiosymboltable_linux.h',
              ],
            }],
            ['OS=="mac"', {
              'link_settings': {
                'libraries': [
                  '$(SDKROOT)/System/Library/Frameworks/AudioToolbox.framework',
                  '$(SDKROOT)/System/Library/Frameworks/CoreAudio.framework',
                ],
              },
            }],
            ['OS=="win"', {
              'link_settings': {
                'libraries': [
                  # Required for the built-in WASAPI AEC.
                  '-ldmoguids.lib',
                  '-lwmcodecdspuuid.lib',
                  '-lamstrmid.lib',
                  '-lmsdmo.lib',
                ],
              },
            }],
          ], # conditions
        }], # include_internal_audio_device==1
      ], # conditions
    },
  ],
  'conditions': [
    ['include_tests==1', {
      'targets': [
        {
          'target_name': 'audio_device_tests',
         'type': 'executable',
         'dependencies': [
            'audio_device',
            'webrtc_utility',
            '<(webrtc_root)/test/test.gyp:test_support_main',
            '<(DEPTH)/testing/gtest.gyp:gtest',
            '<(webrtc_root)/system_wrappers/system_wrappers.gyp:system_wrappers',
          ],
          'sources': [
            'test/audio_device_test_api.cc',
            'test/audio_device_test_defines.h',
          ],
        },
        {
          'target_name': 'audio_device_test_func',
          'type': 'executable',
          'dependencies': [
            'audio_device',
            'webrtc_utility',
            '<(webrtc_root)/common_audio/common_audio.gyp:common_audio',
            '<(webrtc_root)/system_wrappers/system_wrappers.gyp:system_wrappers',
            '<(webrtc_root)/test/test.gyp:test_support',
            '<(DEPTH)/testing/gtest.gyp:gtest',
          ],
          'sources': [
            'test/audio_device_test_func.cc',
            'test/audio_device_test_defines.h',
            'test/func_test_manager.cc',
            'test/func_test_manager.h',
          ],
        },
      ], # targets
      'conditions': [
        ['test_isolation_mode != "noop"', {
          'targets': [
            {
              'target_name': 'audio_device_tests_run',
              'type': 'none',
              'dependencies': [
                'audio_device_tests',
              ],
              'includes': [
                '../../build/isolate.gypi',
              ],
              'sources': [
                'audio_device_tests.isolate',
              ],
            },
          ],
        }],
      ],
    }], # include_tests
  ],
}
