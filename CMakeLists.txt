cmake_minimum_required(VERSION 3.7)
project(FPSOnline)

set(CMAKE_CXX_STANDARD 17)

add_executable(FPSOnline settings.h Button.cpp Button.h Camera.cpp Camera.h Circle2D.h Circle2D.cpp ClientUDP.cpp ClientUDP.h Idrawable.h main.cpp Menu.cpp Menu.h MsgType.cpp MsgType.h Object2D.cpp Object2D.h ObjectType.h Player.cpp Player.h Point2D.cpp Point2D.h Poligon2D.cpp Poligon2D.h ReliableMsg.cpp ReliableMsg.h ResourceManager.cpp ResourceManager.h ServerUDP.cpp ServerUDP.h Time.cpp Time.h UDPConnection.cpp UDPConnection.h UDPSocket.cpp UDPSocket.h Weapon.cpp Weapon.h World.cpp World.h Map2D.h Point3D.h Point3D.cpp Bonus.h Bonus.cpp Enemy.cpp Enemy.h GeneticAlgorithm.cpp GeneticAlgorithm.h NeuralNetwork.cpp NeuralNetwork.h OptimizedBots.cpp OptimizedBots.h)

if(${APPLE})
    include_directories(/usr/local/include)
else()
    set(SFML_DIR "C:/Libraries/SFML/lib/cmake/SFML")
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(FPSOnline sfml-audio sfml-network sfml-graphics sfml-window sfml-system)
endif()