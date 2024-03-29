# ---Choose CMake version---
cmake_minimum_required(VERSION 3.20)

# ---Choose project name---
project(OpenGL-TPs-Template)

#TEST
# ---Choose C++ version---
set(CMAKE_CXX_STANDARD 11)


# ---Add Boid library---
include(FetchContent)
FetchContent_Declare(
        Boid
    GIT_REPOSITORY https://github.com/julesfouchy/p6
    GIT_TAG b9e6366e1198abdd7bc835e6125f9ff5b89a75b8
)
FetchContent_MakeAvailable(Boid)

#TEST
# ---Add Tinyobj library---
add_subdirectory(tinyobjloader)

# ---Create a target for each exercise---
function(add_exercise EXERCISE_NAME)
    set(TARGET_NAME ${EXERCISE_NAME}_exe)
    add_executable(${TARGET_NAME})

    # ---Choose C++ version---
    target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

    # ---Choose warning level---
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors)
    endif()



    # ---Declare source files---
    file(GLOB_RECURSE EXERCISE_FILES CONFIGURE_DEPENDS ${EXERCISE_NAME}/*)
    file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS src-common/*)
    target_sources(${TARGET_NAME} PRIVATE ${EXERCISE_FILES} ${COMMON_FILES})
    target_include_directories(${TARGET_NAME} PRIVATE ${EXERCISE_FILES} src-common)

    # ---Link Boid library---
    target_link_libraries(${TARGET_NAME} PRIVATE p6::p6)

    #TEST
    # ---Link Tinyobj library---
    target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader)


    # ---Set the output folder---
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${EXERCISE_NAME})

    # ---Copy the assets and the shaders to the output folder (where the executable is created)---
    Cool__target_copy_folder(${TARGET_NAME} assets/models)
    Cool__target_copy_folder(${TARGET_NAME} ${EXERCISE_NAME}/shaders shaders)

    p6_copy_folder(${TARGET_NAME} assets/models)
endfunction(add_exercise)


add_exercise(TP3_ShaderColor)
add_exercise(Boid)
add_exercise(TP6)
add_exercise(TP6_MultiTexturing)
add_exercise(TP6_MultiShaders)
add_exercise(TP7)
add_exercise(TP7_FreeflyCam)
add_exercise(TP8_DirectionalLight)
add_exercise(TP8_PointLight)
add_exercise(Boid3D)
add_exercise(TestTinyObj)


