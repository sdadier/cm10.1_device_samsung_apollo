# Copyright (C) 2011 The Android Open Source Project
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

#
# This file is the build configuration for a full Android
# build for apollo hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps). Except for a few implementation
# details, it only fundamentally contains two inherit-product
# lines, full and apollo, hence its name.
#


$(call inherit-product, vendor/google/gapps_armv6_tiny.mk)
$(call inherit-product, device/common/gps/gps_eu_supl.mk)
$(call inherit-product, device/ldpi-common/ldpi.mk)

DEVICE_PACKAGE_OVERLAYS += device/samsung/apollo/overlay

PRODUCT_LOCALES += mdpi ldpi

PRODUCT_COPY_FILES += \
        device/samsung/apollo/etc/gps.conf:system/etc/gps.conf \
        device/samsung/apollo/etc/secgps.conf:system/etc/secgps.conf

PRODUCT_PACKAGES += \
    brcm_patchram_plus \
    gralloc.s5p6442 \
    sensors.s5p6442 \
    copybit.s5p6442 \
    camera.s5p6442 \
    hwcomposer.s5p6442 \
    gps.s5p6442 \
    lights.s5p6442 \
    libGLES_fimg \
    FileManager \
    screencap 

# audio
PRODUCT_PACKAGES += \
	audio.primary.s5p6442 \
	audio.a2dp.default \
	audio.usb.default

# Bluetooth MAC Address
PRODUCT_PACKAGES += \
    bdaddr_read

# Samsung Specific tools
PRODUCT_PACKAGES += \
    SamsungServiceMode 

# Live Wallpapers
PRODUCT_PACKAGES += \
    LiveWallpapers \
    LiveWallpapersPicker \
    VisualizationWallpapers \
    librs_jni

#wlan
PRODUCT_PACKAGES += \
	libnetcmdiface

PRODUCT_COPY_FILES += \
	device/samsung/apollo/kernel:kernel \
  device/samsung/apollo/klogtail:system/bin/klogtail

PRODUCT_COPY_FILES += \
	device/samsung/apollo/libaudio/audio_policy.conf:system/etc/audio_policy.conf

#    ro.telephony.ril.v3=icccardstatus,datacall,signalstrength,facilitylock \

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=120 \
    ro.telephony.ril_class=ApolloRIL \
    ro.telephony.ril.v3=icccardstatus,datacall,signalstrength,facilitylock \
    ro.telephony.sends_barcount=1 \
    ro.ril.oem.nosim.ecclist=911,112,999,000,08,118,120,122,110,119,995 \
    ro.ril.emc.mode=2 \
    ro.ril.enable.managed.roaming=1 \
    mobiledata.interfaces=pdp0,eth0,gprs,ppp0 \
    dalvik.vm.dexopt-flags=m=y \
    dalvik.vm.heapsize=32m \
    dalvik.vm.execution-mode=int:jit \
    dalvik.vm.dexopt-data-only=1 \
    ro.com.google.locationfeatures=1 \
    ro.com.google.networklocation=1 \
    ro.kernel.android.checkjni=0 \
    dalvik.vm.checkjni=false \
    persist.sys.use_dithering=0 \
    persist.sys.use_16bpp_alpha=1 \
    persist.sys.led_trigger=none \
    ro.opengles.version=65537 \
    media.stagefright.enable-player=true \
    windowsmgr.max_events_per_sec=150 \
    ro.telephony.call_ring.delay=0 \
    ro.ril.disable.power.collapse=0 \
    ro.setupwizard.enable_bypass=1

# Wifi
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=eth0 \
    wifi.supplicant_scan_interval=60


# These are the hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:system/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.xml:system/etc/permissions/android.hardware.touchscreen.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/platform.xml:system/etc/permissions/platform.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.software.sip.xml:system/etc/permissions/android.software.sip.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/com.tmobile.software.themes.xml:system/etc/permissions/com.tmobile.software.themes.xml

# Feature live wallpaper
PRODUCT_COPY_FILES += \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml


