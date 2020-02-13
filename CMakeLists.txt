cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME Pseudo3DEngine)

project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -static")

set(SOURCE_FILES main.cpp settings.h World.cpp World.h Object2D.cpp Object2D.h Camera.cpp Camera.h Idrawable.h Poligon2D.cpp Poligon2D.h Circle2D.cpp Circle2D.h Menu.cpp Menu.h Weapon.cpp Weapon.h UDPSocketConnection.cpp UDPSocketConnection.h)

if(${APPLE})
    include_directories(/usr/local/include)
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
else()

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
    else ()
        add_executable(${PROJECT_NAME} ${SOURCE_FILES})
    endif ()

    set(SFML_DIR "D:/Libraries/SFML/lib/cmake/SFML")

    set(SFML_STATIC_LIBRARIES TRUE)

endif()

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} sfml-audio sfml-network sfml-graphics sfml-window sfml-system)
endif ()