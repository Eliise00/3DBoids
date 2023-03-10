# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-src"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-build"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/tmp"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/src/boid-populate-stamp"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/src"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/src/boid-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/src/boid-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/boid-subbuild/boid-populate-prefix/src/boid-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
