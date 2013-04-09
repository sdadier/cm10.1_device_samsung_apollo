## Specify phone tech before including full_phone
$(call inherit-product, vendor/cm/config/gsm.mk)

# Release name
PRODUCT_RELEASE_NAME := apollo
PRODUCT_PACKAGE_OVERLAYS := device/samsung/apollo/overlay

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_mini_phone.mk)

# Inherit device configuration
$(call inherit-product, device/samsung/apollo/full_apollo.mk)


## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := apollo
PRODUCT_NAME := cm_apollo
PRODUCT_BRAND := samsung
PRODUCT_MODEL := GT-I5800
PRODUCT_MANUFACTURER := Samsung

