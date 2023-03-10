# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-src"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-build"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/tmp"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/src/p6-populate-stamp"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/src"
  "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/src/p6-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/src/p6-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Documents/IMAC2/S4/Projet_OpenGL_p6/3DBoids/cmake-build-debug/_deps/p6-subbuild/p6-populate-prefix/src/p6-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
