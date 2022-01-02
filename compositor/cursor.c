#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_cursor.h"

void
sc_compositor_setup_cursor(struct sc_compositor *compositor)
{
	LOG("[sc_compositor_setup_cursor]\n");

	compositor->cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(compositor->cursor,
									compositor->output_layout);

	compositor->cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	wlr_xcursor_manager_load(compositor->cursor_mgr, 1);
	wlr_xcursor_manager_set_cursor_image(compositor->cursor_mgr, "left_ptr",
										 compositor->cursor);

//	wlAddListener(&state->cursor->events.motion, self, @selector(onMotion:));
//	wlAddListener(&state->cursor->events.motion_absolute, self,
//				  @selector(onMotionAbsolute:));
//	wlAddListener(&state->cursor->events.button, self, @selector(onButton:));
//	wlAddListener(&state->cursor->events.axis, self, @selector(onAxis:));
//	wlAddListener(&state->cursor->events.frame, self,
//				  @selector(onCursorFrame:));
//
}
