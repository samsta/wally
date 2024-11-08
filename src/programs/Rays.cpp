#include "Program.h"

Rays::Rays(Leds& leds):
    Program(leds) {}

void Rays::loop(unsigned tick) {
    clearLeds();

    if ((m_num_rays < NUM_RAYS-1) and (random8() < 20)) {
        m_rays[m_num_rays].row = random8() % NUM_ROWS;
        m_rays[m_num_rays].speed = random8(1, 45);
        m_rays[m_num_rays].length = 1;
        m_rays[m_num_rays].age = 0;
        m_rays[m_num_rays].hue = (tick % 24) * 2 - 50;
        m_num_rays++;

    }

    for (unsigned ray_ix = 0; ray_ix < m_num_rays; ray_ix++) {
        auto& ray = m_rays[ray_ix];
        ray.length += ray.speed/10;
        ray.speed += 1;

        for (int col = std::min(ray.length, NUM_LEDS_PER_ROW-1); col>=0; col--) {
            int color_index = ray.length - col;
            leds()[ray.row][NUM_LEDS_PER_ROW-1-col] = CHSV(ray.hue - ray.age, std::min(255, 100 + color_index * color_index), std::max(0, 255 * (col - ray.age) / ray.length));
        }

        ray.age++;
        if (ray.age > 100) {
            for (unsigned i = 0; i < m_num_rays-1; i++) {
                m_rays[i] = m_rays[i+1];
            }
            m_num_rays--;
        }
    }
}