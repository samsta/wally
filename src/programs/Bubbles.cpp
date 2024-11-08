#include "Program.h"
#include <algorithm>
#include <iterator>

Bubbles::Bubbles(Leds& leds, const TProgmemRGBPalette16& pal):
    Program(leds),
    m_pal(pal) {

    clearLeds();
}

void Bubbles::loop(unsigned tick) {
    clearLeds();

    const int num_bubbles = sizeof(m_bubbles)/sizeof(m_bubbles[0]);

    if (random8() < 20)
    {
        // move bubbles and add new one to front
        for (int b = num_bubbles - 1; b > 0; b--)
        {
            m_bubbles[b] = m_bubbles[b-1];
        }
        m_bubbles[0].x = random8(0, NUM_LEDS_PER_ROW);
        m_bubbles[0].y = random8(0, NUM_ROWS * 6);
        m_bubbles[0].speed_x = random8(0, 61) - 30;
        m_bubbles[0].speed_y = random8(0, 21) - 10;
        m_bubbles[0].ttl = 64;
        m_bubbles[0].color = ColorFromPalette(m_pal, random8(), 255);
    }
    
    for (int b = num_bubbles-1; b >= 0; b--) {
        if (m_bubbles[b].ttl >= 0) {
            for (unsigned i = 0; i < NUM_ROWS; i++) {
                for (unsigned j = 0; j < NUM_LEDS_PER_ROW; j++) {
                    int dx = j - int(m_bubbles[b].x);
                    int dy = i * 6 - int(m_bubbles[b].y);
                    if ((dx*dx)*20 + (dy*dy)*6 < m_bubbles[b].ttl * m_bubbles[b].ttl) {
                        if (leds()[i][j] == CRGB::Black) {
                            leds()[i][j] = m_bubbles[b].color;
                        }
                        else {
                            leds()[i][j] = blend(leds()[i][j], m_bubbles[b].color, 128);
                        }
                    }
                }
            }
            m_bubbles[b].ttl--;
            m_bubbles[b].x += m_bubbles[b].speed_x/20.0;
            m_bubbles[b].y += m_bubbles[b].speed_y/20.0;
        }
    }
}