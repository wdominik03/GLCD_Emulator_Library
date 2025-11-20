struct emu_state
{
    long tick;
    int width;
    int height;
    int scale;
    int framebuffer[131072]; //Preallocating memory, for a maximum size of 1024x1024
};

struct emu_state state;
void (*on_tick_function)();

void clear_framebuffer();
int setup_window(int width, int height, int scale, int refresh_rate);
