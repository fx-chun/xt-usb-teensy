#include <WProgram.h>
#include <usb_keyboard.h>
#include <stdbool.h>

#include "keymap.h"

// macro for modifier key cases during the scancode switch(){}
// all modifier key related scancodes should just update
// keyboard_modifier_keys
// (see https://www.pjrc.com/teensy/usb_keyboard.html)
#define MODIFIER(scancode, modifier)                                    \
        case scancode: {                                                \
            if (isScan) { keyboard_modifier_keys |= modifier; }         \
            else { keyboard_modifier_keys &= ~(modifier); }             \
        } break;

/**** CONFIGURABLES ****/
// pin numbers
const int CLOCK_PIN = 22;
const int DATA_PIN = 3;
const int LED_PIN = 13;
/**** END CONFIGURABLES ****/

// bitmask for the scan/release bit in the byte we receive from the XT keyboard 
const uint8_t BITMASK_SCANRELEASE = (1<<7);

// globals for the ISR 
volatile bool g_startedRead = 0; // have we started our read? are we in the middle of one?
volatile uint8_t g_currentRead = 0;     // the byte we are currently reading into
volatile int g_currentReadBitIndex = 0; // the bit we should be updating this interrupt
volatile uint8_t g_readBuffer[16]; // buffer of read bytes from the XT keyboard
volatile int g_bufferSize = 0;     // current size of the buffer

// globals (regular)
// bitmap keeping track of the keys currently pressed down
bool keysDown[MAX_SCANCODE_VALUE] = { 0 };

// ISR for the CLOCK_PIN`s falling edge. 
void clockFalling() {
    if (!g_startedRead) {
        // check for start(1) bit, start read if detected
        g_startedRead = digitalReadFast(DATA_PIN); 

        // initialize variables for read
        g_currentRead = 0; 
        g_currentReadBitIndex = 0;
    } else { 
        // we're reading from the keyboard rn!!

        // read bit & attach onto our working byte 
        g_currentRead |= (digitalReadFast(DATA_PIN) << g_currentReadBitIndex);
        g_currentReadBitIndex++; // next bit!

        if (g_currentReadBitIndex == 8) {
            // finished read, push value to buffer
            g_readBuffer[g_bufferSize] = g_currentRead;
            g_bufferSize++;
            g_startedRead = false; // wait for start(1) bit next interrupt 
        }
    }
}

int main(void) {
    // set up pins, clock interrupt, keyboard state
    pinMode(CLOCK_PIN, INPUT);
    pinMode(DATA_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    attachInterrupt(CLOCK_PIN, clockFalling, FALLING);
    keyboard_modifier_keys = 0;

    // we're alive...
    digitalWriteFast(LED_PIN, HIGH);
    delay(200);
    digitalWriteFast(LED_PIN, LOW);
    delay(200);
    digitalWriteFast(LED_PIN, HIGH);
    delay(200);
    digitalWriteFast(LED_PIN, LOW);

    for (;;) {
        uint8_t value = 0; // raw byte read from the XT keyboard
        bool hasValue = false; // is there a byte to process?

        // use the builtin LED for numlock
        digitalWriteFast(LED_PIN, (keyboard_leds == 1) ? HIGH : LOW);

        // try to grab a value from the read buffer
        cli();
        if (g_bufferSize > 0) {
            hasValue = true;
            value = g_readBuffer[g_bufferSize - 1];
            g_bufferSize--;
        }
        sei();

        if (hasValue) {
            int scanCode = (value & ~(BITMASK_SCANRELEASE));
            bool isScan = (value & BITMASK_SCANRELEASE) == 0; // scan or release?

            // set the keyboard state (keyboard_keys[], keyboard_modifiers) 
            switch (scanCode) {
                MODIFIER(SC_SHIFT_L  , KEY_LEFT_SHIFT ); 
                MODIFIER(SC_SHIFT_R  , KEY_RIGHT_SHIFT); 
                MODIFIER(SC_CONTROL_L, KEY_LEFT_CTRL  );
                MODIFIER(SC_ALT_L    , KEY_LEFT_ALT   );
                MODIFIER(SC_CAPS_LOCK, KEY_LEFT_GUI   );

                //case 0xE0: {// multi-sequence key
                //    // TODO
                //} break;

                default: { // normal key
                    // update our scan table
                    keysDown[scanCode] = isScan;

                    // generate keyboard_keys
                    // loop over keysDown, pick the first 6 keys that are down
                    
                    int keyboardStateIndex = 0;

                    keyboard_keys[0] = 0;
                    keyboard_keys[1] = 0;
                    keyboard_keys[2] = 0;
                    keyboard_keys[3] = 0;
                    keyboard_keys[4] = 0;
                    keyboard_keys[5] = 0;

                    for (int i = 1; i < MAX_SCANCODE_VALUE; i++) {
                        if (keysDown[i]) {
                            // populate one entry
                            keyboard_keys[keyboardStateIndex] = KEYMAP[i]; 
                            keyboardStateIndex++; 
                        }

                        if (keyboardStateIndex >= 6) break; // filled our 6-key limit
                    }
                } break;
            }

            // send the state over usb
            usb_keyboard_send();
        }
    }

    return -1;
}
