cmake_minimum_required(VERSION 3.16)

project(Lithium VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Network)

include(FetchContent)

set(QCORO_WITH_QTQML OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTQUICK OFF CACHE BOOL "" FORCE)
set(QCORO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QCORO_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    QCoro
    GIT_REPOSITORY https://github.com/danvratil/qcoro.git
    GIT_TAG v0.10.0
)
FetchContent_MakeAvailable(QCoro)


qt_add_executable(Lithium
    WIN32 MACOSX_BUNDLE
    main.cpp
    login.cpp
    login.h
    login.ui
    clientcontroller.cpp
    clientcontroller.h
    clientchat.h
    clientchat.cpp
    settings.ui
    form.ui
    form.h
    form.cpp
    settings.h
    settings.cpp

)

target_link_libraries(Lithium PRIVATE
    Qt6::Widgets
    Qt6::Network
    QCoro6Core
    QCoro6Network
)

set_target_properties(Lithium PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Lithium
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Lithium)
endif()
