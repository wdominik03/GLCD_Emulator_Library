# GLCD Emulator Library
## Introduction
This library aims to provide a basic monochrome graphical LCD emulator accessible across Windows, MacOS and Linux. It uses GTK4 to render a one-shot or continously updating scalable preview window.
## GTK
This library uses GTK 4, to be installed separately. If you do not have GTK 4 installed, please follow the follwing guide: [GTK Installation Guide](https://www.gtk.org/docs/installations/)
## Display data structure
The library emulates a monochrome display with an 8-bit wide parallel interface.

The bits of each transmitted byte are equated to 8 pixel on-off signals with the pixels located vertically under eachother. The top-most pixel is toggled by the LSB.

Pixel infromation is located in a framebuffer, represented by an array of integers. This framebuffer is preallocated inside the library file for a screen up-to 1024x1024 pixels. The creation of larger displays is prevented by the library automatically. Framebuffer information is rendered left to right, top to bottom.
## Example
File: ```example.c```
```c
#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>
#include "glcd_emu.h"

int x = 0;
int y = 0;
int vel_x = 1;
int vel_y = 1;


void tick() {
    clear_framebuffer();

    if (x == 0 && vel_x == -1) {
        vel_x = 1;
    } else if (x == state.width-8 && vel_x == 1) {
        vel_x = -1;
    }

    if (y == 0 && vel_y == -1) {
        vel_y = 1;
    } else if (y == state.height-8 && vel_y == 1) {
        vel_y = -1;
    }

    x += vel_x;
    y += vel_y;

    int y_top = (y/8);
    int box_top = (y - y_top*8);

    for (int x_pos = x; x_pos < x+8; x_pos++) {
        state.framebuffer[y_top*state.width+x_pos] = ~((int) pow(2, box_top)-1);
        if (box_top != 8) {
            state.framebuffer[(y_top+1)*state.width+x_pos] = pow(2, box_top)-1;
        }
    }

    state.framebuffer[0] = 1;
}

int
main (int    argc,
      char **argv)
{
    on_tick_function = tick;

    int status = setup_window(128,96,6,20);

    return status;
}
```

This example renders a "DVD screensaver" using an 8x8 box with a framerate of 20 fps. Screen size is set to 128 x 64 pixels, with a scaling factor of 6.
## Interface documentation
This is a barebones documentation for the public facing interface exposed in the ```glcd_emu.h``` header. You may try to access other functions directly from the library file, but it is not recommended.
### struct emu_state
```c
struct emu_state
{
    long tick;
    int width;
    int height;
    int scale;
    int framebuffer[131072]; //Preallocating memory, for a maximum size of 1024x1024
};
```
This is a struct designed to hold runtime state information. The fields are the following:
- ```tick```: long holding the number of ticks elapsed since the start of the display emulator. This value is incremented on each rendering, determined by the preset framerate.
- ```width```: integer value representing display width in pixels. Nominal range: ```16```-```1024```
- ```height```: integer value representing display height in pixels. Nominal range: ```16```-```1024```
- ```scale```: integer value representing scaling factor. Nominal range: ```1```-```25```
- ```framebuffer```: integer array containing display framebuffer. Memory is allocated for a display size of ```1024x1024```, first ```n = (height / 8) * width``` bytes are used.
### state
This instantiation of the ```emu_state``` struct is used by the emulation loop. Nominally runtime changes may only be made through this instance.
```c
struct emu_state state;
```
**Warning**: Changing display size and scaling fields currently does not update window size, therefore screens bigger than initiated will not be fully readable.
### on_tick_function
This variable contains a pointer to a user defined function executed on every tick. Runtime logic may be defined throught the function pointed to.
```c
void (*on_tick_function)();
```
The user defined function may not take any parameters, and is to have a ```void``` return.
### clear_framebuffer()
This function clears the framebuffer.
```c
void clear_framebuffer();
```
**Note**: Because of program optimization this function only clears the part of the framebuffer that is currently being displayed, therefore if less than all bytes are used, the end of the buffer is not cleared.
### setup_window()
This function sets up the emulation and starts the GTK environment.
```c
int setup_window(int width, int height, int scale, int refresh_rate);
```
- ```return```: GTK application status after exiting
- ```width```: This integer sets the width of the emulated display in pixels, limited between ```16``` and ```1024```
- ```height```: This integer sets the height of the emulated display in pixels, limited between ```16``` and ```1024```
- ```scale```: This integer sets the scale of the visualization, in multiples of pixels, limited between ```1``` and ```25```
- ```refresh_rate```: This integer sets the refresh rate of the emulated display in hertz. The ```on_tick_function``` is called and ```state.tick``` is incremented on every refresh. Limited between ```1```-```1000```
This function calls the GTK application manager under the hood, therefore it is thread-blocking. To interact with the emulator while the window is opened, use the ```on_tick_function```
