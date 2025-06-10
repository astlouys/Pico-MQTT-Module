# =====================================================================================================================
# CMakeLists.txt for project Pico-MQTT-Example
# St-Louys Andre - May 2025
# astlouys@gmail.com
# Revision 21-MAY-2025
# Version 1.00
#
# REVISION HISTORY:
# =================
# 21-MAY-2025 1.00 - Initial release.
# =====================================================================================================================
#
#
#
# =====================================================================================================================
#                                          Set the minimum cmake version.
# =====================================================================================================================
cmake_minimum_required(VERSION 3.16)
#
#
#
# =====================================================================================================================
#                              Set the microcontroler board type used for this project.
# =====================================================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")
# set(PICO_BOARD pico2 CACHE STRING "Board type")
# set(PICO_BOARD "pico2")
# set(PICO_BOARD pico2_w CACHE STRING "Board type")
#
# If a Pico2 is used, specify if we want to use the Risc cores.
# set(PICO_PLATFORM "rp2350-riscv")
#
#
#
# =====================================================================================================================
#                               Pull in Raspberry Pi Pico SDK (must be before project).
# =====================================================================================================================
# It is recommended to create a symbolic link to the SDK in the project directory.
# This way, the symbolic link always points to the latest SDK version when it is updated.
include(pico_sdk_import.cmake)
#
# Another option: Set Pico SDK path (should be already declared in the environment variables).
# set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")
# set(PICO_SDK_PATH  "~/pico/pico-sdk")
#
# Another option: Pico SDK may be initialized from Git:
# set(PICO_SDK_FETCH_FROM_GIT on)
#
# Another option: Pico SDK may also be read explicitely from the SDK path.
# include(~/pico/pico-sdk/external/Pico_sdk_import.cmake)
#
#
#
# =====================================================================================================================
#                                       Set the minimum SDK version required.
# =====================================================================================================================
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message("====================================================================")
  message("Raspberry Pi Pico SDK version 1.4.0 (or later) required.")
  message("Your version is ${PICO_SDK_VERSION_STRING}. Aborting build...")
  message("====================================================================")
  return()
endif()
#
#
#
# =====================================================================================================================
#                        Define the project / Specify the project name (name of the executable).
# =====================================================================================================================
# project(Pico-MQTT-Example C CXX ASM)
# project(Pico-MQTT-Example)
set(NAME Pico-MQTT-Example)
project(${NAME} C CXX ASM)
message("-------> Building ${NAME}")
#
#
#
# =====================================================================================================================
#                                             Set the C-Language version.
# =====================================================================================================================
set (C_STANDARD 11)
set (CXX_STANDARD 17)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)
#
#
#
# =====================================================================================================================
#                                           Initialize Raspberry Pi Pico SDK.
# =====================================================================================================================
pico_sdk_init()
#
#
#
# =====================================================================================================================
#                                         Optionally add options to the compiler.
# =====================================================================================================================
# add_compile_options(
#         -Wall
#         -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#         -Wno-unused-function # we have some for the docs that aren't called
#         -Wno-maybe-uninitialized
#         )
#
#
#
# =====================================================================================================================
#                   Make sure all environment variables have been defined and proceed to the build.
# =====================================================================================================================
if (PICO_CYW43_SUPPORTED) # set by PICO_BOARD definition above
  if (NOT TARGET pico_cyw43_arch)
    message("Skipping build as support is not available")
    return()
  else()
    # If not already done, edit file <.bashrc> and add those environment variable definitions.
    set(WIFI_SSID      "$ENV{WIFI_SSID}"      CACHE INTERNAL "WIFI_SSID")
    set(WIFI_PASSWORD  "$ENV{WIFI_PASSWORD}"  CACHE INTERNAL "WIFI_PASSWORD")
    set(MQTT_BROKER_IP "$ENV{MQTT_BROKER_IP}" CACHE INTERNAL "MQTT_BROKER_IP")
    set(MQTT_PASSWORD  "$ENV{MQTT_PASSWORD}"  CACHE INTERNAL "MQTT_PASSWORD")
    message("========================================================================================================")
    message("Setting WiFi SSID:           <${WIFI_SSID}>")
    message("Setting WiFi password:       <${WIFI_PASSWORD}>")
    message("Setting broker IP address to <${MQTT_BROKER_IP}>")
    message("Setting broker password   to <${MQTT_PASSWORD}>")
    message("========================================================================================================")
    if ("${WIFI_SSID}" STREQUAL "")
      message("Environment variable WIFI_SSID (network name) is not defined... aborting build process.")
      message("(edit your .bashrc file to add the WIFI_SSID environment variable or modify the CMakeLists.txt file to define it there.")
      return()
    elseif ("${WIFI_PASSWORD}" STREQUAL "")
      message("Environment variable WIFI_PASSWORD (network password) is not defined... aborting build process.")
      message("(edit your .bashrc file to add the WIFI_PASSWORD environment variable or modify the CMakeLists.txt file to define it there.")
      return()
    elseif ("${MQTT_BROKER_IP}" STREQUAL "")
      message("Environment variable MQTT_BROKER_IP is not defined... aborting build process.")
      message("(edit your .bashrc file to add the MQTT_BROKER_IP environment variable or modify the CMakeLists.txt file to define it there.")
      return()
    elseif ("${MQTT_PASSWORD}" STREQUAL "")
      message("Environment variable MQTT_PASSWORD is not defined... aborting build process.")
      message("(edit your .bashrc file to add the MQTT_PASSWORD environment variable or modify the CMakeLists.txt file to define it there.")
      return()
    else()
      # Add executable. Default name is the project name.
      # add_executable(Pico-MQTT-Example   Pico-MQTT-Example.c   Pico-MQTT-Module.c   Pico-WiFi-Module.c)
      add_executable(
        ${NAME}
        ${NAME}.c
        Pico-MQTT-Module.c
        Pico-WiFi-Module.c
      )
      #
      pico_set_program_name(Pico-MQTT-Example "Pico-MQTT-Example")
      #
      # Enable USB output, disable UART output.
      pico_enable_stdio_usb(Pico-MQTT-Example 1)
      pico_enable_stdio_uart(Pico-MQTT-Example 0)
      #
      target_compile_definitions(
        Pico-MQTT-Example PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        MQTT_BROKER_IP=\"${MQTT_BROKER_IP}\"
        MQTT_PASSWORD=\"${MQTT_PASSWORD}\"
        NO_SYS=1
      )
      #
      # Add the standard include files to the build
      target_include_directories(
        Pico-MQTT-Example PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
      )
      #
      # Add libraries requested to the build
      target_link_libraries(
        Pico-MQTT-Example
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
        pico_multicore
        hardware_i2c
        # hardware_pwm
        # hardware_spi
        hardware_uart
        pico_bootrom
      )
      #
      pico_add_extra_outputs(Pico-MQTT-Example)
    endif()
  endif()
endif()
#
