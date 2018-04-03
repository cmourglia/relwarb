# Building

One time conan setup

```
$ conan remote add conan-community https://api.bintray.com/conan/conan-community/conan
$ conan remote add conan-transit https://conan-transit.bintray.com
```

Download required libraries

```
$ mkdir build && cd build
$ conan .. -g cmake_multi -s build_type=Release --build=missing
$ conan .. -g cmake_multi -s build_type=Debug --build=missing
```

And run cmake as usual
