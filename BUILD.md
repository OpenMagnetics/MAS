
## Build steps

1. Create a build directory:

    ```
    $ mkdir build && cd build
    ```

2. Configure the CMake project (Using Ninja in this example):

    ```
    $ cmake .. -G "Ninja"
    ```

4. Build it:

    ```
    $ cmake --build .
    ```

5. Run the application:

    ```
    $ ./MAS_tests
    ```
6. Create Python package

    ```
    python3 -m pip install -e ../ -vvv
    ```


wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo apt install cmake
sudo apt install ninja-build
npm install -g quicktype
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 10
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 10

quicktype -l c++ -s schema ../schemas/MAS.json -S ../schemas/magnetic.json -S ../schemas/magnetic/core.json -S ../schemas/magnetic/coil.json -S ../schemas/utils.json -S ../schemas/magnetic/core/gap.json -S ../schemas/magnetic/core/shape.json -S ../schemas/magnetic/core/material.json -S ../schemas/magnetic/insulation/material.json -S ../schemas/magnetic/insulation/wireCoating.json -S ../schemas/magnetic/bobbin.json -S ../schemas/magnetic/core/piece.json -S ../schemas/magnetic/core/spacer.json -S ../schemas/magnetic/wire/basicWire.json -S ../schemas/magnetic/wire/round.json -S ../schemas/magnetic/wire/rectangular.json -S ../schemas/magnetic/wire/foil.json -S ../schemas/magnetic/wire/litz.json -S ../schemas/magnetic/wire/planar.json -S ../schemas/magnetic/wire/material.json -S ../schemas/magnetic/wire.json -S ../schemas/utils.json -S ../schemas/magnetic/insulation/wireCoating.json -S ../schemas/magnetic/insulation/material.json -S ../schemas/inputs.json -S ../schemas/outputs.json -S ../schemas/outputs/coreLossesOutput.json -S ../schemas/inputs/designRequirements.json -S ../schemas/inputs/operatingConditions.json -S ../schemas/inputs/operatingPoint.json -S ../schemas/inputs/operatingPointExcitation.json -o ../outputs/MAS.hpp --namespace OpenMagnetics --source-style single-source --type-style pascal-case --member-style underscore-case --enumerator-style upper-underscore-case --no-boost

