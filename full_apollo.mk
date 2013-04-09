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

# This file includes all definitions that apply only to apollo devices
#
# Anything that is generic to all apollo products should go in the tuna directory
#
# Everything in this directory will become public

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

$(call inherit-product, device/samsung/apollo/device.mk)
$(call inherit-product-if-exists, vendor/samsung/apollo/device-vendor.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

PRODUCT_DEVICE := apollo
PRODUCT_NAME := full_apollo
PRODUCT_BRAND := samsung
PRODUCT_MODEL := GT-I5800
PRODUCT_MANUFACTURER := Samsung

