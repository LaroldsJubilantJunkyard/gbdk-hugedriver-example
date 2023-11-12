#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

extern const hUGESong_t song_number3;


uint8_t joypadPrevious = 0;
uint8_t joypadCurrent = 0;

// We could do this for all 4 channels, but in this demo we'll only use one channel
int8_t mutedChannel1Timer=0;

void main(void)
{
    NR52_REG = 0x80;
    NR50_REG = 0xFF; // Max volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    // the critical tags ensure no interrupts will be called while this block of code is being executed
    __critical {
        hUGE_init(&song_number3);
        add_VBL(hUGE_dosound);
    }

    // Loop forever
    while(1) {

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();

        if((joypadCurrent & J_START)  && !(joypadPrevious & J_START)){

            // Muge the huge driver sounds for channel 1
            hUGE_mute_channel(HT_CH1,HT_CH_MUTE);

            // Play a sound effect on channel 1
            NR10_REG=0X00;
            NR11_REG=0X81;
            NR12_REG=0Xff;
            NR13_REG=0X73;
            NR14_REG=0X86;

            // We'll wait 10 frames before continuing our hugedriver track
            mutedChannel1Timer=10;
        }
        

        // If the timer is larger than 0, it's active
        if(mutedChannel1Timer>0){

            // Decease the timer until it reaches zero
            mutedChannel1Timer--;

            // If the timer just reached zero
            if(mutedChannel1Timer==0){

                // unmute the previously muted channel
                hUGE_mute_channel(HT_CH1,HT_CH_PLAY);
            }
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
