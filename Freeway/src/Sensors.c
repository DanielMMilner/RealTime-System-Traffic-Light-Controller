#include "Sensors.h"

TurningSensors turningSensors = { 0, 0, 0, 0, 1, 0, 0 };

// create global struct to share data between threads
ISR_data ISR_area_data;

int getSensorValue(int sensor) {
	//returns the current value of a sensor
	pthread_mutex_lock(&turningSensors.mutex);
	int value = 0;
	switch (sensor) {
	case (1):
		value = turningSensors.NE_Waiting;
		break;
	case (2):
		value = turningSensors.SW_Waiting;
		break;
	case (3):
		value = turningSensors.ES_Waiting;
		break;
	case (4):
		value = turningSensors.WN_Waiting;
		break;
	case (5):
		value = turningSensors.Use_Sensors;
		break;
	case (6):
		value = turningSensors.East_Onramp;
		break;
	case (7):
		value = turningSensors.West_Onramp;
		break;
	}
	pthread_mutex_unlock(&turningSensors.mutex);

	return value;
}

void changeSensor(int* sensor, int value) {
	pthread_mutex_lock(&turningSensors.mutex);
	if (value == 2) {
		//toggle sensor
		*sensor = !*sensor;
	} else {
		//activates or deactivates sensor
		*sensor = value;
	}
	pthread_mutex_unlock(&turningSensors.mutex);
}

void remoteSensorActivation(int sensor) {
	//converts remote command from central controller into an action
	switch (sensor) {
	case (1):
		changeSensor(&turningSensors.NE_Waiting, 1);
		break;
	case (2):
		changeSensor(&turningSensors.SW_Waiting, 1);
		break;
	case (3):
		changeSensor(&turningSensors.ES_Waiting, 1);
		break;
	case (4):
		changeSensor(&turningSensors.WN_Waiting, 1);
		break;
	case (5):
		changeSensor(&turningSensors.Use_Sensors, 2);
		if(turningSensors.Use_Sensors){
			printf("Sensors enabled\n");
		}else{
			printf("Sensors disabled\n");
		}
		break;
	case (6):
		changeSensor(&turningSensors.East_Onramp, 2);
		if(turningSensors.East_Onramp){
			printf("East on ramp lights enabled\n");
		}else{
			printf("East on ramp lights disabled\n");
		}
		break;
	case (7):
		changeSensor(&turningSensors.West_Onramp, 2);
		if(turningSensors.West_Onramp){
			printf("East on ramp lights enabled\n");
		}else{
			printf("East on ramp lights disabled\n");
		}
		break;
	}
}

void *userInput(){
	//gets keyboard input
	int temp;
	while(1){
		temp = getchar();
		remoteSensorActivation(temp - '0');
	}
}

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

void DecodeKeyValue(uint32_t word) {
	//gets keypad input and acts accordingly.
	switch (word) {
	case 0x01:
		printf("NE Sensor triggered\n");
		changeSensor(&turningSensors.NE_Waiting, 1);
		break;
	case 0x02:
		printf("SW Sensor triggered\n");
		changeSensor(&turningSensors.SW_Waiting, 1);
		break;
	case 0x04:
		printf("ES Sensor triggered\n");
		changeSensor(&turningSensors.ES_Waiting, 1);
		break;
	case 0x08:
		printf("WN Sensor triggered\n");
		changeSensor(&turningSensors.WN_Waiting, 1);
		break;
	case 0x10:
		printf("Key  5 pressed\n");
		changeSensor(&turningSensors.Use_Sensors, 2);
		break;
	case 0x20:
		changeSensor(&turningSensors.East_Onramp, 2);
		break;
	case 0x40:
		changeSensor(&turningSensors.West_Onramp, 2);
		break;
	case 0x00:  // key release event (do nothing)
		break;
	}
}

