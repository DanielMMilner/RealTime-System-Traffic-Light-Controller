//#include <stdlib.h>
//#include <stdio.h>
//#include "stdint.h"
//#include <fcntl.h>
//#include <devctl.h>
//#include <hw/i2c.h>
//#include <errno.h>
//#include <unistd.h>
//#include <sys/neutrino.h>
//#include "LCDdisplay.h"
//
//#define DATA_SEND 0x40  // sets the Rs value high
//#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command
//
//
//// Function prototypes
//int *LCDfunction();
//int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData);
//void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column);
//void Initialise_LCD (int fd, _Uint32t LCDi2cAdd);
//
//int synchronized = 1;
//
//typedef struct
//{
//	int fd;
//	uint8_t Address;
//	uint8_t mode;
//	pthread_mutex_t mutex;
//} LCD_connect;
//
//
//
//void *LCDthread_A_ex (void *data)
//{
//	LCD_connect *td = (LCD_connect*) data;
//	uint8_t	LCDdata[10] = {};
//
//	int i;
//	for(i=0;i<100;i++)
//	{
//		if(synchronized) pthread_mutex_lock(&td->mutex);     //lock the function to make sure the variables are protected
//			// write some Text to the LCD screen
//			SetCursor(td->fd, td->Address,0,0); // set cursor on LCD to first position first line
//			sprintf(LCDdata,"A(0,0)=%d",i);
//			I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
//		if(synchronized) pthread_mutex_unlock(&td->mutex);	//unlock the functions to release the variables for use by other functions
//		usleep(1000000); // 1.0 seconds
//	}
//	return 0;
//}
//
//void *LCDthread_B_ex (void *data)
//{
//	LCD_connect *td = (LCD_connect*) data;
//	uint8_t	LCDdata[10] = {};
//
//	int i;
//	for(i=0;i<100;i++)
//	{
//		if(synchronized) pthread_mutex_lock(&td->mutex);     //lock the function to make sure the variables are protected
//			// write some Text to the LCD screen
//			SetCursor(td->fd, td->Address,0,10); // set cursor on LCD to first position first line
//			sprintf(LCDdata,"B(0,10)=%d",i);
//			I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
//		if(synchronized) pthread_mutex_unlock(&td->mutex);	//unlock the functions to release the variables for use by other functions
//		usleep(500000); // 0.5 seconds
//	}
//	return 0;
//}
//
//void *LCDthread_C_ex (void *data)
//{
//	LCD_connect *td = (LCD_connect*) data;
//	uint8_t	LCDdata[10] = {};
//
//	int i;
//	for(i=0;i<100;i++)
//	{
//		if(synchronized) pthread_mutex_lock(&td->mutex);     //lock the function to make sure the variables are protected
//			// write some Text to the LCD screen
//			SetCursor(td->fd, td->Address,1,0); // set cursor on LCD to first position first line
//			sprintf(LCDdata,"C(1,0)=%d",i);
//			I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
//		if(synchronized) pthread_mutex_unlock(&td->mutex);	//unlock the functions to release the variables for use by other functions
//		usleep(200000); // 0.2 seconds
//	}
//	return 0;
//}
//
//void *LCDthread_D_ex (void *data)
//{
//	LCD_connect *td = (LCD_connect*) data;
//	uint8_t	LCDdata[10] = {};
//
//	int i;
//	for(i=0;i<100;i++)
//	{
//		if(synchronized) pthread_mutex_lock(&td->mutex);     //lock the function to make sure the variables are protected
//			// write some Text to the LCD screen
//			SetCursor(td->fd, td->Address,1,10); // set cursor on LCD to first position first line
//			sprintf(LCDdata,"D(1,10)=%d",i);
//			I2cWrite_(td->fd, td->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
//		if(synchronized) pthread_mutex_unlock(&td->mutex);	//unlock the functions to release the variables for use by other functions
//		usleep(100000); // 0.1 seconds
//	}
//}
//
//
//int *LCDfunction()
//{
//	int file;
//	int error;
//	volatile uint8_t LCDi2cAdd = 0x3C;
//	_Uint32t speed = 10000; // nice and slow (will work with 200000)
//	LCD_connect td;
//
//	uint8_t	LCDdata[21] = {};
//
//	// Create the mutex
//	pthread_mutex_init(&td.mutex,NULL);		// pass NULL as the attr parameter to use the default attributes for the mutex
//
//	// Open I2C resource and set it up
//	if ((file = open("/dev/i2c1",O_RDWR)) < 0)	  // OPEN I2C1
//		printf("Error while opening Device File.!!\n");
//	else
//		printf("I2C1 Opened Successfully\n");
//
//	error = devctl(file,DCMD_I2C_SET_BUS_SPEED,&(speed),sizeof(speed),NULL);  // Set Bus speed
//	if (error)
//	{
//		fprintf(stderr, "Error setting the bus speed: %d\n",strerror ( error ));
//		exit(EXIT_FAILURE);
//	}
//	else
//		printf("Bus speed set = %d\n", speed);
//
//	Initialise_LCD(file, LCDi2cAdd);
//
//	usleep(1);
//
//
//	// launch threads
//	pthread_t  th1, th2, th3, th4;
//	pthread_mutex_lock(&td.mutex);		//lock the function to make sure the variables are protected
//		td.fd     = file;
//		td.Address= LCDi2cAdd;
//	td.mode   = DATA_SEND;
//    pthread_mutex_unlock(&td.mutex);	//unlock the functions to release the variables for use by other functions
//
//	pthread_create (&th1, NULL, LCDthread_A_ex, &td);
//	pthread_create (&th2, NULL, LCDthread_B_ex, &td);
//	pthread_create (&th3, NULL, LCDthread_C_ex, &td);
//	pthread_create (&th4, NULL, LCDthread_D_ex, &td);
//
//	pthread_join (th4, NULL); // wait for fastest thread to finish
//
//	// Destroy the mutex
//	pthread_mutex_destroy(&td.mutex);
//
//	printf("\n complete");
//	return EXIT_SUCCESS;
//}
//
//
//// Writes to I2C
//int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData)
//{
//	i2c_send_t hdr;
//    iov_t sv[2];
//    int status, i;
//
//    uint8_t LCDpacket[21] = {};  // limited to 21 characters  (1 control bit + 20 bytes)
//
//    // set the mode for the write (control or data)
//    LCDpacket[0] = mode;  // set the mode (data or control)
//
//	// copy data to send to send buffer (after the mode bit)
//	for (i=0;i<NbData+1;i++)
//		LCDpacket[i+1] = *pBuffer++;
//
//    hdr.slave.addr = Address;
//    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
//    hdr.len = NbData + 1;  // 1 extra for control (mode) bit
//    hdr.stop = 1;
//
//    SETIOV(&sv[0], &hdr, sizeof(hdr));
//    SETIOV(&sv[1], &LCDpacket[0], NbData + 1); // 1 extra for control (mode) bit
//      // int devctlv(int filedes, int dcmd,     int sparts, int rparts, const iov_t *sv, const iov_t *rv, int *dev_info_ptr);
//    status = devctlv(fd, 		  DCMD_I2C_SEND, 2,          0,          sv,              NULL,           NULL);
//
//    if (status != EOK)
//    	printf("status = %s\n", strerror ( status ));
//
//    return status;
//}
//
//
//void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column)
//{
//	uint8_t position = 0x80; // SET_DDRAM_CMD (control bit)
//	uint8_t rowValue = 0;
//	uint8_t	LCDcontrol = 0;
//	if (row == 1)
//		rowValue = 0x40;     // memory location offset for row 1
//	position = (uint8_t)(position + rowValue + column);
//	LCDcontrol = position;
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//}
//
//
//void Initialise_LCD (int fd, _Uint32t LCDi2cAdd)
//{
//	uint8_t	LCDcontrol = 0x00;
//
//	//   Initialise the LCD display via the I2C bus
//	LCDcontrol = 0x38;  // data byte for FUNC_SET_TBL1
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x39; // data byte for FUNC_SET_TBL2
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x14; // data byte for Internal OSC frequency
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x79; // data byte for contrast setting
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x50; // data byte for Power/ICON control Contrast set
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x6C; // data byte for Follower control
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x0C; // data byte for Display ON
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//
//	LCDcontrol = 0x01; // data byte for Clear display
//	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
//}
//
//
//
