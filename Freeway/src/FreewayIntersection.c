/*
 * FreewayIntersection.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include "FreewayIntersection.h"

void changeMainIntersectionState(enum mainIntersectionStates *newState,
		timer_t *timer_id, struct itimerspec *itime) {
	switch (*newState) {
	case RRRRRRRR:
		printf("RRRRRRRR 1\n");
		setTime(timer_id, itime, 1);
		*newState = GGGGRRRR;
		break;
	case GGGGRRRR:
		printf("GGGGRRRR 2\n");
		setTime(timer_id, itime, 4);
		if (turningSensors.NE_Waiting || turningSensors.SW_Waiting
				|| turningSensors.ES_Waiting || turningSensors.WN_Waiting) {
			*newState = YGYGRRRR;
		} else {
			*newState = GGGGRRRR;
		}
		break;
	case YGYGRRRR:
		printf("YGYGRRRR 3\n");
		setTime(timer_id, itime, 2);
		*newState = RGRGRRGG;
		break;
	case RGRGRRGG:
		printf("RGRGRRGG 4\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.NE_Waiting, 0);
		changeSensor(&turningSensors.SW_Waiting, 0);
		*newState = RGRGRRYY;
		break;
	case RGRGRRYY:
		printf("RGRGRRYY 5\n");
		setTime(timer_id, itime, 2);
		if (!turningSensors.ES_Waiting && turningSensors.WN_Waiting) {
			*newState = RGRYRRRR;
		} else if (turningSensors.ES_Waiting && turningSensors.WN_Waiting) {
			*newState = RYRYRRRR;
		} else if (turningSensors.ES_Waiting && !turningSensors.WN_Waiting) {
			*newState = RYRGGRRR;
		} else {
			*newState = GGGGRRRR;
		}
		break;
	case RYRYRRRR:
		printf("RYRYRRRR 6\n");
		setTime(timer_id, itime, 2);
		*newState = RRRRGGRR;
		break;
	case RRRRGGRR:
		printf("RRRRGGRR 7\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
		*newState = RRRRYYRR;
		break;
	case RRRRYYRR:
		printf("RRRRYYRR 8\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	case RYRGGRRR:
		printf("RYRGGRRR 9\n");
		setTime(timer_id, itime, 2);
		*newState = RRRGGRRR;
		break;
	case RRRGGRRR:
		printf("RRRGGRRR 10\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.ES_Waiting, 0);
		*newState = RRRGYRRR;
		break;
	case RRRGYRRR:
		printf("RRRGYRRR 11\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	case RGRYRRRR:
		printf("RGRYRRRR 12\n");
		setTime(timer_id, itime, 2);
		*newState = RGRRGRRR;
		break;
	case RGRRGRRR:
		printf("RGRRGRRR 13\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.WN_Waiting, 0);
		*newState = RGRRRYRR;
		break;
	case RGRRRYRR:
		printf("RGRRRYRR 14\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	}
}

void *mainIntersectionStateMachine() {
	enum mainIntersectionStates newState = RRRRRRRR;
	my_message_t msg;
	int chid;
	int rcvid;
	struct itimerspec itime;
	timer_t timer_id;

	setUpTimer(&chid, &timer_id);

	changeMainIntersectionState(&newState, &timer_id, &itime);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				changeMainIntersectionState(&newState, &timer_id, &itime);
			}
		}
	}
	return EXIT_SUCCESS;
}

/*** Server code ***/
//void *server()
//{
//	int serverPID=0, chid=0; 	// Server PID and channel ID
//
//	serverPID = getpid(); 		// get server process ID
//
//	// Create Channel
//	chid = ChannelCreate(_NTO_CHF_DISCONNECT);
//	if (chid == -1)  // _NTO_CHF_DISCONNECT flag used to allow detach
//	{
//	    printf("\nFailed to create communication channel on server\n");
//	    pthread_exit( EXIT_FAILURE );
//	}
//
//	printf("Server Listening for Clients on:\n");
//	printf("  --> Process ID   : %d \n", serverPID);
//	printf("  --> Channel ID   : %d \n\n", chid);
//
//	char str[sizeof(serverPID) + sizeof(chid)+1];
//	sprintf(str, "%d\n%d",serverPID,chid);
//
//	FILE* file_ptr = fopen("/net/VM_x86_Target01/tmp/s3542977Server.info", "w");
//	if(fwrite(&str, sizeof(str), 1, file_ptr) < 1){
//		printf("Error: Could not write to file\n");
//	}
//	fclose( file_ptr );
//
//	my_data msg;
//	int rcvid=0, msgnum=0;  	// no message received yet
//	int Stay_alive=0, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)
//
//	my_reply replymsg; 			// replymsg structure for sending back to client
//	replymsg.hdr.type = 0x01;
//	replymsg.hdr.subtype = 0x00;
//
//	living =1;
//	while (living)
//	{
//	   // Do your MsgReceive's here now with the chid
//	   rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
//
//	   if (rcvid == -1)  // Error condition, exit
//	   {
//		   printf("\nFailed to MsgReceive\n");
//		   break;
//	   }
//
//	   // did we receive a Pulse or message?
//	   // for Pulses:
//	   if (rcvid == 0)  //  Pulse received, work out what type
//	   {
//		   switch (msg.hdr.code)
//		   {
//			   case _PULSE_CODE_DISCONNECT:
//					// A client disconnected all its connections by running
//					// name_close() for each name_open()  or terminated
//				   if( Stay_alive == 0)
//				   {
//					   ConnectDetach(msg.hdr.scoid);
//					   printf("\nServer was told to Detach from ClientID:%d ...\n", msg.ClientID);
//					   living = 0; // kill while loop
//					   continue;
//				   }
//				   else
//				   {
//					   printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg.ClientID);
//				   }
//				   break;
//
//			   case _PULSE_CODE_UNBLOCK:
//					// REPLY blocked client wants to unblock (was hit by a signal
//					// or timed out).  It's up to you if you reply now or later.
//				   printf("\nServer got _PULSE_CODE_UNBLOCK after %d, msgnum\n", msgnum);
//				   break;
//
//			   case _PULSE_CODE_COIDDEATH:  // from the kernel
//				   printf("\nServer got _PULSE_CODE_COIDDEATH after %d, msgnum\n", msgnum);
//				   break;
//
//			   case _PULSE_CODE_THREADDEATH: // from the kernel
//				   printf("\nServer got _PULSE_CODE_THREADDEATH after %d, msgnum\n", msgnum);
//				   break;
//
//			   default:
//				   // Some other pulse sent by one of your processes or the kernel
//				   printf("\nServer got some other pulse after %d, msgnum\n", msgnum);
//				   break;
//
//		   }
//		   continue;// go back to top of while loop
//	   }
//
//	   // for messages:
//	   if(rcvid > 0) // if true then A message was received
//	   {
//		   msgnum++;
//
//		   // If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
//		   if (msg.hdr.type == _IO_CONNECT )
//		   {
//			   MsgReply( rcvid, EOK, NULL, 0 );
//			   printf("\n gns service is running....");
//			   continue;	// go back to top of while loop
//		   }
//
//		   // Some other I/O message was received; reject it
//		   if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX )
//		   {
//			   MsgError( rcvid, ENOSYS );
//			   printf("\n Server received and IO message and rejected it....");
//			   continue;	// go back to top of while loop
//		   }
//
//		   // A message (presumably ours) received
//
//		   // put your message handling code here and assemble a reply message
//		   sensor = msg.data;
//		   sprintf(replymsg.buf, "Message %d received", msgnum);
//		   printf("Message received '%c' from client (ID:%d), ", msg.data, msg.ClientID);
//		   fflush(stdout);
//
//		   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
//	   }
//	   else
//	   {
//		   printf("\nERROR: Server received something, but could not handle it correctly\n");
//	   }
//
//	}
//
//	printf("\nServer received Destroy command\n");
//	// destroyed channel before exiting
//	ChannelDestroy(chid);
//	return EXIT_SUCCESS;
//}
