#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

extern const hUGESong_t sample_song;
extern const hUGESong_t song_number2;
extern const hUGESong_t song_number3;

uint8_t currentSound=0;
uint8_t muted=FALSE;
uint8_t soundVolume=0;
uint8_t soundBank=0;


// driver routine
void PlaySoundVBL(void) NONBANKED{

    uint8_t _previous_bank = _current_bank;

    // Switch to whichever bank our song is in
    // Not neccessary if the song is in bank 0
    SWITCH_ROM(soundBank);

    hUGE_dosound();

    SWITCH_ROM(_previous_bank);
}

void main(void)
{

    uint8_t joypadPrevious = 0;
    uint8_t joypadCurrent = 0;

    LCDC_REG = 0xD1;
    NR52_REG = 0x80;
    NR50_REG = 0; // Lowest volume for left/right speakers. a value of zero will still be audible
    NR51_REG = 0; // Turn off sound fully


    __critical {
        hUGE_init(&sample_song);
        add_VBL(PlaySoundVBL);
    }

    // Loop forever
    while(1) {
        uint8_t v;

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();

        if(!muted){
            if((soundVolume>>4)<7){
                soundVolume++;
            }
        }else {
            if((soundVolume>>4)>0){
                soundVolume--;
            }
        }

        v = (soundVolume>>4);

        NR50_REG = AUDVOL_VOL_LEFT(v) | AUDVOL_VOL_RIGHT(v); // a value of zero will still be audible
        NR51_REG = v!=0 ? 0xFF:0; // turn sound off fully if our volume is zero
        
        if((joypadCurrent & J_B)  && !(joypadPrevious & J_B)){
            muted=!muted;
        }
        
        if((joypadCurrent & J_A)  && !(joypadPrevious & J_A)){

            currentSound=(currentSound+1)%3;

            if(currentSound==0){

                // Sample song is in bank 0
                // No bank switching needed, bank 0 is always active
                hUGE_init(&sample_song);
            }else if(currentSound==1){
                soundBank=1;

                // Switch ROM bank BEFORE!!! starting a song in a different bank
                SWITCH_ROM(soundBank);
                hUGE_init(&song_number2);
            }else if(currentSound==2){
                soundBank=2;

                // Switch ROM bank BEFORE!!! starting a song in a different bank
                SWITCH_ROM(soundBank);
                hUGE_init(&song_number3);
            }

            muted=FALSE;

        }


		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
