#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

BANKREF_EXTERN(song_number1_bankref)
BANKREF_EXTERN(song_number2_bankref)

extern const hUGESong_t sample_song;
extern const hUGESong_t song_number2;
extern const hUGESong_t song_number3;

uint8_t currentSound=0;
uint8_t soundVolume=0;
uint8_t soundBank=0;
uint8_t pauseMusic=FALSE;

uint8_t joypadPrevious = 0;
uint8_t joypadCurrent = 0;


// We could do this for all 4 channels, but in this demo we'll only use one channel
int8_t mutedChannel1Timer=0;

void ChangeSong(uint8_t nextSound) NONBANKED{

    __critical {

        currentSound = nextSound;

        // Save what bank we are in. Later we will restore it if changed
        // This function is non banked, theoretically: it could be called from code from another bank, we want to make sure
        // That everything is the same bank-wise by the end
        uint8_t _previous_bank = _current_bank;

        // Mute each channel
        hUGE_mute_channel(HT_CH1,HT_CH_MUTE);
        hUGE_mute_channel(HT_CH2,HT_CH_MUTE);
        hUGE_mute_channel(HT_CH3,HT_CH_MUTE);
        hUGE_mute_channel(HT_CH4,HT_CH_MUTE);

        if(currentSound==0){

            soundBank=BANK(song_number1_bankref);

            // Sample song is in bank 0
            // No bank switching needed, bank 0 is always active
            hUGE_init(&sample_song);

        }else if(currentSound==1){

            soundBank=BANK(song_number2_bankref);

            // Switch ROM bank BEFORE!!! starting a song in a different bank
            SWITCH_ROM(soundBank);
            hUGE_init(&song_number2);

        }else if(currentSound==2){

            // We didn't auto-bank song number 3, so we'll manually specify
            soundBank=2;

            // Switch ROM bank BEFORE!!! starting a song in a different bank
            SWITCH_ROM(soundBank);
            hUGE_init(&song_number3);
        }

        // Restore our previous bank
        SWITCH_ROM(_previous_bank);
    }
}

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
    NR52_REG = 0x80;
    NR50_REG = 0xff; // Maximum volume for left/right speakers. 
    NR51_REG = 0xff; // Turn on sound fully

    pauseMusic=FALSE;

    // the critical tags ensure no interrupts will be called while this block of code is being executed
    __critical {
        ChangeSong(0);
        add_VBL(PlaySoundVBL);
    }

    // Loop forever
    while(1) {

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();
        
        // If a was just pressed, and we aren't currently changing sounds
        if((joypadCurrent & J_A)  && !(joypadPrevious & J_A)){

            // Change the next sound we play
            ChangeSong((currentSound+1)%3);
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
