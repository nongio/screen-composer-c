#ifndef _SC_OUTPUT_REPAINTDELAY_H
#define _SC_OUTPUT_REPAINTDELAY_H

struct sc_output;
struct wlr_output_event_present;

void
sc_output_update_presentation(struct sc_output *output,
							  struct wlr_output_event_present *output_event);

int sc_output_get_ms_until_refresh(struct sc_output *output);

#endif
