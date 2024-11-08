#include "Arduino.h"
#include "FastLED.h"
#include "Palettes.hpp"
#include "Preferences.h"
#include <algorithm>
#include <iterator>
#include "Constants.h"
#include "Program.h"

Leds leds;
Leds leds_reordered;
uint8_t program_memory[MAX_PROGRAM_SIZE] = {0};

Program* current_program = nullptr;

Preferences preferences;

const char* app_key = "app";
const char* brightness_key = "b";
const char* program_key = "p";

unsigned last_program_number = 0;
const uint8_t MIN_BRIGHTNESS = 1;
const uint8_t MAX_BRIGHTNESS = 50;
uint8_t brightness = MAX_BRIGHTNESS;

const unsigned MIN_PROG_CHANGE_MS = 10;
const unsigned MAX_PROG_CHANGE_MS = 300;
const unsigned MIN_BRIGHTNESS_CHANGE_MS = 1000;
const unsigned BRIGTHNESS_CHANGE_STEP_MS = 200;
const unsigned PREFERENCES_COOLDOWN_MS = 10000;

const uint8_t BUTTON_PIN = 13;
const uint8_t LED_PIN = 2;


void onProgramChange(uint8_t program) {

    /*
        NOTE: we are using placement new to create the program object in the
              pre-allocated memory. This is done to avoid dynamic memory allocation
    */

    if (current_program) {
        current_program->~Program();
    }

    switch (program) {

        default:
            program = 0; // keep within bounds if we get an invalid program number
            /*
                fall through
            */
        case 0:
            current_program = new (program_memory) Noise(leds, SamsSpecial_p);
            break;
        case 1:
            current_program = new (program_memory) Noise(leds, GoldStripe_p);
            break;
        case 2:
            current_program = new (program_memory) Noise(leds, PurpleBlue_p);
            break;
        case 3:
            current_program = new (program_memory) Noise(leds, PartyStripes_p);
            break;
        case 4:
            current_program = new (program_memory) Fire(leds, HeatColors_p);
            break;
        case 5:
            current_program = new (program_memory) Fire(leds, OceanFromBlack_p);
            break;
        case 6:
            current_program = new (program_memory) Fire(leds, BlackToPartyColors_p);
            break;
        case 7:
            current_program = new (program_memory) Rays(leds);
            break;
        case 8:
            current_program = new (program_memory) Sparkles(leds, false);
            break;
        case 9:
            current_program = new (program_memory) Sparkles(leds, true);
            break;
        case 10:
            current_program = new (program_memory) Bubbles(leds, PartyColors_p);
            break;
        case 11:
            current_program = new (program_memory) Bubbles(leds, ForestColors_p);
            break;
        case 12:
            current_program = new (program_memory) Bubbles(leds, SamsSpecial_p);
            break;	
    }
    last_program_number = program;
}

void setup() {
    Serial.begin(9600); 
    FastLED.addLeds<WS2811, LED_PIN, RGB>(&leds_reordered[0][0], NUM_LEDS)
        .setCorrection(TypicalLEDStrip);

    preferences.begin(app_key, true);
    if (preferences.isKey(brightness_key)) {
        brightness = preferences.getUChar(brightness_key);
        Serial.println("Brightness loaded: " + String(brightness));
    }
    if (preferences.isKey(program_key)) {
        last_program_number = preferences.getUChar(program_key);
        Serial.println("Program loaded: " + String(last_program_number));
    }
    preferences.end();

    FastLED.setBrightness(brightness);
    onProgramChange(last_program_number);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
    static unsigned tick = 0;
    static bool button_was_pressed = false;
    static unsigned button_press_ms = 0;
    static bool preferences_saved = true;
    static unsigned last_preferences_change_ms = 0;

    /*
        The button is used to change the brightness and the program. The user can
        cycle through the programs by pressing the button for a short time, and
        change the brightness by holding the button down for a longer time.
     */
    auto now = millis();
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!button_was_pressed) {
            button_press_ms = now;
            button_was_pressed = true;
        }

        if (now - button_press_ms > MIN_BRIGHTNESS_CHANGE_MS) {
            if (brightness <= MIN_BRIGHTNESS) {
                brightness = MAX_BRIGHTNESS;
            } else {
                brightness--;
            }
            FastLED.setBrightness(brightness);
            button_press_ms += BRIGTHNESS_CHANGE_STEP_MS;
            last_preferences_change_ms = now;
            preferences_saved = false;
        }

    } else {
        if (button_was_pressed && 
            (now - button_press_ms > MIN_PROG_CHANGE_MS) &&
            (now - button_press_ms < MAX_PROG_CHANGE_MS)) {

            onProgramChange(last_program_number + 1);
            last_preferences_change_ms = now;
            preferences_saved = false;
        }
        button_was_pressed = false;
    }

    /*
        To reduce flash memory wear, we save changes only when they have been
        stable for a while as the user might be cycling through programs or
        brightness levels to find something they like.
    */
    if (((now - last_preferences_change_ms) > PREFERENCES_COOLDOWN_MS) && !preferences_saved) {
        preferences.begin(app_key, false);
        preferences.putUChar(brightness_key, brightness);
        preferences.putUChar(program_key, last_program_number);
        preferences.end();
        preferences_saved = true;
        Serial.println("Preferences saved: brightness=" + String(brightness) + ", program=" + String(last_program_number));
    }

    current_program->loop(++tick);
    
    /*
        The strips are arranged in rows going back and forth to minimise wiring, like this:
          >>>>>>>>>>>>>>>>>\
                           |
         /<<<<<<<<<<<<<<<<</
         |
         \>>>>>>>>>>>>>>>>>>
                           
        This means that we need to reorder the rows before we write them out to the strips,
        with every other row reversed.
    */
    for (int i=0; i < NUM_ROWS; i++) {
        if (i % 2) {
            std::copy(std::begin(leds[i]), std::end(leds[i]), leds_reordered[i]);
        } else {
            std::reverse_copy(std::begin(leds[i]), std::end(leds[i]), leds_reordered[i]);
        }
    }

    FastLED.show();
    delay(16);
}
