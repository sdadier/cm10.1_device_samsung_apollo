# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# BoardConfig.mk
#
# Product-specific compile-time definitions.
#
BOARD_USES_GENERIC_AUDIO := false

BOARD_USES_LIBSECRIL_STUB := true

# Target
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv6-vfp
TARGET_BOARD_PLATFORM := s5p6442
TARGET_CPU_ABI := armeabi-v6l
TARGET_CPU_ABI2 := armeabi
ARCH_ARM_HAVE_VFP := true
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true
TARGET_GLOBAL_CFLAGS += -mfpu=vfp -mfloat-abi=softfp -O3
TARGET_GLOBAL_CPPFLAGS += -mfpu=vfp -mfloat-abi=softfp -O3

TARGET_PREBUILT_KERNEL := device/samsung/apollo/prebuilt/kernel
TARGET_RECOVERY_INITRC := device/samsung/apollo/prebuilt/recovery.rc

## Webkit
ENABLE_WEBGL := true
TARGET_WEBKIT_USE_MORE_MEMORY := true
BOARD_NEEDS_MEMORYHEAPPMEM := true
BOARD_USE_NASTY_PTHREAD_CREATE_HACK := true
TARGET_USES_16BPPSURFACE_FOR_OPAQUE := true
TARGET_AVOID_DRAW_TEXTURE_EXTENSION := true
TARGET_NO_HW_VSYNC := true
BOARD_FRAMEBUFFER_FORCE_FORMAT := HAL_PIXEL_FORMAT_RGB_565

TARGET_PROVIDES_INIT := true
BOARD_KERNEL_CMDLINE := console=ttySAC1,115200 loglevel=4 no_console_suspend
BOARD_KERNEL_BASE := 0x22000000

BOARD_VENDOR_USE_AKMD := akm8973
TARGET_USES_OLD_LIBSENSORS_HAL := true

# Graphics
TARGET_BOARD_PLATFORM_GPU := fimg
BOARD_EGL_CFG := device/samsung/apollo/prebuilt/graphics/egl.cfg
TARGET_LIBAGL_USE_GRALLOC_COPYBITS := true
#BOARD_EGL_NEEDS_LEGACY_FB := true
BOARD_NEEDS_MEMORYHEAPPMEM := true

# Camera
USE_CAMERA_STUB := false
BOARD_CAMERA_LIBRARIES := libcamera
BOARD_CAMERA_DEVICE := /dev/video0
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CAMERA_HARDWARE
COMMON_GLOBAL_CFLAGS += -DBINDER_COMPAT

TARGET_DISABLE_TRIPLE_BUFFERING := false

# Bootanimation
TARGET_BOOTANIMATION_PRELOAD := false
TARGET_BOOTANIMATION_TEXTURE_CACHE := true
TARGET_BOOTANIMATION_USE_RGB565 := true

# Javascript, Browser and Webkit
WITH_JIT := true
ENABLE_JSC_JIT := true
JS_ENGINE := v8
HTTP := chrome
BOARD_USE_LEGACY_TOUCHSCREEN := true

BOARD_NAND_PAGE_SIZE := 4096
BOARD_NAND_SPARE_SIZE := 128

# RIL
BOARD_MOBILEDATA_INTERFACE_NAME := "pdp0"
BOARD_RIL_CLASS := ../../../device/samsung/apollo/ril/
BOARD_USES_FROYO_RILCLIENT := true
#TARGET_PROVIDES_LIBRIL := true
BOARD_USES_LEGACY_RIL := true
#BOARD_FORCE_RILD_AS_ROOT := true

# FM Radio
BOARD_HAVE_FM_RADIO := true
BOARD_GLOBAL_CFLAGS += -DHAVE_FM_RADIO
BOARD_FM_DEVICE := si4709

# Bluetooth
BOARD_HAVE_SAMSUNG_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

## Device specific libs
TARGET_PROVIDES_LIBAUDIO := true
TARGET_PROVIDES_LIBLIGHT := true

## Fix colors in panorama mode
BOARD_CPU_COLOR_CONVERT := true

# Wifi
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wext
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
BOARD_WLAN_DEVICE := bcm4329
WIFI_DRIVER_FW_STA_PATH     := "/system/etc/wifi/bcm4329_sta.bin"
WIFI_DRIVER_FW_AP_PATH      := "/system/etc/wifi/bcm4329_mfg.bin"
WIFI_DRIVER_FW_P2P_PATH     := "/system/etc/wifi/bcm4329_sta.bin"
WIFI_DRIVER_MODULE_PATH     := "/lib/modules/dhd.ko"
WIFI_DRIVER_MODULE_ARG      := "firmware_path=/system/etc/wifi/bcm4329_sta.bin nvram_path=/system/etc/wifi/nvram_net.txt"
WIFI_DRIVER_MODULE_NAME     := "dhd"

# NAND
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 8388608
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 222822400
BOARD_USERDATAIMAGE_PARTITION_SIZE := 204996608
BOARD_FLASH_BLOCK_SIZE := 4096

# Releasetools
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := ./device/samsung/apollo/releasetools/apollo_ota_from_target_files
TARGET_RELEASETOOL_IMG_FROM_TARGET_SCRIPT := ./device/samsung/apollo/releasetools/apollo_img_from_target_files

# USB
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/platform/s3c-usbgadget/gadget/lun0/file"
BOARD_UMS_LUNFILE := /sys/devices/platform/s3c-usbgadget/gadget/lun0/file
BOARD_USE_USB_MASS_STORAGE_SWITCH := true

# Recovery
BOARD_USES_BOOTMENU := false
BOARD_PROVIDES_BOOTMODE := true
BOARD_USES_BML_OVER_MTD := false
#BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/samsung/apollo/recovery/recovery_ui.c
BOARD_BOOT_DEVICE := /dev/block/bml5
BOARD_DATA_DEVICE := /dev/block/stl7
BOARD_DATA_FILESYSTEM := ext4
BOARD_DATA_FILESYSTEM_OPTIONS := llw,check=no,nosuid,nodev
BOARD_HAS_DATADATA := false
BOARD_SYSTEM_DEVICE := /dev/block/stl6
BOARD_SYSTEM_FILESYSTEM := ext4
BOARD_SYSTEM_FILESYSTEM_OPTIONS := llw,check=no
BOARD_BML_BOOT := "/dev/block/bml5"
BOARD_CACHE_DEVICE := /dev/block/stl8
BOARD_CACHE_FILESYSTEM := rfs
BOARD_CACHE_FILESYSTEM_OPTIONS := llw,check=no,nosuid,nodev
BOARD_SDEXT_DEVICE := /dev/block/mmcblk0p2
BOARD_SDEXT_FILESYSTEM := auto
BOARD_HAS_PARAMFS := false
BOARD_USES_BMLUTILS := true
BOARD_SDCARD_DEVICE_PRIMARY := /dev/block/mmcblk1p1
BOARD_SDCARD_DEVICE_SECONDARY := /dev/block/mmcblk0p1
BOARD_HAS_SDCARD_INTERNAL := false
BOARD_HAS_DOWNLOAD_MODE := true
TARGET_ROOT_IS_RECOVERY := true
TARGET_BOOT_IS_RAMDISK := true
BOARD_RECOVERY_IGNORE_BOOTABLES := true
TARGET_RECOVERY_PRE_COMMAND := "echo 1 > /cache/.startrecovery; sync;"
BOARD_RECOVERY_HANDLES_MOUNT := true
TARGET_NO_KERNEL := true
BOARD_LDPI_RECOVERY := true
