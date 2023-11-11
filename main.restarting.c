#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

extern const hUGESong_t song_number3;
uint8_t musicIsPlaying=TRUE;
uint8_t joypadPrevious = 0;
uint8_t joypadCurrent = 0;

void main(void)
{
    NR52_REG = 0x80; // Master sound on
    NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    musicIsPlaying=TRUE;

    __critical {

        // Init and use huge drive to play song number 3
        hUGE_init(&song_number3);
        add_VBL(hUGE_dosound);
    }

    // Loop forever
    while(1) {

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();
        
        // If the start button was just pressed this frame
        if((joypadCurrent & J_START)  && !(joypadPrevious & J_START)){

            // Mute each channel
            // This is so that each channel stops and doesn't continue on
            hUGE_mute_channel(HT_CH1,HT_CH_MUTE);
            hUGE_mute_channel(HT_CH2,HT_CH_MUTE);
            hUGE_mute_channel(HT_CH3,HT_CH_MUTE);
            hUGE_mute_channel(HT_CH4,HT_CH_MUTE);

            hUGE_init(&song_number3);
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
