cmake_minimum_required(VERSION 3.5)

project(qtllama VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (UNIX AND APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif (UNIX AND APPLE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/ui/MainWindow.h src/ui/MainWindow.cpp src/ui/MainWindow.ui
        src/llm/Interface/LlamaInterface.h src/llm/Interface/LlamaInterface.cpp
        src/chat/Chat.h src/chat/Chat.cpp
        src/llm/Interface/LlamaThread.h src/llm/Interface/LlamaThread.cpp
        src/config/ConfigApp.cpp
        src/config/ConfigApp.h
        src/llm/Samplers/Sampler.h
        src/llm/Samplers/Sampler.cpp
        src/llm/Samplers/SamplerArray.cpp
        src/llm/Samplers/SamplerArray.h
        src/ui/SamplerWidget.h src/ui/SamplerWidget.cpp src/ui/SamplerWidget.ui
        src/ui/MessageWidget.h src/ui/MessageWidget.cpp src/ui/MessageWidget.ui
        src/ui/BoolParamWidget.h src/ui/BoolParamWidget.cpp src/ui/BoolParamWidget.ui
        src/ui/NumParamWidget.h src/ui/NumParamWidget.cpp src/ui/NumParamWidget.ui
)
include_directories(${PROJECT_SOURCE_DIR}/src/ui) # hack for custom widgets

set(LLAMA_CPP_DIR submodules/llama.cpp)
set(LLAMA_CPP_HEADERS ${LLAMA_CPP_DIR}/include/llama.h)

include_directories(${LLAMA_CPP_DIR}/include)
add_subdirectory(${LLAMA_CPP_DIR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtllama
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${LLAMA_CPP_HEADERS}
    )
else()
    add_executable(qtllama
        ${PROJECT_SOURCES}
        ${LLAMA_CPP_HEADERS}

    )
endif()

target_link_libraries(qtllama PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qtllama PRIVATE llama)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qtllama)
endif()
set_target_properties(qtllama PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    CXX_CLANG_TIDY clang-tidy
)

set_property(TARGET qtllama PROPERTY COMPILE_WARNING_AS_ERROR ON)

include(GNUInstallDirs)
install(TARGETS qtllama
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qtllama)
endif()
