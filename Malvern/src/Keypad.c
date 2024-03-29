#include "Keypad.h"

// create global struct to share data between threads
keypadData ISR_area_data;

void strobe_SCL(uintptr_t gpio_port_add) {
	uint32_t PortData;
	PortData = in32(gpio_port_add + GPIO_DATAOUT); // value that is currently on the GPIO port
	PortData &= ~(SCL);
	out32(gpio_port_add + GPIO_DATAOUT, PortData); // Clock low
	delaySCL();

	PortData = in32(gpio_port_add + GPIO_DATAOUT); // get port value
	PortData |= SCL; // Clock high
	out32(gpio_port_add + GPIO_DATAOUT, PortData);
	delaySCL();
}

void delaySCL() { // Small delay used to get timing correct for BBB
	volatile int i;
	for (i = 0; i < 0x1F; i++) // 0x1F results in a delay that sets F_SCL to ~480 kHz
			{ // i*1 is faster than i+1 (i+1 results in F_SCL ~454 kHz, whereas i*1 is the same as a=i)
		i *= 1;
	}
}

uint32_t KeypadReadIObit(uintptr_t gpio_base, uint32_t BitsToRead) {
	volatile uint32_t val = 0;
	val = in32(gpio_base + GPIO_DATAIN); // value that is currently on the GPIO port

	val &= BitsToRead; // mask bit

	if (val == BitsToRead)
		return 1;
	else
		return 0;
}

void *KeypadThread(void *data)
{
	keypadData *kd = (keypadData*) data;
	int id = 0; // Attach interrupt Event to IRQ for GPIO1B  (upper 16 bits of port)


	// Main code starts here
	// The thread that calls InterruptWait() must be the one that called InterruptAttach().
	// id = InterruptAttach (GPIO1_IRQ, Inthandler, &ISR_area_data, sizeof(ISR_area_data), _NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_NO_UNMASK | _NTO_INTR_FLAGS_END);
	id = InterruptAttach(GPIO1_IRQ, Inthandler, kd,
			sizeof(*kd), _NTO_INTR_FLAGS_TRK_MSK);

	InterruptUnmask(GPIO1_IRQ, id);  // Enable a hardware interrupt

	uint32_t val = 0;
	while(1)
	{
		// Block main until we get a sigevent of type: 	ISR_area_data.pevent
		InterruptWait(0, NULL); // block this thread until an interrupt occurs  (Wait for a hardware interrupt)

		volatile uint32_t word = 0;
		//  confirm that SD0 is still low (that is a valid Key press event has occurred)
		val = KeypadReadIObit(kd->gpio1_base, SD0); // read SD0 (means data is ready)

		if (val == 0)  // start reading key value form the keypad
		{
			word = 0;  // clear word variable

			delaySCL(); // wait a short period of time before reading the data Tw  (10 us)

			for (int i = 0; i < 16; i++)          // get data from SD0 (16 bits)
			{
				strobe_SCL(kd->gpio1_base); // strobe the SCL line so we can read in data bit

				val = KeypadReadIObit(kd->gpio1_base, SD0); // read in data bit
				val = ~val & 0x01; // invert bit and mask out everything but the LSB
				word = word | (val << i); // add data bit to word in unique position (build word up bit by bit)
			}

			if((word != 32768) && (word != 0)) // ignore key 16
			{
				pthread_mutex_lock(&kd->mutex);
				kd->new_press = 1;
				kd->key = word;
				pthread_mutex_unlock(&kd->mutex);
			}
		}
	}

//	munmap_device_io(control_module, AM335X_CONTROL_MODULE_SIZE);

	return EXIT_SUCCESS;
}

const struct sigevent* Inthandler(void* area, int id) {
	keypadData *p_ISR_data = (keypadData *) area;

	InterruptMask(GPIO1_IRQ, id);  // Disable all hardware interrupt

	// must do this in the ISR  (else stack over flow and system will crash
	out32(p_ISR_data->gpio1_base + GPIO_IRQSTATUS_1, SD0); //clear IRQ

	// do this to tell us how many times this handler gets called
	p_ISR_data->count_thread++;
	// got IRQ.
	// work out what it came from

	InterruptUnmask(GPIO1_IRQ, id);  // Enable a hardware interrupt

	// return a pointer to an event structure (preinitialized
	// by main) that contains SIGEV_INTR as its notification type.
	// This causes the InterruptWait in "int_thread" to unblock.
	return (&p_ISR_data->pevent);
}