const struct sigevent* Inthandler(void* area, int id) {
	ISR_data *p_ISR_data = (ISR_data *) area;

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

void *keypadMethod() {
	pthread_mutex_init(&turningSensors.mutex,NULL);		// pass NULL as the attr parameter to use the default attributes for the mutex

	uintptr_t control_module = mmap_device_io(AM335X_CONTROL_MODULE_SIZE,
	AM335X_CONTROL_MODULE_BASE);
	uintptr_t gpio1_base = mmap_device_io(AM335X_GPIO_SIZE, AM335X_GPIO1_BASE);

	// initalise the global stuct
	ISR_area_data.count_thread = 0;
	ISR_area_data.gpio1_base = gpio1_base;

	memset(&ISR_area_data.pevent, 0, sizeof(ISR_area_data.pevent));
	SIGEV_INTR_INIT(&ISR_area_data.pevent);
	ISR_area_data.pevent.sigev_notify = SIGEV_INTR; // Setup for external interrupt

	int data = 1;
	// we also need to have the PROCMGR_AID_INTERRUPT and PROCMGR_AID_IO abilities enabled. For more information, see procmgr_ability().
	ThreadCtl(_NTO_TCTL_IO_PRIV, &data);  // Request I/O privileges  for QNX7;

	procmgr_ability(0, PROCMGR_AID_INTERRUPT | PROCMGR_AID_IO);

	if ((control_module) && (gpio1_base)) {

		volatile uint32_t val = 0;

		// set DDR for LEDs to output and GPIO_28 to input
		val = in32(gpio1_base + GPIO_OE); // read in current setup for GPIO1 port
		val |= 1 << 28;                // set IO_BIT_28 high (1=input, 0=output)
		out32(gpio1_base + GPIO_OE, val); // write value to input enable for data pins
		val &= ~(LED0 | LED1 | LED2 | LED3);    // write value to output enable
		out32(gpio1_base + GPIO_OE, val); // write value to output enable for LED pins

		val = in32(gpio1_base + GPIO_OE);
		val &= ~SCL;                      // 0 for output
		out32(gpio1_base + GPIO_OE, val); // write value to output enable for data pins

		val = in32(gpio1_base + GPIO_DATAOUT);
		val |= SCL;           // Set Clock Line High as per TTP229-BSF datasheet
		out32(gpio1_base + GPIO_DATAOUT, val); // for 16-Key active-Low timing diagram

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
		out32(gpio1_base + GPIO_IRQSTATUS_SET_1, SD0); // Write 1 to GPIO_IRQSTATUS_SET_1
		out32(gpio1_base + GPIO_IRQWAKEN_1, SD0);  // Write 1 to GPIO_IRQWAKEN_1
		out32(gpio1_base + GPIO_FALLINGDETECT, SD0);    // set falling edge
		out32(gpio1_base + GPIO_CLEARDATAOUT, SD0);   // clear GPIO_CLEARDATAOUT
		out32(gpio1_base + GPIO_IRQSTATUS_1, SD0);      // clear any prior IRQs

		int id = 0; // Attach interrupt Event to IRQ for GPIO1B  (upper 16 bits of port)

		// Main code starts here
		// The thread that calls InterruptWait() must be the one that called InterruptAttach().
		// id = InterruptAttach (GPIO1_IRQ, Inthandler, &ISR_area_data, sizeof(ISR_area_data), _NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_NO_UNMASK | _NTO_INTR_FLAGS_END);
		id = InterruptAttach(GPIO1_IRQ, Inthandler, &ISR_area_data,
				sizeof(ISR_area_data), _NTO_INTR_FLAGS_TRK_MSK);

		InterruptUnmask(GPIO1_IRQ, id);  // Enable a hardware interrupt

		int i = 0;
		printf("Main entering loop and will call InterruptWait\n");
		for (;;) {
			// Block main until we get a sigevent of type: 	ISR_area_data.pevent
			InterruptWait(0, NULL); // block this thread until an interrupt occurs  (Wait for a hardware interrupt)

			volatile uint32_t word = 0;
			//  confirm that SD0 is still low (that is a valid Key press event has occurred)
			val = KeypadReadIObit(gpio1_base, SD0); // read SD0 (means data is ready)

			if (val == 0)  // start reading key value form the keypad
					{
				word = 0;  // clear word variable

				delaySCL(); // wait a short period of time before reading the data Tw  (10 us)

				for (i = 0; i < 16; i++)          // get data from SD0 (16 bits)
						{
					strobe_SCL(gpio1_base); // strobe the SCL line so we can read in data bit

					val = KeypadReadIObit(gpio1_base, SD0); // read in data bit
					val = ~val & 0x01; // invert bit and mask out everything but the LSB
					//printf("val[%u]=%u, ",i, val);
					word = word | (val << i); // add data bit to word in unique position (build word up bit by bit)
				}
				//printf("word=%u\n",word);
				DecodeKeyValue(word);
			}
		}

		munmap_device_io(control_module, AM335X_CONTROL_MODULE_SIZE);
	}
	return EXIT_SUCCESS;
}
