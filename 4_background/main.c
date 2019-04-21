#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "blob.c"
#include "background.c"
#include "backgroundMap.c"
#include "windowMap.c"

void main(){
    font_t min_font;
    UINT8 current_sprite_index = 0;

    font_init();
    min_font = font_load(font_min); // 36 tile
    font_set(min_font);

    

    set_bkg_data(37, 7, backgroundTiles);
    set_bkg_tiles(0, 0, 40, 18, backgroundMap);

    set_win_tiles(0, 0, 5, 1, windowMap);
    move_win(7, 120);

    set_sprite_data(0, 2, blob);
    set_sprite_tile(0, 0);
    move_sprite(0, 88, 78);

    SHOW_BKG;
    SHOW_WIN;
    SHOW_SPRITES;

    //Turn on sound
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
    DISPLAY_ON;


    while(1) {
        //Audio
        UBYTE joypad_state = joypad();

        if(joypad_state) {
            // see https://github.com/bwhitman/pushpin/blob/master/src/gbsound.txt
            // chanel 1 register 0, Frequency sweep settings
            // 7	Unused
            // 6-4	Sweep time(update rate) (if 0, sweeping is off)
            // 3	Sweep Direction (1: decrease, 0: increase)
            // 2-0	Sweep RtShift amount (if 0, sweeping is off)
            // 0001 0110 is 0x16, sweet time 1, sweep direction increase, shift ammount per step 110 (6 decimal)
            NR10_REG = 0x16; 

            // chanel 1 register 1: Wave pattern duty and sound length
            // Channels 1 2 and 4
            // 7-6	Wave pattern duty cycle 0-3 (12.5%, 25%, 50%, 75%), duty cycle is how long a quadrangular  wave is "on" vs "of" so 50% (2) is both equal.
            // 5-0 sound length (higher the number shorter the sound)
            // 01000000 is 0x40, duty cycle 1 (25%), wave length 0 (long)
            NR11_REG = 0x30;

            // chanel 1 register 2: Volume Envelope (Makes the volume get louder or quieter each "tick")
            // On Channels 1 2 and 4
            // 7-4	(Initial) Channel Volume
            // 3	Volume sweep direction (0: down; 1: up)
            // 2-0	Length of each step in sweep (if 0, sweeping is off)
            // NOTE: each step is n/64 seconds long, where n is 1-7	
            // 0111 0011 is 0x73, volume 7, sweep down, step length 3
            NR12_REG = 0x73;  

            // chanel 1 register 3: Frequency LSbs (Least Significant bits) and noise options
            // for Channels 1 2 and 3
            // 7-0	8 Least Significant bits of frequency (3 Most Significant Bits are set in register 4)
            NR13_REG = 0x00;   

            // chanel 1 register 4: Playback and frequency MSbs
            // Channels 1 2 3 and 4
            // 7	Initialize (trigger channel start, AKA channel INIT) (Write only)
            // 6	Consecutive select/length counter enable (Read/Write). When "0", regardless of the length of data on the NR11 register, sound can be produced consecutively.  When "1", sound is generated during the time period set by the length data contained in register NR11.  After the sound is ouput, the Sound 1 ON flag, at bit 0 of register NR52 is reset.
            // 5-3	Unused
            // 2-0	3 Most Significant bits of frequency
            // 1100 0011 is 0xC3, initialize, no consecutive, frequency = MSB + LSB = 011 0000 0000 = 0x300
            NR14_REG = 0xC3;	    
        }

        //Walking Animation
        if(current_sprite_index == 0){
            current_sprite_index = 1;
        }
        else{
            current_sprite_index = 0;
        }

        //Controls
        switch(joypad()){
            case J_LEFT:
                scroll_sprite(0, -5, 0);
                set_sprite_tile(0, current_sprite_index);
                //Flip X
                set_sprite_prop(0, 0x20U);
                break;
            case J_RIGHT:
                scroll_sprite(0, 5, 0);
                set_sprite_tile(0, current_sprite_index);
                //Default X
                set_sprite_prop(0, 0x00U);
                break;
            case J_UP:
                scroll_sprite(0, 0, -5);
                break;
            case J_DOWN:
                scroll_sprite(0, 0, 5);
                break;
        }

        scroll_bkg(1, 0);

        //Game Time
        delay(100);
    }
}