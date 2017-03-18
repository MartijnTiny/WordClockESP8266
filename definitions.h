#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifdef  __cplusplus
extern "C" {
#endif
// Neopixel
#define PIN 3         // PIN where neopixel / WS2811 strip is attached
//#define NUMLEDS 110    // Number of leds in the strip
#define NUMLEDS 2

#define TRIGGER_PIN 4
#define NTPPOLLTIME 60

#define HOSTNAME "Lauries_Klok"   // Friedly hostname


// ***************************************************************************
// Global variables / definitions
// ***************************************************************************
#define DBG_OUTPUT_PORT Serial  // Set debug output port

// List of all color modes
enum MODE { HOLD, OFF, ALL, WIPE, RAINBOW, RAINBOWCYCLE, THEATERCHASE, THEATERCHASERAINBOW, CLOCK, TEST, HEART };

MODE mode = CLOCK;   // Standard mode that is active when software starts

enum HEARTMODE {H_OFF, H_ON, H_HEARTBEAT, H_RAINBOW};
HEARTMODE heartmode = H_ON;

int delay_ms = 50;          // Global variable for storing the delay between color changes --> smaller == faster
int brightness = 75;       // Global variable for storing the brightness (255 == 100%)

bool exit_func = false;     // Global helper variable to get out of the color modes when mode changes

struct ledstate           // Data structure to store a state of a single led
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
};

uint8_t Rainbow_count = 0;
uint8_t Rainbow_color = 0;

typedef struct ledstate LEDState;   // Define the datatype LEDState
LEDState ledstates[NUMLEDS];        // Get an array of led states to store the state of the whole strip
LEDState main_color = {200,200,200};                // Store the "main color" of the strip used in single color modes 

#ifdef  __cplusplus
}
#endif

#endif  /* DEFINITIONS_H */  
