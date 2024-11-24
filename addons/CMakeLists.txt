############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
#     SkinnyGlobal.h
#     SkinnyNamespace.h SkinnyNamespace.cpp
#     SkinnyShapeFactory.h SkinnyShapeFactory.cpp
#     SkinnyShapeProvider.h SkinnyShapeProvider.cpp
#     SkinnyShortcut.h SkinnyShortcut.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/Skin.h
../examples/iotdashboard/Skin.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/MenuBar.h
../examples/iotdashboard/MenuBar.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/UsageBox.h
../examples/iotdashboard/UsageBox.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/Box.h
../examples/iotdashboard/Box.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/GridBox.h
../examples/iotdashboard/GridBox.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/StorageBar.h
../examples/iotdashboard/StorageBar.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/StorageBarSkinlet.h
../examples/iotdashboard/StorageBarSkinlet.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/StoragePage.h
../examples/iotdashboard/StoragePage.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/LightDisplay.h
../examples/iotdashboard/LightDisplay.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/LightDisplaySkinlet.h
../examples/iotdashboard/LightDisplaySkinlet.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/RoomsPage.h
../examples/iotdashboard/RoomsPage.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/Diagram.h
../examples/iotdashboard/Diagram.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/DiagramSkinlet.h
../examples/iotdashboard/DiagramSkinlet.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/BoxWithButtons.h
../examples/iotdashboard/BoxWithButtons.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/RoundedIcon.h
../examples/iotdashboard/RoundedIcon.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/RoundButton.h
../examples/iotdashboard/RoundButton.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/TopBar.h
../examples/iotdashboard/TopBar.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/ValueMeter.h
../examples/iotdashboard/ValueMeter.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/UsageDiagram.h
../examples/iotdashboard/UsageDiagram.cpp
${CMAKE_SOURCE_DIR}/examples/iotdashboard/DashboardPage.h
../examples/iotdashboard/DashboardPage.cpp

)

# qt_add_resources(SOURCES DejaVuSans.qrc Roboto.qrc SegoeUI.qrc)

set(target qskaddons)

if (BUILD_QSKDLL)
    qsk_add_library(${target} SHARED ${SOURCES})
else()
    qsk_add_library(${target} STATIC ${SOURCES})
endif()

target_link_libraries(${target} PUBLIC qskinny)

if (BUILD_QSKDLL)
    set_target_properties( ${target} PROPERTIES DEFINE_SYMBOL SKINNY_MAKEDLL )
endif()

target_compile_definitions(${target}
    PRIVATE PLUGIN_PATH=${CMAKE_BINARY_DIR}/plugins)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(ENABLE_ENSURE_SKINS)
    target_include_directories(${target} PRIVATE ${QSK_SOURCE_DIR}/designsystems)
    target_compile_definitions(${target} PRIVATE ENSURE_SKINS ENSURE_FONTS)
    target_link_libraries(${target} PRIVATE material3skin fluent2skin fusionskin)
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)
