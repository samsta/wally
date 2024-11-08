#include "Program.h"

Sparkles::Sparkles(Leds& leds, bool with_sunset):
    Program(leds),
    m_sparkles{0},
    m_with_sunset(with_sunset) {
}

void Sparkles::loop(unsigned tick) {
    static uint8_t c = 0;

    for (unsigned row = 0; row < NUM_ROWS; ++row) {
        for (unsigned col = 0; col < NUM_LEDS_PER_ROW; ++col) {
            auto r = random16();
            if (r < 100) {
                m_sparkles[row][col] = 255;
            }
            else if (m_sparkles[row][col] > 5) {
                m_sparkles[row][col] -= 5;
            } else {
                m_sparkles[row][col] = 0;
            }

            leds()[row][col] = CHSV(c, 100, m_sparkles[row][col]);
            if (m_with_sunset) {
                leds()[row][col] = blend(leds()[row][col], ColorFromPalette(SunsetColors_p, col, 100), NUM_LEDS_PER_ROW-col);
            }
        }
    }
    c++;
}