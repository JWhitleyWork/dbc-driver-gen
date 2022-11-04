# dbc-driver-gen
A C++ application for generating basic drivers and ROS2 wrappers from DBC files.

## Dependencies
This project depends on [can_dbc_parser](https://github.com/JWhitleyWork/can_dbc_parser).
Install `can_dbc_parser` according to the instructions in the readme.

## Build / Installation Instructions
From the repository folder

```
mkdir build && cd build
cmake ..
sudo make install
```

## Uninstall Instructions
From the repository folder

```
cd build
sudo make uninstall
```

## Usage Instructions
Once installed, the binaries `dbc-driver` and `dbc-ros2-driver` should become available in your `$PATH`.
For usage instructions, run `dbc-driver --help` or `dbc-ros2-driver --help`.
