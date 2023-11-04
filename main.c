#include <gb/gb.h>
#include <stdint.h>
#include <gb/gb.h>
#include "hUGEDriver.h"

BANKREF_EXTERN(song_number1_bankref)
BANKREF_EXTERN(song_number2_bankref)

extern const hUGESong_t sample_song;
extern const hUGESong_t song_number2;
extern const hUGESong_t song_number3;

uint8_t nextSound =0;
uint8_t currentSound=0;
uint8_t muted=FALSE;
uint8_t soundVolume=0;
uint8_t soundBank=0;

// We could do this for all 4 channels, but in this demo we'll only use one channel
int8_t mutedChannel1Timer=0;

void ChangeSong() NONBANKED{

    __critical {

        currentSound = nextSound;

        // Save what bank we are in. Later we will restore it if changed
        // This function is non banked, theoretically: it could be called from code from another bank, we want to make sure
        // That everything is the same bank-wise by the end
        uint8_t _previous_bank = _current_bank;

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

void UpdateAudioVolume() NONBANKED{

    uint8_t nonShiftedSoundVolume;

    // If were not changing sounds
    if(currentSound==nextSound){

        // Incease our sound volume while it's below 7 (7 is the maximum volume)
        if((soundVolume>>4)<7){
            soundVolume++;
        }
    
    // Otherwise, if we are changing sounds
    }else {

        // Decrease the sound volume while it's above 0
        if((soundVolume>>4)>0){
            soundVolume--;
        }else{

            // If it's below 0, change the song
            ChangeSong();
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

// driver routine
void PlaySoundVBL(void) NONBANKED{

    UpdateAudioVolume();

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
    NR50_REG = 0; // Lowest volume for left/right speakers. 
    // A 'NR50_REG' value of zero will still be audible, so we turn off sound fully
    NR51_REG = 0; // Turn off sound fully


    muted=FALSE;

    __critical {
        ChangeSong();
        add_VBL(PlaySoundVBL);
    }

    // Loop forever
    while(1) {

        joypadPrevious=joypadCurrent;
        joypadCurrent = joypad();

        if((joypadCurrent & J_B)  && !(joypadPrevious & J_B)){

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
        
        // If a was just pressed, and we aren't currently changing sounds
        if((joypadCurrent & J_A)  && !(joypadPrevious & J_A) && currentSound==nextSound){

            // Change the next sound we play
            nextSound=(currentSound+1)%3;
        }

        if(mutedChannel1Timer>0){
            mutedChannel1Timer--;
            if(mutedChannel1Timer==0){
                hUGE_mute_channel(HT_CH1,HT_CH_PLAY);
            }
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
