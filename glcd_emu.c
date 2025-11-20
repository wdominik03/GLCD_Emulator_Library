#include <gtk/gtk.h>

void no_op() {};

//int emu_width = 128;
//int emu_height = 64;
//int emu_scale = 6;
//long emu_tick = 0;
void (*on_tick_function)() = no_op;

GtkWidget *canvas;

struct emu_state
{
    long tick;
    int width;
    int height;
    int scale;
    int framebuffer[131072]; //Preallocating memory, for a maximum size of 1024x1024
};

struct emu_state state;

void clear_framebuffer() {
    for (int i = 0; i < (state.width*(state.height/8)); i++) {
        state.framebuffer[i] = 0;
    }
};

static void
draw_function (GtkDrawingArea *area,
               cairo_t        *cr,
               int             width,
               int             height,
               gpointer        data)
{
    GdkRGBA color;

    for (int x = 0; x < state.width; x++) {
        for (int y = 0; y < (state.height/8); y++) {
            for (int z = 0;z < 8; z++) {
                if ((state.framebuffer[y*state.width+x] & (1 << z))) {
                    cairo_rectangle(cr,x*state.scale,(y*8+z)*state.scale,state.scale,state.scale);
                }
            }
        }
    }

  gtk_widget_get_color (GTK_WIDGET (area),
                        &color);
  gdk_cairo_set_source_rgba (cr, &color);

  cairo_fill (cr);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    GtkWidget *window;

    char window_title[80];
    sprintf(window_title, "%d x %d GLCD Emulator\0", state.width, state.height);

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), window_title);
    gtk_window_set_default_size (GTK_WINDOW (window), state.width*state.scale, state.height*state.scale);
    gtk_window_set_resizable(GTK_WINDOW (window), false);

    canvas = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (canvas), state.width*state.scale);
    gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (canvas), state.height*state.scale);
    gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (canvas), draw_function, NULL, NULL);
    gtk_window_set_child( GTK_WINDOW (window), canvas);

    gtk_window_present (GTK_WINDOW (window));
}

void tick_handler(int* paramPtr) {
    state.tick++;
    (on_tick_function)();
    gtk_widget_queue_draw(canvas);
}

int setup_window(int width, int height, int scale, int refresh_rate) {
    if (scale < 1) {
        scale = 1;
    } else if (scale > 25) {
        scale = 25;
    }

    if (width < 16) {
        width = 16;
    } else if (width > 1024) {
        width = 1024;
    }

    if (height % 8 != 0) {
        height = (height/8)*8;
    }
    if (height < 16) {
        height = 16;
    } else if (height > 1024) {
        height = 1024;
    }

    state.width = width;
    state.height = height;
    state.scale = scale;

    GtkApplication *app;
    int status;
    int test_param = 23523;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    g_timeout_add(1000/refresh_rate,G_SOURCE_FUNC (tick_handler),&test_param);
    status = g_application_run (G_APPLICATION (app), 0, 0);
    g_object_unref (app);

    return status;
}