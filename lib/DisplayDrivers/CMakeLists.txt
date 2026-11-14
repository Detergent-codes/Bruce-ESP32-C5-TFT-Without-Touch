# ESP-IDF component build ("Arduino as component").
#
# Drop this folder into your project's components/ directory. Only the backend
# selected by the USE_* build flag compiles; the other sources reduce to nothing,
# so the display library you are not using does not have to be present.
#
#   idf.py menuconfig  -> nothing to set here
#   Add the flag in your project CMakeLists, e.g.
#       idf_build_set_property(COMPILE_OPTIONS "-DUSE_ARDUINO_GFX=1" APPEND)

file(GLOB DISPLAYDRIVERS_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/backends/*.cpp)

idf_component_register(
    SRCS ${DISPLAYDRIVERS_SRCS}
    INCLUDE_DIRS "src"
    REQUIRES driver esp_timer
)

# The Arduino core and the backend graphics libraries are optional: a build only
# ever needs the one its USE_* flag selects. idf_component_optional_requires()
# (IDF >= 5.0) silently skips whatever is not in the build.
set(DISPLAYDRIVERS_OPTIONAL
    arduino-esp32 arduino
    GFX_Library_for_Arduino Arduino_GFX
    LovyanGFX
    M5Unified M5GFX m5gfx
    GxEPD2
    EPD_Painter
    Adafruit_GFX Adafruit-GFX-Library
    TFT_eSPI
)

if(COMMAND idf_component_optional_requires)
    idf_component_optional_requires(PUBLIC ${DISPLAYDRIVERS_OPTIONAL})
else()
    # IDF 4.x: fall back to whichever of them exists as a sibling component.
    get_filename_component(_dd_parent "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
    foreach(_dd_lib ${DISPLAYDRIVERS_OPTIONAL})
        if(EXISTS "${_dd_parent}/${_dd_lib}/CMakeLists.txt")
            idf_component_get_property(_dd_lib_lib ${_dd_lib} COMPONENT_LIB)
            target_link_libraries(${COMPONENT_LIB} PUBLIC ${_dd_lib_lib})
        endif()
    endforeach()
endif()
