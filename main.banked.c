#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

BANKREF_EXTERN(song_number2_bankref)

extern const hUGESong_t song_number2;


// It's important this function be NONBANKED
// That'll garuantee it's placed in the fixed Bank 0
void PlaySoundVBL(void) NONBANKED{

    uint8_t _previous_bank = _current_bank;

    // Switch to whichever bank our song is in
    // Not neccessary if the song is in bank 0
    SWITCH_ROM(BANK(song_number2_bankref));

    hUGE_dosound();

    SWITCH_ROM(_previous_bank);
}

void main(void)
{
    NR52_REG = 0x80;
    NR50_REG = 0xFF; // Max volume for left/right speakers. 
    NR51_REG = 0xFF; // Turn on sound fully

    __critical {


        // Switch ROM bank BEFORE!!! starting a song in a different bank
        SWITCH_ROM(BANK(song_number2_bankref));
        hUGE_init(&song_number2);
        
        add_VBL(PlaySoundVBL);
    }

    // Loop forever
    while(1) {

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
