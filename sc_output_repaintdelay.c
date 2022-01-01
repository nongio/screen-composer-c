#define _POSIX_C_SOURCE 200809L
#include "sc_output_repaintdelay.h"

void
sc_output_update_presentation(struct sc_output *output,
							  struct wlr_output_event_present *output_event)
{
	output->last_presentation = *output_event->when;
	output->refresh_nsec = output_event->refresh;
}

int
sc_output_get_ms_until_refresh(struct sc_output *output)
{
	// Compute predicted milliseconds until the next refresh. It's used for
	// delaying both output rendering and surface frame callbacks.
	int msec_until_refresh = 0;

	if (output->max_render_time != 0) {
		struct timespec now;
		clockid_t presentation_clock =
			wlr_backend_get_presentation_clock(output->compositor->wlr_backend);
		clock_gettime(presentation_clock, &now);

		const long NSEC_IN_SECONDS = 1000000000;
		struct timespec predicted_refresh = output->last_presentation;
		predicted_refresh.tv_nsec += output->refresh_nsec % NSEC_IN_SECONDS;
		predicted_refresh.tv_sec += output->refresh_nsec / NSEC_IN_SECONDS;
		if (predicted_refresh.tv_nsec >= NSEC_IN_SECONDS) {
			predicted_refresh.tv_sec += 1;
			predicted_refresh.tv_nsec -= NSEC_IN_SECONDS;
		}

		// If the predicted refresh time is before the current time then
		// there's no point in delaying.
		//
		// We only check tv_sec because if the predicted refresh time is less
		// than a second before the current time, then msec_until_refresh will
		// end up slightly below zero, which will effectively disable the delay
		// without potential disastrous negative overflows that could occur if
		// tv_sec was not checked.
		if (predicted_refresh.tv_sec >= now.tv_sec) {
			long nsec_until_refresh = (predicted_refresh.tv_sec - now.tv_sec) * NSEC_IN_SECONDS +
									  (predicted_refresh.tv_nsec - now.tv_nsec);

			// We want msec_until_refresh to be conservative, that is, floored.
			// If we have 7.9 msec until refresh, we better compute the delay
			// as if we had only 7 msec, so that we don't accidentally delay
			// more than necessary and miss a frame.
			msec_until_refresh = nsec_until_refresh / 1000000;
		}
	}

	return msec_until_refresh - output->max_render_time;
}
