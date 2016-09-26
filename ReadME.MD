# Teletext

### Instructions

Run Main.exe to launch the teletext system.
The SDL 2 running time libraries are included to make the executable work.

### Manual Installation
To manually install teletext, you need to download and install the [SDL 2.0 library](https://www.libsdl.org/download-2.0.php), along with the [GCC](https://sourceforge.net/projects/mingw/files/latest/download?source=files). In addition, you need the [SDL 2.0 TTF library](https://www.libsdl.org/projects/SDL_ttf/). This [tutorial](http://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvsnet2010u/index.php) by Lazy Foo shows you how to setup everything.

### Controls

* h -------------- show / hide keyboard

f -------------- fullscreen / windowed mode

< -------------- move left

> -------------- move right

del ------------ delete a control code
   
The .m7 files are used as command line arguments to print images in the teletext. To do this, cd to the file directory, and type "main.exe panda.m7" to run the image of a panda.
