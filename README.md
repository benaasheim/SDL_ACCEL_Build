# SDL_ACCEL_Build
=========

A real-time raytracer for Linux and Apple systems.

*Warning: development in progress*

Usage
-----

*usage instructions will be provided once there is something to use*


Dependencies
------------

#### Runtime

+ SDL2
+ SDL2_image

#### Build

+ gcc 5.3 (or other suitable C11 compiler)
+ CMake
+ gcc or clang autovectorization ("-ftree-autovectorize")


Building
--------

First, ensure that you have installed all listed dependencies. They should be available from your distributions package manager.

To build the project, create a `build/` directory, run CMake, and build with CMake specifying a source directory: `-Dsrc=<src_dir_name` 

```sh
$ mkdir build
$ cd build/
$ cmake .. -Dsrc=src/
$ cmake --build ./
```

License
-------

GNU GPL v2
