# sampling_vulkan

to build ensure you have vulkan SDK install
then it should be as simple as:
* g++ -c dev_enum/main.cpp
* g++ -o devEnum main.o -lvulkan
* ./devEnum

or with cmake
* mkdir build
* cd build
* cmake ../
* make
* dev_enum/devEnum

have fun...