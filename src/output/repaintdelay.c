#define _POSIX_C_SOURCE 200809L

#include "log.h"
#include "sc_output.h"
#include "sc_output_repaintdelay.h"
#include "utils.h"

void
sc_output_update_presentation(struct sc_output *output,
							  struct wlr_output_event_present *output_event)
{
	// timespec of when the last output refresh did happen
	output->last_presentation = *output_event->when;
	output->last_presentation_d = timespec_to_double(output->last_presentation);
	// how many nano seconds are expected before next output refresh will happen
	output->next_presentation = *output_event->when;
	output->next_presentation.tv_nsec += output_event->refresh;
	output->next_presentation_d = timespec_to_double(output->next_presentation);
}

int
sc_output_get_ms_until_refresh(struct sc_output *output)
{
	// Compute predicted milliseconds until the next refresh. It's used for
	// delaying both output rendering and surface frame callbacks.
	int msec_until_refresh = 0;

	struct timespec now;
	clockid_t presentation_clock =
		wlr_backend_get_presentation_clock(output->compositor->wlr_backend);
	clock_gettime(presentation_clock, &now);

	double now_d = timespec_to_double(now);

	//LOG("last: %f\n", output->last_presentation_d);
	//LOG("next: %f\n", output->next_presentation_d);
	//LOG("diff: %f\n", output->next_presentation_d - output->last_presentation_d);

	// If the predicted refresh time is before the current time then
	// there's no point in delaying.
	//
	// We only check tv_sec because if the predicted refresh time is less
	// than a second before the current time, then msec_until_refresh will
	// end up slightly below zero, which will effectively disable the delay
	// without potential disastrous negative overflows that could occur if
	// tv_sec was not checked.
	if(output->next_presentation_d >= now_d) {

		// We want msec_until_refresh to be conservative, that is, floored.
		// If we have 7.9 msec until refresh, we better compute the delay
		// as if we had only 7 msec, so that we don't accidentally delay
		// more than necessary and miss a frame.
		msec_until_refresh = ((output->next_presentation_d - now_d) * 1000.0);
	}

	return msec_until_refresh;
}
