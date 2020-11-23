cmake_minimum_required(VERSION 3.5)

project(LocalSend LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_NAME LocalSend)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

if (ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

add_subdirectory(qtstatusbar/src)
#add_subdirectory(extra-cmake-modules/)

set(ECM_DIR /Users/mpatil/Documents/QtProjects/LocalSend/extra-cmake-modules/)
find_package(Qt5 COMPONENTS Core Quick LinguistTools REQUIRED)
find_package(Qt5 COMPONENTS Core Quick QuickControls2 REQUIRED)
find_package(Qt5Bluetooth REQUIRED)

set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/releaseFiles/mac/LocalSend.icns)
set(APP_ICON_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/releaseFiles/windows/icon.ico)

set(SRC_FILES
    main.cpp
    tcpserverhandler.cpp
    tcpserverhandler.h
    tcpclienthandler.cpp
    tcpclienthandler.h
    udphandler.cpp
    udphandler.h
    devicelist.cpp
    devicelist.h
    qml.qrc
    )
set(LIBS
    PRIVATE StatusBar Qt5::Core Qt5::Quick Qt5::QuickControls2 Qt5::Bluetooth)
if(ANDROID)
    add_library(LocalSend SHARED
        android/AndroidManifest.xml
        ${SRC_FILES}
        )
    find_package(Qt5AndroidExtras REQUIRED)
    target_link_libraries(LocalSend
        ${LIBS} Qt5::AndroidExtras)
elseif(CMAKE_HOST_APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(MACOSX_BUNDLE_ICON_FILE LocalSend.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(LocalSend
        MACOSX_BUNDLE
        ${APP_ICON_MACOSX}
        ${SRC_FILES}
        )
    set_target_properties(LocalSend PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/releaseFiles/mac/Info.plist)
    target_link_libraries(LocalSend
        ${LIBS})
elseif(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/releaseFiles/windows/windows.rc")
    add_executable(LocalSend
        ${SRC_FILES}
        ${APP_ICON_RESOURCE_WINDOWS}
        )
    target_link_libraries(LocalSend
        ${LIBS})
else()
    add_executable(LocalSend
        ${SRC_FILES}
        )
    target_link_libraries(LocalSend
        ${LIBS})
endif()

target_compile_definitions(LocalSend
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)



qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
