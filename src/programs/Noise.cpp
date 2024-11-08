#include "Program.h"
#include <algorithm>

Noise::Noise(Leds& leds, const TProgmemRGBPalette16& pal):
    Program(leds),
    m_pal(pal) {}

void Noise::loop(unsigned tick) {
    unsigned sx = 6000;
    unsigned sy = 2000;
    for (unsigned h = 0; h < NUM_ROWS; h++)
    {
        for (unsigned v = 0; v < NUM_LEDS_PER_ROW; v++)
        {
            int noise = 128 + inoise16_raw(h*sx + m_t*10, v*sy + m_t2*10, m_t + m_t2)/100;
            noise = std::max(std::min(noise, 255), 0);
            leds()[h][v] = ColorFromPalette(m_pal, noise, 255);
        }
    }

    m_t += 10;
    m_t2 += 10;
}