#include <libndls.h>

#define TIMER 0x900D0000

unsigned timer_ctl_bkp[2], timer_load_bkp[2];

void timer_init(unsigned timer)
{
	/* Timer configuration :
		- timers not freezed
		- timers decreasing
		- timers count to 0 and stop until a new value is loaded
	*/
	if (has_colors)
	{
		volatile unsigned* timer_ctl = (unsigned*)(TIMER + 0x08 + 0x20 * timer);
		volatile unsigned* timer_load = (unsigned*)(TIMER + 0x20 * timer);

		timer_ctl_bkp[timer] = *timer_ctl;
		timer_load_bkp[timer] = *timer_load;

		*timer_ctl &= ~(1 << 7);
		*timer_ctl = 0b01100011;
		*timer_ctl |= (1 << 7);
	}
	else
	{
		volatile unsigned* timer_ctl = (unsigned*)(TIMER + 0x08 + 0x0C * timer);
		volatile unsigned* timer_divider = (unsigned*)(TIMER + 0x04 + 0x0C * timer);

		timer_ctl_bkp[timer] = *timer_ctl;
		timer_load_bkp[timer] = *timer_divider;

		*timer_ctl = 0;
		*timer_divider = 0;
	}
}

void timer_restore(unsigned timer)
{
	if (has_colors)
	{
		volatile unsigned* timer_ctl = (unsigned*)(TIMER + 0x08 + 0x20 * timer);
		volatile unsigned* timer_load = (unsigned*)(TIMER + 0x20 * timer);

		*timer_ctl &= ~(1 << 7);
		*timer_ctl = timer_ctl_bkp[timer] & ~(1 << 7);
		*timer_load = timer_load_bkp[timer];
		*timer_ctl = timer_ctl_bkp[timer];
	}
	else
	{
		volatile unsigned* timer_ctl = (unsigned*)(TIMER + 0x08 + 0x0C * timer);
		volatile unsigned* timer_divider = (unsigned*)(TIMER + 0x04 + 0x0C * timer);
		volatile unsigned* timer_value = (unsigned*)(TIMER + 0x0C * timer);

		*timer_ctl = timer_ctl_bkp[timer];
		*timer_divider = timer_load_bkp[timer];
		*timer_value = 32;
	}
}

void timer_load(unsigned timer, unsigned value)
{
	if (has_colors)
	{
		volatile unsigned* timer_load = (unsigned*)(TIMER + 0x20 * timer);
		*timer_load = value;
	}
	else
	{
		volatile unsigned* timer_value = (unsigned*)(TIMER + 0x0C * timer);
		*timer_value = value;
	}
}

unsigned timer_read(unsigned timer)
{
	if (has_colors)
	{
		volatile unsigned* timer_value = (unsigned*)(TIMER + 0x04 + 0x20 * timer);
		return *timer_value;
	}
	else
	{
		volatile unsigned* timer_value = (unsigned*)(TIMER + 0x0C * timer);
		return *timer_value;
	}
}
