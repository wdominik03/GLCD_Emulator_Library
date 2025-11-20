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
}

int
main (int    argc,
      char **argv)
{
    on_tick_function = tick;

    int status = setup_window(128,96,6,20);

    return status;
}