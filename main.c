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

int8_t mutedChannel1Timer=0;
int8_t mutedChannel2Timer=0;
int8_t mutedChannel3Timer=0;
int8_t mutedChannel4Timer=0;


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

    uint8_t v;

    if(currentSound==nextSound){
        if((soundVolume>>4)<7){
            soundVolume++;
        }
    }else {
        if((soundVolume>>4)>0){
            soundVolume--;
        }else{

            ChangeSong();
        }
    }

    v = (soundVolume>>4);

    // v should be a value from 0-7
    if(v>7)v=7;

    // For more Info: https://gbdev.io/pandocs/Audio_Registers.html#ff24--nr50-master-volume--vin-panning
    // bit's 6, 5, and 4 are the left volume
    // bit's 0, 1, and 2 are the right volume
    // |       |      7     |   6  5  4    |     3      |   2  1  0    |
    // | NR50  |  VIN left  | Left Volume  | VIN right  | Right VOlume |
    NR50_REG = AUDVOL_VOL_LEFT(v) | AUDVOL_VOL_RIGHT(v); // a value of zero will still be audible
    NR51_REG = v!=0 ? 0xFF:0; // turn sound off fully if our volume is zero
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

        if((joypadCurrent & J_START)  && !(joypadPrevious & J_START)){

            hUGE_mute_channel(HT_CH1,HT_CH_MUTE);

            NR10_REG=0X00;
            NR11_REG=0X81;
            NR12_REG=0Xff;
            NR13_REG=0X73;
            NR14_REG=0X86;

            mutedChannel1Timer=10;
        }
        
        if((joypadCurrent & J_A)  && !(joypadPrevious & J_A) && currentSound==nextSound){

            nextSound=(currentSound+1)%3;
        }

        if(mutedChannel1Timer>0){
            mutedChannel1Timer--;
            if(mutedChannel1Timer==0){
                hUGE_mute_channel(HT_CH1,HT_CH_PLAY);
            }
        }

        if(mutedChannel2Timer>0){
            mutedChannel2Timer--;
            if(mutedChannel2Timer==0){
                hUGE_mute_channel(HT_CH2,HT_CH_PLAY);
            }
        }

        if(mutedChannel3Timer>0){
            mutedChannel3Timer--;
            if(mutedChannel3Timer==0){
                hUGE_mute_channel(HT_CH3,HT_CH_PLAY);
            }
        }

        if(mutedChannel4Timer>0){
            mutedChannel4Timer--;
            if(mutedChannel4Timer==0){
                hUGE_mute_channel(HT_CH4,HT_CH_PLAY);
            }
        }

		// Done processing, yield CPU and wait for start of next frame
        vsync();
    }
}
