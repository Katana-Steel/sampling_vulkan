# sampling_vulkan

to build ensure you have vulkan SDK install
then it should be as simple as:
* g++ -c dev_enum/main.cpp
* g++ -o devEnum main.o -lvulkan
* ./devEnum
or
* mkdir build
* cd build; cmake ../
* make
* ./devEnum
have fun...