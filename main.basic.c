#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

extern const hUGESong_t song_number3;

void main(void)
{
    NR52_REG = 0x80; // Master sound on
    NR50_REG = 0xFF; // Maximum volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    __critical {

        // Init and use huge drive to play song number 3
        hUGE_init(&song_number3);
        add_VBL(hUGE_dosound);
    }

    // Loop forever
    while(1) {

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
