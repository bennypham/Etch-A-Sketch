/*
Credits
=======
Jonathan Oaks
- for code to get data from SNES controller
- github.com/nailcliper/CS120B-Stepmania/blob/master/ATMega1284/src/snes.c

Ryan Yuzuki
- for breadboard layout
- github.com/ryuzu001/120bfinalproj

More information on SNES Controller
===================================
gamefaqs.gamespot.com/snes/916396-super-nintendo/faqs/5395
uzebox.org/files/NES-controller-Hydra-Ch6All-v1.0.pdf
youtube.com/watch?v=DSAs789t42k
*/

/*
Credit: Edward Carrasco for helping me setup the SNES controller working

*/

#ifndef SNESCONTROLLER_H_
#define SNESCONTROLLER_H_

// #define F_CPU 8000000UL // Clock Definition 8MHz
#include <avr/io.h>
#include <util/delay.h> // _delay_us

// ==========================================================
// In main(), the PORT's individual bits must be set to input or output
// e.g. DDRA = 0x06
#define SNES_PORT   PORTA
#define SNES_PIN    PINA
#define SNES_DATA       0 // PORTA0 must be set to input
#define SNES_LATCH      1 // PORTA1 must be set to output
#define SNES_CLOCK      2 // PORTA2 must be set to output
// ==========================================================

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

uint16_t snes_data = 0x00;

#define SNES_B      snes_data & 0x01
#define SNES_Y      (snes_data >> 1) & 0x01
#define SNES_SELECT (snes_data >> 2) & 0x01
#define SNES_START  (snes_data >> 3) & 0x01
#define SNES_UP     (snes_data >> 4) & 0x01
#define SNES_DOWN   (snes_data >> 5) & 0x01
#define SNES_LEFT   (snes_data >> 6) & 0x01
#define SNES_RIGHT  (snes_data >> 7) & 0x01
#define SNES_A      (snes_data >> 8) & 0x01
#define SNES_X      (snes_data >> 9) & 0x01
#define SNES_L      (snes_data >> 10) & 0x01
#define SNES_R      (snes_data >> 11) & 0x01

void SNES_ReadController(void) {
    //uint16_t snes_data = 0x0000;
    SNES_PORT = SET_BIT(SNES_PIN, SNES_LATCH);
    _delay_us(12);
    SNES_PORT = CLR_BIT(SNES_PIN, SNES_LATCH);
    _delay_us(6);

    for (unsigned char i = 0; i < 12; i++) {
        if(!GET_BIT(SNES_PIN, SNES_DATA)) {
            snes_data |= (1<<i);
        } else {
            snes_data &= ~(1<<i);
        }
        
        SNES_PORT = SET_BIT(SNES_PIN, SNES_CLOCK);
        _delay_us(6);
        SNES_PORT = CLR_BIT(SNES_PIN, SNES_CLOCK);
        _delay_us(6);
    }
    //return snes_data;
}

uint8_t SNES_ButtonB(uint16_t snes_data) { return snes_data & 0x01; }
uint8_t SNES_ButtonY(uint16_t snes_data) { return (snes_data >> 1) & 0x01; }
uint8_t SNES_ButtonSelect(uint16_t snes_data) { return (snes_data >> 2) & 0x01; }
uint8_t SNES_ButtonStart(uint16_t snes_data) { return (snes_data >> 3) & 0x01; }
uint8_t SNES_ButtonUp(uint16_t snes_data) { return (snes_data >> 4) & 0x01; }
uint8_t SNES_ButtonDown(uint16_t snes_data) { return (snes_data >> 5) & 0x01; }
uint8_t SNES_ButtonLeft(uint16_t snes_data) { return (snes_data >> 6) & 0x01; }
uint8_t SNES_ButtonRight(uint16_t snes_data) { return (snes_data >> 7) & 0x01; }
uint8_t SNES_ButtonA(uint16_t snes_data) { return (snes_data >> 8) & 0x01; }
uint8_t SNES_ButtonX(uint16_t snes_data) { return (snes_data >> 9) & 0x01; }
uint8_t SNES_ButtonL(uint16_t snes_data) { return (snes_data >> 10) & 0x01; }
uint8_t SNES_ButtonR(uint16_t snes_data) { return (snes_data >> 11) & 0x01; }

#endif /* SNESCONTROLLER_H_ */