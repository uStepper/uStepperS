#ifndef GCODE_H_
#define GCODE_H_

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define MAX_PACKET_SIZE 200
#define MAX_COMMAND_SIZE 10
/** Time before a packet is considered received in microseconds */
#define PACKET_TIMEOUT 10000 // 10ms

#define GCODE_DEFAULT_VALID_RES "OK"
#define GCODE_DEFAULT_ERROR_RES "ERROR"

/** States for packet */
typedef enum {
	GCODE_PACKET_NONE = 0,
	GCODE_PACKET_READY,
	GCODE_PACKET_CRC_UNVALID,
	GCODE_PACKET_CRC_MISSING,
} packet_state_t;

/** Structure to hold gcode commands and functions to call */
typedef struct {
	char command[MAX_COMMAND_SIZE];
	void (*func)(char *command, char *data);
} command_func_t;


class GCode {

	public:
		GCode(void);

		/** Called in the main loop to process the serial data (added by using insert()) */
		void run(void);

		/** Send commands / data using the "sendFunc" */ 
		void send(char *command);
		void send(char *command, bool checksum);

		/** Extract value from the packet, f.x. X10.0 */
		bool value(char *name, float *var);
		bool value(char *name, int32_t *var);
		bool value(char *name, float *var, char *packet);
		bool value(char *name, int32_t *var, char *packet);

		/** Check if packet contains a specific command, f.x. G0 */
		bool check(char *cmd);
		bool check(char *cmd, char *packet );

		/** Return the current packet for manuel inspection */
		char *getPacket(void);

		/** Returns the state of the current package */
		uint8_t getStatus(void);

		/** Insert serial (rx) data to be processed */
		void insert(char data);
		void insert(char * data);

		/** Assign a function used to send serial data */
		void setSendFunc(void (*func)(char *data));

		/** Add a command and a function to call when that command is received */
		void addCommand(char * command, void (*func)(char *command, char *data));

		/** Change the response on valid and error */
		void setValidRes( const char * str );
		void setErrorRes( const char * str );

		/** Enable or disable the usage of checksum */
		void enableCRC( bool state );

		/** Print all loaded g-codes */
		void printCommands( void );

	private:

		/** Packet buffer to hold the actual received packet */
		char packet[MAX_PACKET_SIZE];

		/** Status of the actual package */
		uint8_t status = 0; 

		/** Contains a list of commands and function pointers */
		command_func_t * commands = NULL; 

		/** Number of commands added */
		uint8_t commandCount = 0;

		/** Function pointer to a function that sends serial data */
		void (*sendFunc)(char *data );

		/** Function pointer to a function that processes any unknown commands */
		void (*defaultFunc)(char *command, char *data);
		
		/* Function to read actual bytes from serial and build packet (is not blocking) */
		uint8_t read(void);
		uint8_t process(void);

		char validRes[10];
		char errorRes[10]; 

		bool useCRC = false;

		bool inPacket = false; 	// New packet is incoming
		uint32_t lastChar = 0; 	// When last byte was received
		uint8_t packetLen = 0;	// Lenght of current packet
};

#endif
