/******************************************************
 *  Demonstration program to read the XC4602 Jaycar Keypad
 *  on a BeagleBone Black (BBB) running QNX 6.60 that matches the lecture notes
 *
 *
 *	This version uses and ISR to clear the IRQ
 *
 *	uses InterruptAttach()
 *
 *  The Keypad uses the TTP229-BSF TonTouch Chip and can be
 *  purchased from Jaycar (XC4602). Make sure it is in 16 key
 *  mode by bridging our the two pins (pair 3 on P1 on keypad)
 *
 *  When Key press is detected a hardware interrupt will be fired
 *  IRQ 99 (top part of GPIO1, ie IO bit 28 of GPIO1). The key
 *  will be decoded and a thread will start which controls the
 *  4 LEDS on the beaglebone to switch on and off for a 100 ms
 *  duration to indicate a valid key has been pressed and decoded.
 *
 *  XC4602 pin     -> BeagleBone Black Pin
 *  VCC - VDD_3V3B -> pin P9_03 or P9_04
 *  GND - DGND     -> pin P9_01 or P9_02
 * 	SCL - GPIO1_16 -> pin P9_15
 * 	SD0 - GPIO1_28 -> pin P9_12
 *
 *  Author: Samuel Ippolito
 *  Date:   17-08-2018
 *
 *****************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <hw/inout.h>      // for in32() and out32();
#include <sys/mman.h>      // for mmap_device_io();
#include <stdint.h>        // for unit32 types
#include <sys/neutrino.h>  // for ThreadCtl( _NTO_TCTL_IO_PRIV , NULL)
#include <sched.h>
#include <sys/procmgr.h>
#include <string.h>

#define AM335X_CONTROL_MODULE_BASE   (uint64_t) 0x44E10000
#define AM335X_CONTROL_MODULE_SIZE   (size_t)   0x00001448
#define AM335X_GPIO_SIZE             (uint64_t) 0x00001000
#define AM335X_GPIO1_BASE            (size_t)   0x4804C000

#define LED0          (1<<21)   // GPIO1_21
#define LED1          (1<<22)   // GPIO1_22
#define LED2          (1<<23)   // GPIO1_23
#define LED3          (1<<24)   // GPIO1_24

#define SD0 (1<<28)  // SD0 is connected to GPIO1_28
#define SCL (1<<16)  // SCL is connected to GPIO1_16

#define GPIO_OE        0x134
#define GPIO_DATAIN    0x138
#define GPIO_DATAOUT   0x13C

#define GPIO_IRQSTATUS_SET_1 0x38   // enable interrupt generation
#define GPIO_IRQWAKEN_1      0x48   // Wakeup Enable for Interrupt Line
#define GPIO_FALLINGDETECT   0x14C  // set falling edge trigger
#define GPIO_CLEARDATAOUT    0x190  // clear data out Register
#define GPIO_IRQSTATUS_1     0x30   // clear any prior IRQs

#define GPIO1_IRQ 99  // TRG page 465 list the IRQs for the am335x

#define P9_12_pinConfig 0x878 //  conf_gpmc_ben1 (TRM pp 1364) for GPIO1_28,  P9_12

// GPMC_A1_Configuration
#define PIN_MODE_0   0x00
#define PIN_MODE_1   0x01
#define PIN_MODE_2   0x02
#define PIN_MODE_3   0x03
#define PIN_MODE_4   0x04
#define PIN_MODE_5   0x05
#define PIN_MODE_6   0x06
#define PIN_MODE_7   0x07

// PIN MUX Configuration strut values  (page 1420 from TRM)
#define PU_ENABLE    0x00
#define PU_DISABLE   0x01
#define PU_PULL_UP   0x01
#define PU_PULL_DOWN 0x00
#define RECV_ENABLE  0x01
#define RECV_DISABLE 0x00
#define SLEW_FAST    0x00
#define SLEW_SLOW    0x01

typedef union _CONF_MODULE_PIN_STRUCT   // See TRM Page 1420
{
	unsigned int d32;
	struct {    // name: field size
		unsigned int conf_mmode :3;       // LSB
		unsigned int conf_puden :1;
		unsigned int conf_putypesel :1;
		unsigned int conf_rxactive :1;
		unsigned int conf_slewctrl :1;
		unsigned int conf_res_1 :13;      // reserved
		unsigned int conf_res_2 :12;      // reserved MSB
	} b;
} _CONF_MODULE_PIN;

typedef struct {
	int count_thread;
	uintptr_t gpio1_base;
	struct sigevent pevent; // remember to fill in "event" structure in main
} keypadData;

void strobe_SCL(uintptr_t gpio_port_add);

void delaySCL();

uint32_t KeypadReadIObit(uintptr_t gpio_base, uint32_t BitsToRead);

void DecodeKeyValue(uint32_t word);

const struct sigevent* Inthandler(void* area, int id);

void *configureKeypad();

void *KeypadThread(void *data);

#endif /* KEYPAD_H_ */
