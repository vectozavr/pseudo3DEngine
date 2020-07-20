cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME FPSOnline)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -static")

set(SOURCE_FILES settings.h Button.cpp Button.h Camera.cpp Camera.h Circle2D.h Circle2D.cpp ClientUDP.cpp ClientUDP.h Idrawable.h main.cpp Menu.cpp Menu.h MsgType.cpp MsgType.h Object2D.cpp Object2D.h ObjectType.h Player.cpp Player.h Point2D.cpp Point2D.h Poligon2D.cpp Poligon2D.h ReliableMsg.cpp ReliableMsg.h ResourceManager.cpp ResourceManager.h ServerUDP.cpp ServerUDP.h Time.cpp Time.h UDPConnection.cpp UDPConnection.h UDPSocket.cpp UDPSocket.h Weapon.cpp Weapon.h World.cpp World.h Map2D.h Point3D.h Point3D.cpp Bonus.h Bonus.cpp)

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
    set(PROJECT_NAME FPSOnline)
endif()