PRODUCT_COPY_FILES += \
        device/samsung/apollo/etc/media_profiles.xml:system/etc/media_profiles.xml \
        device/samsung/apollo/etc/media_codecs.xml:system/etc/media_codecs.xml \
        device/samsung/apollo/etc/asound.conf:system/etc/asound.conf \
        device/samsung/apollo/etc/dhcpcd.conf:system/etc/dhcpcd.conf \
        device/samsung/apollo/etc/vold.fstab:system/etc/vold.fstab \
        device/samsung/apollo/etc/dbus.conf:system/etc/dbus.conf \
        device/samsung/apollo/etc/sysctl.conf:system/etc/sysctl.conf \
        device/samsung/apollo/Credits-CM.html:system/etc/Credits-CM.html \
        device/samsung/apollo/prebuilt/keychars/qwerty.kcm.bin:system/usr/keychars/qwerty.kcm.bin \
        device/samsung/apollo/prebuilt/keychars/qwerty2.kcm.bin:system/usr/keychars/qwerty2.kcm.bin \
        device/samsung/apollo/prebuilt/keychars/qt602240_ts_input.kcm.bin:system/usr/keychars/qt602240_ts_input.kcm.bin \
        device/samsung/apollo/prebuilt/keychars/s3c-keypad.kcm.bin:system/usr/keychars/s3c-keypad.kcm.bin \
        device/samsung/apollo/prebuilt/keychars/sec_jack.kcm.bin:system/usr/keychars/sec_jack.kcm.bin \
        device/samsung/apollo/prebuilt/keylayout/qt602240_ts_input.kl:system/usr/keylayout/qt602240_ts_input.kl \
        device/samsung/apollo/prebuilt/keylayout/s3c-keypad.kl:system/usr/keylayout/s3c-keypad.kl \
        device/samsung/apollo/prebuilt/keylayout/sec_headset.kl:system/usr/keylayout/sec_headset.kl \
        device/samsung/apollo/prebuilt/idc/qt602240_ts_input.idc:system/usr/idc/qt602240_ts_input.idc \
        device/samsung/apollo/prebuilt/wifi/bcm4329_aps.bin:system/etc/wifi/bcm4329_aps.bin \
        device/samsung/apollo/prebuilt/wifi/bcm4329_mfg.bin:system/etc/wifi/bcm4329_mfg.bin \
        device/samsung/apollo/prebuilt/wifi/bcm4329_sta.bin:system/etc/wifi/bcm4329_sta.bin \
        device/samsung/apollo/prebuilt/wifi/nvram_mfg.txt:system/etc/wifi/nvram_mfg.txt \
        device/samsung/apollo/prebuilt/wifi/nvram_net.txt:system/etc/wifi/nvram_net.txt \
        device/samsung/apollo/prebuilt/wifi/wifi.conf:system/etc/wifi/wifi.conf \
        device/samsung/apollo/prebuilt/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
      	device/samsung/apollo/prebuilt/camera/libcamera.so:obj/lib/libcamera.so \
      device/samsung/apollo/prebuilt/ril/rild:system/bin/rild \
      device/samsung/apollo/prebuilt/ril/libril.so:system/lib/libril.so \
      device/samsung/apollo/prebuilt/ril/libsec-ril.so:system/lib/libsec-ril.so \
    	device/samsung/apollo/prebuilt/camera/libsecjpeg.so:obj/lib/libsecjpeg.so \
    	device/samsung/apollo/prebuilt/camera/libcamera.so:system/lib/libcamera.so \
    	device/samsung/apollo/prebuilt/gps/libsecgps.so:system/lib/libsecgps.so \
    	device/samsung/apollo/prebuilt/gps/libsecril-client.so:system/lib/libsecril-client.so \
    	device/samsung/apollo/prebuilt/gps/gps.s5p6442.so:system/lib/hw/gps.s5p6442.so \
    	device/samsung/apollo/prebuilt/sensors/akmd2:system/bin/akmd2 \
    	device/samsung/apollo/prebuilt/bluetooth/bcm4329.hcd:system/vendor/firmware/bcm4329.hcd \
    	device/samsung/apollo/prebuilt/bin/charging_mode:system/bin/charging_mode \
    	device/samsung/apollo/prebuilt/bin/playlpm:system/bin/playlpm \
    	device/samsung/apollo/prebuilt/bin/immvibed:system/bin/immvibed 

# HardwareRenderer properties
PRODUCT_PROPERTY_OVERRIDES += \
    hwui.render_dirty_regions=false

# Misc properties
PRODUCT_PROPERTY_OVERRIDES += \
    pm.sleep_mode=1

# Set default USB interface
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mass_storage

# LDPI assets
PRODUCT_AAPT_CONFIG := normal ldpi mdpi
PRODUCT_AAPT_PREF_CONFIG := ldpi

# Copy bcm4329 firmware
$(call inherit-product-if-exists, hardware/broadcom/wlan/bcmdhd/firmware/bcm4329/device-bcm.mk)

