#ifndef TIMER_H
#define TIMER_H

// Note from matrefeytontias:
// Everything on HW timers is by aeTIos and Streetwalrus from http://www.omnimaga.org/
// 
// Note from RedHat:
// This file is also sourced from n2DLib. So I'll drop the link to the original file below.
// 
// https://github.com/n2DLib/n2DLib/blob/master/n2DLib.h
//
// Should the README for this app ever be removed, I'll include the docs for the timers below

#define TIMER_1S 32768

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Hardware timers
	 *
	 * The TI-Nspire calculators have 2 hardware timers that allows for accurate timing of any task.
	 * Each timer runs at 32 kHz. n2DLib provides a precise use for them, which is as follows :
	 *
	 * Every 0.000030517578125 second(roughly 30 µs, that is 32 768 times per second), each timer's
	 * value is decreased by 1.
	 *
	 * When a timer reaches 0, it stops decreasing until you reload a value in it.To sum up, when a
	 * timer reads 0, it means the amount of time you passed has run out.
	 */

	/**
	 * Initializes the first timer (unsigned timer = 0) or the second timer (unsigned timer = 1).
	 * Any other parameter than 0 or 1 will crash your calc badly. This MUST be called ONCE for
	 * every timer you use in your program or your timing will be invalid.
	 */
	extern void timer_init(unsigned);

	/**
	 * Restores the initial state of a timer. You MUST call this for every timer that you initialized
	 * via timer_init.
	 */
	extern void timer_restore(unsigned);

	/**
	 * Loads a value into a timer. This value will be decremented by the timer each tick until it
	 * reaches 0. Note that a timer will freeze until its value is non-zero.
	 */
	extern void timer_load(unsigned, unsigned);

	/**
	 * Reads a value from a timer. If it reads 0, it will always read as 0 until you load a new
	 * value into the timer.
	 */
	extern unsigned timer_read(unsigned);

#ifdef __cplusplus
}
#endif

#endif // TIMER_H
