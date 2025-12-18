cmake_minimum_required(VERSION 3.16.0)

set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS}
    components/esp-adf/components/esp_codec_dev
    components/esp_hosted
    components/esp-wifi-remote/components/esp_wifi_remote
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(drone-cam)
