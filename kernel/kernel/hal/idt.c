/**
 *
 * Copyright 2020 David Aylaian
 * https://github.com/davidaylaian/carbon/
 * Licensed under the Apache License 2.0
 *
 */

#include <attribute.h>
#include <stddef.h>
#include <stdint.h>
#include <terminal.h>
#include <hal.h>

#define IDT_MAX_INTERRUPTS 256

// describes an interrupt descriptor
struct idt_descriptor {
	uint16_t base_lo;
	uint16_t selector;
	uint8_t  reserved;
	uint8_t  flags;
	uint16_t base_hi;
} packed;

// describes the structure for the idtr register
struct idt_idtr {
	uint16_t limit;
	uint32_t base;
} packed;

static struct idt_descriptor idt[IDT_MAX_INTERRUPTS];
struct idt_idtr idt_idtr;

// TODO: replace with printfs

// if this function is ever called, then something has gone terribly wrong
static noreturn void default_handler()
{
	terminal_print("\n\n");
	terminal_print("              +==================================================+              ");
	terminal_print("              |  ***  default_handler: Unhandled Exception  ***  |              ");
	terminal_print("              |  ***           Execution halted.            ***  |              ");
	terminal_print("              +==================================================+              ");
	terminal_print("\n\n");

	disable();
	while (1);
}

// sets an interrupt vector, with internal options flags and sel
static inline void idt_setvect_internal(size_t i, size_t base, uint8_t flags, uint16_t sel)
{
	if (i <= IDT_MAX_INTERRUPTS)
	{
		idt[i].base_lo	= base & 0xFFFF;
		idt[i].base_hi	= (base >> 16) & 0xFFFF;

		idt[i].reserved	= 0;
		idt[i].flags	= flags;
		idt[i].selector	= sel;
	}
}

// sets an interrupt vector
void idt_setvect(size_t intn, size_t handler)
{
	idt_setvect_internal(intn, handler, 0x8E, 0x08);
}

// install the idt
void idt_install()
{
	// set up idtr
	idt_idtr.limit = sizeof(struct idt_descriptor) * IDT_MAX_INTERRUPTS - 1;
	idt_idtr.base  = (uint32_t) &idt[0];

	// register default handlers
	for (size_t i = 0; i < IDT_MAX_INTERRUPTS; i++)
	{
		idt_setvect(i, (size_t) default_handler);
	}

	// load idt instruction
	asm("lidt [idt_idtr]");
}
