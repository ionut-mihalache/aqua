## Introduction ##

DSP (Driver Service Protocol) relies on AQUA for interoperability. AQUA is used for local IPC and is based on shared memory.

## Building ##

__NOTE__: All the commands below should be used in the root directory.

### Linux ###

#### Production ####

```
$ rm -rf build
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build -j<proc-number>
```

#### Development ####

```
$ rm -rf build
$ cmake -B build -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build -j<proc-number>
```

### Windows ###


#### Production ####

```
$ rm -rf build
$ cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
$ cmake --build build -j<proc-number>
```

#### Development ####

```
$ rm -rf build
$ cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build -j<proc-number>
```


## Usage ##

### Linux ###

```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<absolute-path-to-build-lib-directory-after-cmake-build> # for dynamic library
```

### Windows ###

TODO
