#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"


extern const hUGESong_t song_number3;

uint8_t soundVolume=0;
uint8_t muteMusic=FALSE;

uint8_t joypadPrevious = 0;
uint8_t joypadCurrent = 0;


void UpdateAudioVolume() NONBANKED{

    uint8_t nonShiftedSoundVolume;

    // If were not changing sounds
    if(!muteMusic){

        // Incease our sound volume while it's below 7 (7 is the maximum volume)
        if((soundVolume>>4)<7){
            soundVolume++;
        }
    
    // Otherwise, if we are changing sounds
    }else {

        // Decrease the sound volume while it's above 0
        if((soundVolume>>4)>0){
            soundVolume--;
        }
    }

    nonShiftedSoundVolume = (soundVolume>>4);

    // nonShiftedSoundVolume should be a value from 0-7
    if(nonShiftedSoundVolume>7)nonShiftedSoundVolume=7;

    // For more Info: https://gbdev.io/pandocs/Audio_Registers.html#ff24--nr50-master-volume--vin-panning
    // bit's 6, 5, and 4 are the left volume
    // bit's 0, 1, and 2 are the right volume
    // |       |      7     |   6  5  4    |     3      |   2  1  0    |
    // | NR50  |  VIN left  | Left Volume  | VIN right  | Right VOlume |
    NR50_REG = AUDVOL_VOL_LEFT(nonShiftedSoundVolume) | AUDVOL_VOL_RIGHT(nonShiftedSoundVolume); // a value of zero will still be audible
    NR51_REG = nonShiftedSoundVolume!=0 ? 0xFF:0; // turn sound off fully if our volume is zero
}


void main(void)
{
    NR52_REG = 0x80;
    NR50_REG = 0; // Lowest volume for left/right speakers. 
    // A 'NR50_REG' value of zero will still be audible, so we turn off sound fully
    NR51_REG = 0; // Turn off sound fully

    muteMusic=FALSE;

    // the critical tags ensure no interrupts will be called while this block of code is being executed
    __critical {
        hUGE_init(&song_number3);
        add_VBL(hUGE_dosound);
    }

    // Loop forever
    while(1) {

        UpdateAudioVolume();

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();
        
        if((joypadCurrent & J_START)  && !(joypadPrevious & J_START)){

            muteMusic=!muteMusic;
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
