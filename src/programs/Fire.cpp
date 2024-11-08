#include "Program.h"

Fire::Fire(Leds& leds, const TProgmemRGBPalette16& pal):
    Program(leds),
    m_pal(pal) {
    
    std::fill(&m_heat[0][0], &m_heat[0][0] + NUM_ROWS * NUM_LEDS_PER_ROW, 0);
}


void Fire::calcFire(unsigned tick)
{
   const unsigned CENTER_LEFT = NUM_LEDS_PER_ROW/2-1;
   const unsigned CENTER_RIGHT = NUM_LEDS_PER_ROW/2;

   int cooling = 9;

   for (unsigned r = 0; r < NUM_ROWS; r++)
   {
      // Step 1.  Cool down every cell a little
      m_heat[r][1] = m_heat[r][0];
      for(unsigned c = 0; c < NUM_LEDS_PER_ROW; c++)
      {
         m_heat[r][c] = qsub8(m_heat[r][c], random8(0, cooling));
      }
   }
        
   for (unsigned r = 0; r < NUM_ROWS; r++)
   {      
      // Step 2.  Heat from each cell drifts up and diffuses a little. We're starting at the bottom and move upwards
      for (unsigned c = NUM_LEDS_PER_ROW - 2; c >= 2; c--)
      {
         m_heat[r][c] = (m_heat[r][c - 1] + 2 * m_heat[r][c - 2])/3;
      }
          
      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      bool sparking = 20 > random8();
      if(sparking)
      {
        unsigned c = random8(8);
        m_heat[r][c] = qadd8(m_heat[r][c], random8(80,200));
      }
      m_heat[r][0] = qadd8(m_heat[r][0], random8(0, 5));
   }
}

void Fire::loop(unsigned tick) {
    calcFire(tick);
    for (unsigned i = 0; i < NUM_ROWS; i++) {
        for (unsigned j = 0; j < NUM_LEDS_PER_ROW; j++) {
            // diffuse a little sideways
            auto heat = m_heat[i][j];
            if (i > 0) {
                heat = qadd8(heat, m_heat[i-1][j]/10);
            }
            if (i < NUM_ROWS - 1) {
                heat = qadd8(heat, m_heat[i+1][j]/10);
            }
            leds()[i][j] = ColorFromPalette(m_pal, heat, 255);
        }
    }
}