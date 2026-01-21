cmake_minimum_required(VERSION 3.16)

project(VoltorbFlip LANGUAGES CXX)

#first as it is requirement for the others
add_subdirectory(VOF-Network)

if(ANDROID) #needed for github workflow, may never be filled

else() #desktop
    add_subdirectory(VOF-Client-Desktop)
    add_subdirectory(VOF-Server-CLI)
endif()
