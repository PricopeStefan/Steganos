# Steganos
### An attempt at an all-in-one steganography project using multimedia covers.

[![Build Status](https://pricope-stefan.com/jenkins/buildStatus/icon?job=Steganos)](https://pricope-stefan.com/jenkins/job/Steganos/)

You may find more information regarding the results of the application along with the thesis that goes more in-depth about the theory behind the project on [my website.](https://pricope-stefan.com/thesis/ "Link to thesis overview")

___
## Used libraries
Many thanks go to these special projects that I have used in Steganos to make it better.
* [Lodepng](https://github.com/lvandeve/lodepng "Lodepng") by lvandeve
* [cxxopts](https://github.com/jarro2783/cxxopts/ "Cxxopts") by jarro2783
* [Robot36](https://github.com/xdsopl/robot36/ "Robot36") by xdsopl

___
## Requirements
Any OS should be fine in theory as long as it can install the following requirements:
* cmake >=3.8
* A C++ compiler that supports the C++17 standard (preferably latest version of gcc/g++ on Linux and anything newer than MSVC2015 on Windows devices)

___
## Building the application

Clone this repository on your computer. 

```git clone https://github.com/PricopeStefan/Steganos```
### On Linux
Just run these commands
```bash
~ $ cd Steganos
~/Steganos $ mkdir build && cd build
~/Steganos/build $ cmake ..
~/Steganos/build $ cmake --build . # could also simply run make
```

### On Windows
1. If you have Visual Studio installed, open Developer Command Prompt for VS. Otherwise open a normal command prompt (be sure to have cmake included in your path)
2. Run these commands
```cmd
D:> cd Steganos
D:/Steganos> mkdir build
D:/Steganos/build> cd build
D:/Steganos/build> cmake -G "Visual Studio 16 2019" -A Win32 -S ..
D:/Steganos/build> cmake --build . --config Release
```
___
## Usage examples
After you have successfully compiled Steganos, you are now ready to encode/decode secret messages inside cover files.

For **encoding** you must always have the cover flag and the secret flag in the command.
For **decoding** you must specify only the cover.
```bash
# Using long arguments for encoding
$ ./Steganos --cover cover.png --secret secret.txt
Done.

# Shortened
$ ./Steganos -c cover.png -s secret.txt
Done.

# Verbose and custom output file
$ ./Steganos -c cover.bmp -s secret.txt -o super_secret_cover.png -v
BMP Image data is found beginning at 54
Ignoring 0 bytes to get to the actual image data
Available methods:
SEQUENTIAL (default)
PERSONAL_SCRAMBLE
No encoding method specified! Picking default option.
Deleted image data and its associated stream
Done.

# You can get list of available options for a pair 
# of cover-secret files by adding the -h flag.
$ ./Steganos -c cover.bmp -s secret.ppm -h
Available steganography methods when using BMP files as cover:
SEQUENTIAL (DEFAULT)
PERSONAL_SCRAMBLE
Special option because secret file is .ppm : SSTV (and decoded via SEQUENTIAL)

# after knowing the available methods, we can use either one
$ ./Steganos -c cover.bmp -s secret.ppm -m sstv
Done.

# to decode, you must only have the cover flag 
# enabled and you can specify the mode
$ ./Steganos -c suspicious_image.bmp -m sequential
Done.
# result is now in output.bin (default output file name)

# in the case of the personal_scramble algorithm
# you can also specify a password using -p
$ ./Steganos -c cover.bmp -s secret.txt -m personal_scramble -p super_secret_key
Done.

# decoding it with the wrong password will work 
# but it will produce random invalid data
$ ./Steganos -c suspicious_image.bmp -m personal_scramble -p invalid_key
Done.
$ file output.bin
output.bin:data
# using the right password will get us the right file
$ ./Steganos -c suspicious_image.bmp -m personal_scramble -p super_secret_key
Done.
$ file output.bin
output.bin:ASCII text, with no line terminators
$ mv output.bin message.txt
$ cat message.txt
Hello world!
```

___
## Possible issues

1. Sometimes installing cmake via the apt package manager will install a version <3.8 which will not work with Steganos. Possible fixes I have found if this happens:
  * Download CMake directly from [source]('https://cmake.org/download/').  [Harder but less overhead]
  * If you have python3 installed, they have a more recent CMake version available. Running the commands below will install cmake on the system. [Easier but more used space]
  ```bash
  # if python3 is not installed
  $ sudo apt -y update && sudo apt -y upgrade
  ...
  $ sudo apt -y install python3 && sudo apt -y install python3-pip
  ...
  # after python3 is installed
  $ sudo pip3 install cmake
  ``` 
2. Please submit any issues you find on GitHub so I can check them out.
___
## Further work
* In the near future I will try to configure Jenkins such that it will also build and compile the Steganos binaries.
* More supported file formats (mainly JPEG).
* Some code refactorization.
