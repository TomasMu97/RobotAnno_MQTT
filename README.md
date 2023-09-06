# Table of content
- [Table of content](#table-of-content)
- [RobotAnno](#robotanno)
- [Rbtanno spike-based controller](#rbtanno-spike-based-controller)
- [Repository folders](#repository-folders)
- [Mosquitto and dependences cross-compilation](#mosquitto-and-dependences-cross-compilation)

# RobotAnno
This repository contains the source code for a spike-based controller for the RobotAnno robotics platform. This controller has been implemented for hardware using VHDL rtl languaje. This controller module has been tested and deployed on a Zynq 7100 Xilinx platform called Zynq MMP board. The Rbtanno controller module can be configured from an user application through an AXI-4 module.

# Rbtanno spike-based controller


# Repository folders
- app: This folder constins the code of the user application that interacts with the Rbtanno controller. This app configures all the Rbtanno controller registers, sends action commands and check the controller state.
  - src: API source code of the Rbtanno controller.
  - test: Source code for different Rbtanno controller applications.
- rtl: Hardware implementation for Rbtanno controller.
  - src: VHDL source code for AXI-4 configuration module and for Rbtanno spike-based controller.
  - tb: Testbenches for hardware modules.
  - docs: Documentation for the AXI-4 configuration module.
  - vivado: Scripts to create the vivado project that includes all components to be synthesized for the Zynq MMP platform.
  - petalinux: BSP that containts the OS to be deployed on the Zynq MMP platform.
- mmp_board_docs: Documentation about the Zynq MMP platform.

# Mosquitto and dependences cross-compilation
This version of RbtAnno runs as MQTT client in the zynq platform. All dependences have to be cross-compiled. For the cross-compilation it will be used the arm-linux-gnueabihf-g++-8 compiler.
- Install the compiler using the following commands:
  ```
  sudo apt-get install g++-8-arm-linux-gnueabihf
  sudo apt-get install gcc-8-arm-linux-gnueabihf
  ```
- cJSON: Download the latest release from https://github.com/DaveGamble/cJSON/releases and perfom the following steps:
  - Unzip the cJSON release file (cJSON-X.X.X.zip) where X.X.X is the software version.
  - Create a folders inside the unzipped cJSON folder where the compilation and the installation will be done.
    ```
    mkdir build
    mkdir build/install
    ```
  - Go to the new build folder path
    ```
    cd build
    ```
  - Run the following commands to generate de cmake project, compile and install the software.
    ```
    cmake -D CMAKE_C_COMPILER=arm-linux-gnueabihf-gcc-8 -D CMAKE_INSTALL_PREFIX=./install ..
    make
    make install
    ```
  - The resulting library files are in bulid/install folder.

- Mosquitto: Download the latest tag from https://github.com/eclipse/mosquitto/tags and run the following steps:
  - Unzip the Monqutio tad file (mosquitto-X.X.X.zip) where X.X.X is the software version.
  - Create a folders inside the unzipped Mosquitto folder where the compilation and the installation will be done.
    ```
    mkdir build
    mkdir build/install
    ```
  - Go to the new build folder path
    ```
    cd build
    ```
  - Modify the CMakeLists.txt file according your need. For this compilation the compiling options are the following:
    ```
    option(WITH_BUNDLED_DEPS "Build with bundled dependencies?" ON)
    option(WITH_TLS "Include SSL/TLS support?" OFF)
    option(WITH_TLS_PSK "Include TLS-PSK support (requires WITH_TLS)?" OFF)
    option(WITH_EC "Include Elliptic Curve support (requires WITH_TLS)?" OFF)
    option(WITH_UNIX_SOCKETS "Include Unix Domain Socket support?" ON)
    option(WITH_SOCKS "Include SOCKS5 support?" ON)
    option(WITH_SRV "Include SRV lookup support?" OFF)
    option(WITH_STATIC_LIBRARIES "Build static versions of the libmosquitto/pp libraries?" ON)
    option(WITH_PIC "Build the static library with PIC (Position Independent Code) enabled archives?" OFF)
    option(WITH_THREADING "Include client library threading support?" OFF)
    option(WITH_DLT "Include DLT support?" OFF)
    option(WITH_CJSON "Build with cJSON support (required for dynamic security plugin and useful for mosquitto_sub)?" ON)
    option(WITH_CLIENTS "Build clients?" ON)
    option(WITH_BROKER "Build broker?" OFF)
    option(WITH_APPS "Build apps?" ON)
    option(WITH_PLUGINS "Build plugins?" ON)
    option(DOCUMENTATION "Build documentation?" OFF)
    ```
  - Run the following commands to generate de cmake project, compile and install the software.
    ```
    cmake -D CMAKE_C_COMPILER=arm-linux-gnueabihf-gcc-8 -D CMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++-8 -DCMAKE_INSTALL_PREFIX=./install -D CJSON_INCLUDE_DIR=path/to/cJSON-X.X.X/build/install/include/ -D CJSON_LIBRARY=path/to/cJSON-X.X.X/build/install/lib/libcjson.so ..
    make
    make install
    ```
  - The resulting library files are in bulid/install folder.

# RobotAnno mqtt app compilation
Once the dependencies have been compiled successfully, let's move to compile the rbtanno mqtt cliente:
- Move to the app folder
  ```
  cd app
  ```
- Compile the software using make
  ```
  make
  ```
The resulting software has to be generated. Copy this app in the zynq platform and launch it.
