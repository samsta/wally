#pragma once

#include "Constants.h"
#include "FastLED.h"
#include "Palettes.hpp"
#include <algorithm>

typedef CRGB Leds[NUM_ROWS][NUM_LEDS_PER_ROW];

class Program {
public:
    virtual ~Program(){}

    Program(Leds& leds):
        m_leds(leds) {}

    virtual void loop(unsigned tick) = 0;

    Leds& leds() {
        return m_leds;
    }

    void clearLeds() {
        std::fill(&m_leds[0][0], &m_leds[0][0] + NUM_ROWS * NUM_LEDS_PER_ROW, CRGB::Black);
    }

    virtual unsigned ticksPerBeat() const {
        return 32;
    }

private:
    Leds& m_leds;
};


/*
    The Noise program is a simple noise generator that uses a palette to color the noise.
    It's a 2D perlin noise generator that is used to generate a 2D noise field that is then
    used to color the LEDs. The parameters of the noise field are changed over time to create
    an animation.
*/
class Noise: public Program {
public:
    Noise(Leds& leds, const TProgmemRGBPalette16& pal);

    void loop(unsigned tick) override;

private:
    uint32_t m_t;
    uint32_t m_t2;
    const TProgmemRGBPalette16& m_pal;
};

/*
    The Sparkles program is a simple program that creates random sparkles on the LEDs.
    The sparkles are created by randomly deciding if a pixel should be lit up and then
    fading the pixel out over time. The sparkles are colored by going around the color
    wheel over time, with a low saturation to keep them white-ish.

    Optionally, the Sparkles program can be created with a sunset background, which
    is a simple gradient from yellow to red, purple, blue and then black.

*/
class Sparkles: public Program {
public:
    Sparkles(Leds& leds, bool with_sunset);

    void loop(unsigned tick) override;
private:
    uint8_t m_sparkles[NUM_ROWS][NUM_LEDS_PER_ROW];
    bool m_with_sunset;
};

/*
    The Rays program creates rays (or shooting stars) that move across the LED panel, from top to bottom.
*/
class Rays: public Program {
public:
    Rays(Leds& leds);

    void loop(unsigned tick) override;
private:
    static const unsigned NUM_RAYS = 12;
    struct Ray {
        int row;
        int length;
        int speed;
        int age;
        int hue;
    };
    Ray m_rays[NUM_RAYS];
    unsigned m_num_rays = 0;
};

/*
    Colorful bubbles that float around the LED panel.
*/
class Bubbles: public Program {
public:
    Bubbles( Leds& leds, const TProgmemRGBPalette16& pal);

    void loop(unsigned tick) override;
private:
    struct Bubble {
        Bubble(): ttl(-1) {}
        float x;
        float y;
        int ttl;
        float speed_x;
        float speed_y;
        CRGB color;
    };
    Bubble m_bubbles[16];
    const TProgmemRGBPalette16& m_pal;
};

/*
    A fire-like effect that moves up the LED panel.
 */
class Fire: public Program {
public:
    Fire(Leds& leds, const TProgmemRGBPalette16& pal);

    void loop(unsigned tick) override;
private:
    void calcFire(unsigned tick);
    uint8_t m_heat[NUM_ROWS][NUM_LEDS_PER_ROW];
    const TProgmemRGBPalette16& m_pal;
};

// determine the size of the biggest class in here
constexpr size_t max_size(size_t a, size_t b) {
    return a > b ? a : b;
}

constexpr size_t MAX_PROGRAM_SIZE = max_size(
    sizeof(Noise), max_size(
        sizeof(Sparkles), max_size(
            sizeof(Rays), max_size(
                sizeof(Bubbles), sizeof(Fire)
            )
        )
    )
);