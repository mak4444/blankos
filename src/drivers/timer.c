// Programmable Interval Timer channel 0 driver
// Author: xamidev
// Licensed under the Unlicense. See the repo below.
// https://github.com/xamidev/blankos

#include "../kernel/system.h"
#include "../libc/stdio.h"

volatile unsigned long global_ticks = 0;

void timer_handler()
{
	global_ticks++;
}

void timer_install()
{
	irq_install_handler(0, timer_handler);
	printf("[timer] initialized, starting g_ticks...\n");
}

void delay(int ticks)
{
	unsigned long eticks;
	eticks = global_ticks + ticks;
	while (global_ticks < eticks);
}

int uptime()
{
	return global_ticks;
}
