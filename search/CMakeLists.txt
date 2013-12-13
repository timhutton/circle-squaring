cmake_minimum_required( VERSION 2.6 )
cmake_policy( SET CMP0003 NEW )

project( csq )

add_executable( csq
    src/main.cpp
	src/utils.h       src/utils.cpp
	src/point2d.h     src/point2d.cpp
	src/polygon.h     src/polygon.cpp
	src/transform.h   src/transform.cpp
	src/piece.h	      src/piece.cpp
	src/world.h	      src/world.cpp
)