void *configureKeypad(keypadData *kd)
{
	kd->new_press = 0;
	pthread_mutex_init(&kd->mutex, NULL);

	uintptr_t control_module = mmap_device_io(AM335X_CONTROL_MODULE_SIZE,
	AM335X_CONTROL_MODULE_BASE);
	kd->gpio1_base = mmap_device_io(AM335X_GPIO_SIZE, AM335X_GPIO1_BASE);

	// initalise the global stuct
	kd->count_thread = 0;

	memset(&kd->pevent, 0, sizeof(kd->pevent));
	SIGEV_INTR_INIT(&kd->pevent);
	kd->pevent.sigev_notify = SIGEV_INTR; // Setup for external interrupt

	int data = 1;
	// we also need to have the PROCMGR_AID_INTERRUPT and PROCMGR_AID_IO abilities enabled. For more information, see procmgr_ability().
	ThreadCtl(_NTO_TCTL_IO_PRIV, &data);  // Request I/O privileges  for QNX7;

	procmgr_ability(0, PROCMGR_AID_INTERRUPT | PROCMGR_AID_IO);

	if ((control_module) && (kd->gpio1_base)) {

		volatile uint32_t val = 0;

		// set DDR for LEDs to output and GPIO_28 to input
		val = in32(kd->gpio1_base + GPIO_OE); // read in current setup for GPIO1 port
		val |= 1 << 28;                // set IO_BIT_28 high (1=input, 0=output)
		out32(kd->gpio1_base + GPIO_OE, val); // write value to input enable for data pins
		val &= ~(LED0 | LED1 | LED2 | LED3);    // write value to output enable
		out32(kd->gpio1_base + GPIO_OE, val); // write value to output enable for LED pins

		val = in32(kd->gpio1_base + GPIO_OE);
		val &= ~SCL;                      // 0 for output
		out32(kd->gpio1_base + GPIO_OE, val); // write value to output enable for data pins

		val = in32(kd->gpio1_base + GPIO_DATAOUT);
		val |= SCL;           // Set Clock Line High as per TTP229-BSF datasheet
		out32(kd->gpio1_base + GPIO_DATAOUT, val); // for 16-Key active-Low timing diagram

		in32s(&val, 1, control_module + P9_12_pinConfig);
		printf("Original pinmux configuration for GPIO1_28 = %#010x\n", val);

		// set up pin mux for the pins we are going to use  (see page 1354 of TRM)
		volatile _CONF_MODULE_PIN pinConfigGPMC; // Pin configuration strut
		pinConfigGPMC.d32 = 0;
		// Pin MUX register default setup for input (GPIO input, disable pull up/down - Mode 7)
		pinConfigGPMC.b.conf_slewctrl = SLEW_SLOW; // Select between faster or slower slew rate
		pinConfigGPMC.b.conf_rxactive = RECV_ENABLE; // Input enable value for the PAD
		pinConfigGPMC.b.conf_putypesel = PU_PULL_UP; // Pad pullup/pulldown type selection
		pinConfigGPMC.b.conf_puden = PU_ENABLE;    // Pad pullup/pulldown enable
		pinConfigGPMC.b.conf_mmode = PIN_MODE_7; // Pad functional signal mux select 0 - 7

		// Write to PinMux registers for the GPIO1_28
		out32(control_module + P9_12_pinConfig, pinConfigGPMC.d32);
		in32s(&val, 1, control_module + P9_12_pinConfig);   // Read it back
		printf("New configuration register for GPIO1_28 = %#010x\n", val);

		// Setup IRQ for SD0 pin ( see TRM page 4871 for register list)
		out32(kd->gpio1_base + GPIO_IRQSTATUS_SET_1, SD0); // Write 1 to GPIO_IRQSTATUS_SET_1
		out32(kd->gpio1_base + GPIO_IRQWAKEN_1, SD0);  // Write 1 to GPIO_IRQWAKEN_1
		out32(kd->gpio1_base + GPIO_FALLINGDETECT, SD0);    // set falling edge
		out32(kd->gpio1_base + GPIO_CLEARDATAOUT, SD0);   // clear GPIO_CLEARDATAOUT
		out32(kd->gpio1_base + GPIO_IRQSTATUS_1, SD0);      // clear any prior IRQs
	}
	return EXIT_SUCCESS;
}
