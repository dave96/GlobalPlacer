#include "visuals.h"

#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PX_PER_POSITION 20
#define PX_MARGIN 2

// Note: This code is verbatim from cairo's examples! I did not write it,
// nor do I claim to, but it is just for the rendering of the project.
cairo_surface_t *cairo_create_x11_surface0(int x, int y)
{
    Display *dsp;
    Drawable da;
    int screen;
    cairo_surface_t *sfc;

    if ((dsp = XOpenDisplay(NULL)) == NULL)
        exit(1);
    screen = DefaultScreen(dsp);
    da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp),
        0, 0, x, y, 0, 0, 0);
	// XSetWindowAttributes winattr;
	// winattr.override_redirect = 1;
	// XChangeWindowAttributes(dsp, da, CWOverrideRedirect, &winattr);

    XSelectInput(dsp, da, StructureNotifyMask | ExposureMask | KeyPressMask);
    XMapWindow(dsp, da);

    sfc = cairo_xlib_surface_create(dsp, da,
        DefaultVisual(dsp, screen), x, y);
    cairo_xlib_surface_set_size(sfc, x, y);

    return sfc;
}

void paint(cairo_t *cr, graph_t *graph, placement_t *place) {
	// Paint in white
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);

	double *xcoords = &place->coords[0];
	double *ycoords = &place->coords[graph->vertices];

	// Paint vertices as rectangles
	cairo_set_source_rgb(cr, 0.258, 0.529, 0.960);
	cairo_set_line_width(cr, 0);

	for (int i = 0; i < graph->vertices; ++i) {
		cairo_rectangle(cr, xcoords[i] * PX_PER_POSITION + PX_MARGIN, ycoords[i] * PX_PER_POSITION + PX_MARGIN, PX_PER_POSITION - 2*PX_MARGIN, PX_PER_POSITION - 2*PX_MARGIN);
		cairo_stroke_preserve(cr);
		cairo_fill(cr);
	}

	// Paint edges as lines
	cairo_set_source_rgb(cr, 1, 0.25, 0.25);
	cairo_set_line_width(cr, .2);

	for (int i = 0; i < graph->vertices; ++i) {
		// Start from the vertex center
		double xcenter = xcoords[i] * PX_PER_POSITION + (PX_PER_POSITION / 2);
		double ycenter = ycoords[i] * PX_PER_POSITION + (PX_PER_POSITION / 2);

		// Travel the adjacency matrix
		for (int j = i + 1; j < graph->vertices; ++j) {
			if (graph->laplacian[i * graph->vertices + j]) {
				cairo_move_to(cr, xcenter, ycenter);

				// Find J's center.
				double jxc = xcoords[j] * PX_PER_POSITION + (PX_PER_POSITION / 2);
				double jyc = ycoords[j] * PX_PER_POSITION + (PX_PER_POSITION / 2);

				// Use a "manhattan" line
				// cairo_line_to(cr, jxc, ycenter);
				cairo_line_to(cr, jxc, jyc);

				// Stroke
				cairo_stroke(cr);
			}
		}
	}

}

int cairo_check_event(cairo_surface_t *sfc, int block, cairo_t *cr, graph_t *graph, placement_t *place)
{
    char keybuf[8];
    KeySym key;
    XEvent e;

    for (;;)
    {
        if (block || XPending(cairo_xlib_surface_get_display(sfc)))
            XNextEvent(cairo_xlib_surface_get_display(sfc), &e);
        else
            return 0;

        switch (e.type)
        {
            case ButtonPress:
                return -e.xbutton.button;
            case KeyPress:
                XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                return key;
			case Expose:
				paint(cr, graph, place);
				break;
            default:
				break;
                // fprintf(stderr, "Dropping unhandled XEvent.type = %d.\n", e.type);
        }
    }
}

void cairo_close_x11_surface(cairo_surface_t *sfc)
{
   Display *dsp = cairo_xlib_surface_get_display(sfc);

   cairo_surface_destroy(sfc);
   XCloseDisplay(dsp);
}

void visuals_render(graph_t *graph, placement_t *place, int rows, int cols) {
	// cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 240, 80);
	cairo_surface_t *surface = cairo_create_x11_surface0(PX_PER_POSITION * rows, PX_PER_POSITION * cols);
	cairo_t *cr = cairo_create (surface);

	int k;
	while ((k = cairo_check_event(surface, 1, cr, graph, place)) != 65307) {
		printf("%d\n", k);
	}

	cairo_destroy (cr);
	// cairo_surface_write_to_png (surface, "hello.png");
	cairo_close_x11_surface (surface);
}

void visuals_png(graph_t *graph, placement_t *place, int rows, int cols, const char *name) {
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, PX_PER_POSITION * rows, PX_PER_POSITION * cols);
	// cairo_surface_t *surface = cairo_create_x11_surface0(PX_PER_POSITION * rows, PX_PER_POSITION * cols);
	cairo_t *cr = cairo_create(surface);

	paint(cr, graph, place);

	cairo_destroy(cr);
	cairo_surface_write_to_png(surface, name);
	cairo_surface_destroy(surface);
}