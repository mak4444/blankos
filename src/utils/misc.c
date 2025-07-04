// Miscellaneous small programs
// Author: xamidev
// Licensed under the Unlicense. See the repo below.
// https://github.com/xamidev/blankos

#include "../libc/stdio.h"
#include "../kernel/system.h"
#include "../libc/string.h"
#include "../drivers/framebuffer.h"
#include "../drivers/ata.h"
#include "../drivers/rtc.h"
#include "../kernel/io.h"
#include "../drivers/pci.h"
#include "../kernel/initrd.h"
#include "../kernel/kmain.h"

// Print a rainbow colorful text for testing

#define BUF_SIZE 256
#define COLORS 20

void program_rainbow(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <string>\n", argv[0]);
		return;
	}
	
	char input_buffer[BUF_SIZE] = {0};
	for (int i=1; i<argc; i++)
	{
		strcat(input_buffer, argv[i]);
		if (i<argc-1)
		{
			strcat(input_buffer, " ");
		}
	}

	enum Colors colors[] = {
        	white, black, red, green, blue, yellow,
        	cyan, magenta, orange, purple, brown,
        	gray, pink, lime, navy, teal, maroon,
        	olive, silver, gold, indigo, violet,
        	coral, turquoise, salmon, chocolate,
        	khaki, lavender, beige
    	};
	int colors_count = sizeof(colors)/sizeof(colors[0]);

	for (int i=0; i<colors_count-1; i++)
	{
		colorputs(input_buffer, colors[i], colors[i+1]);
		puts("\n");
	}	
}

// Clear the terminal

#define ROWS 25

void program_clear()
{	
	clear();
}

// Get uptime in ticks

void program_uptime()
{
	int ticks = uptime();
	double seconds = ticks/18.2065; // PIT channel 0 freq
	printf("%d ticks\t%f seconds\n", ticks, seconds);
}

// Get help

void program_help()
{
	printf("help\tpanic\twords\tprimes\trainbow\tclear\nmath\tbf\t   uptime   echo\t  sysinfo\tconway\nrot13   morse\tcowsay   time\t  read\t   reboot\npi\t  ls\t   cat\t  bmp\t   lspci\t  naval\nsnake   exec\t uhex\t forth\n");
}

// Panic

void program_panic()
{
	asm volatile("int $0x13");
}

// Output input

void program_echo(int argc, char* argv[])
{
	for (int i=1; i<argc; i++)
	{
		puts(argv[i]);
		if (i < argc-1) {
			putc(' ');
		}
	}
	puts("\n");
}

// Get current RTC time

void program_time()
{
	rtc_time_t time;
	rtc_read_time(&time);
	puts("Current RTC time: ");
	print_time(&time);
	puts("\n");
}

// Read a sector

void program_read(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <sector>\n", argv[0]);
	} else if (argc == 2)
	{
		uint8_t buffer[512];
		ata_read_sector(atoi(argv[1]), buffer);
		
		for (int i=0; i<512; i++)
		{
			if (i%50==0) puts("\n"); // hardcoded = bad
			printf("%02x ", buffer[i]);
		}
		puts("\n");
	} else
	{
		puts("Invalid argument number\n");
	}
}

// Reboots the machine (might just shutdown) (or do nothing if youre lucky)

void program_reboot()
{
	puts("Rebooting...\n");
	
	while(inb(0x64) & 0x02);
	outb(0x64, 0xFE);

	while (1) asm volatile("hlt");
}

// List PCI buses and devices

void program_lspci()
{
	scan_pci_bus();
}

// Executes binary file

void program_exec(int argc, char* argv[])
{
	if (argc < 2)
	{
		puts("Usage: exec <binary>\n");
		return;
	} 
	void* binary_file = load_file_from_initrd((uint8_t*)initrd_addr, argv[1]); 

	if (binary_file == NULL)
	{
		printf("[exec] Failed to load program '%s'.\n", argv[1]);
		return;
	} 

	void (*program_entry)() = (void (*)())binary_file;
	program_entry();
}
