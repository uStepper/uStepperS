#define GCODE_MOVE 				"G0"
#define GCODE_MOVETO            "G1"
#define GCODE_CONTINUOUS 		"G2"
#define GCODE_BRAKE 			"G3"
#define GCODE_HOME 				"G4"

// Miscellaneous commands
#define GCODE_STOP 				"M0" // Stop everything
#define GCODE_SET_SPEED 		"M1"
#define GCODE_SET_ACCEL 		"M2"
#define GCODE_SET_BRAKE_FREE	"M3"
#define GCODE_SET_BRAKE_COOL 	"M4"
#define GCODE_SET_BRAKE_HARD 	"M5"
#define GCODE_SET_CL_ENABLE 	"M6" // Enable closed loop 
#define GCODE_SET_CL_DISABLE 	"M7" // Disable closed loop

#define GCODE_RECORD_START 		"M10"
#define GCODE_RECORD_STOP 		"M11"
#define GCODE_RECORD_ADD 		"M12"
#define GCODE_RECORD_PLAY 		"M13"
#define GCODE_RECORD_PAUSE 		"M14"
#define GCODE_REQUEST_DATA 		"M15"
#define GCODE_REQUEST_CONFIG    "M16"

#define BAUD_RATE 115200
