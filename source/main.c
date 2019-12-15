/*	Author: bennypham
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#include "snescontroller.h"
#include "nokia5110.h"
#endif

uint8_t headX = 48;
uint8_t headY = 24;

unsigned long scenePeriod = 50; 
unsigned long buttonPeriod = 50; 
unsigned long tasksPeriod = 50; 
unsigned long SNES_Button_Period = 50; 

void drawBox() {
    unsigned char cols;
    unsigned char rows;
    //84 x 48 Nokia5110
    for(cols = 0; cols < 84; cols++) {
        NokiaLCD_SetPixel(cols, 0 , 1);
        NokiaLCD_SetPixel(cols, 47 , 1);
    }
    for(rows = 0; rows < 47; rows++) {
        NokiaLCD_SetPixel(0, rows , 1);
        NokiaLCD_SetPixel(83, rows , 1);
    }
}

void drawEtchSketch() {
    unsigned char cols;
    unsigned char rows;
    // Each time 
    for(cols = headX - 1; cols <= headX + 1; cols++) {
        for(rows = headY - 1; rows <= headY + 1; rows++) {
            NokiaLCD_SetPixel(cols, rows, 1);
        }
    }
}

void moveSketchUp() {
    headY -= 1;
}
void moveSketchDown() {
    headY += 1;
}
void moveSketchRight() {
    headX += 1;
}
void moveSketchLeft() {
    headX -= 1;
}

// enum MOVEMENT_States {buttonReleased, upPressed, downPressed, rightPressed, leftPressed} UP_State;
// int MOVE_Tick(int state) {
//     switch(state) {
//         case buttonReleased:
//             if(SNES_UP) {
//                 state = upPressed;
//             }
//             else if(SNES_DOWN) {
//                 state = downPressed;
//             }
//             else if(SNES_RIGHT) {
//                 state = rightPressed;
//             }
//             else if(SNES_LEFT) {
//                 state = leftPressed;
//             }
//             break;
//         case upPressed:
//             if(SNES_DOWN) {
//                 state = downPressed;
//             }
//             else if(SNES_RIGHT) {
//                 state = rightPressed;
//             }
//             else if(SNES_LEFT) {
//                 state = leftPressed;
//             }
//             else {
//                 state = upPressed;
//             }
//             break;
//         case downPressed:
//             if(SNES_RIGHT) {
//                 state = rightPressed;
//             }
//             else if(SNES_LEFT) {
//                 state = leftPressed;
//             }
//             else if(SNES_UP) {
//                 state = upPressed;
//             }
//             else {
//                 state = downPressed;
//             }
//             break;
//         case rightPressed:
//             if(SNES_LEFT) {
//                 state = leftPressed;
//             }
//             else if(SNES_UP) {
//                 state = upPressed;
//             }
//             else if(SNES_DOWN) {
//                 state = downPressed;
//             }
//             else {
//                 state = rightPressed;
//             }
//             break;
//         case leftPressed:
//             if(SNES_UP) {
//                 state = upPressed;
//             }
//             else if(SNES_DOWN) {
//                 state = downPressed;
//             }
//             else if(SNES_RIGHT) {
//                 state = rightPressed;
//             }
//             else {
//                 state = leftPressed;
//             }
//             break;
//     }
//     switch(state) {
//         case buttonReleased:
//             break;
//         case upPressed:
//             moveSketchUp();
//         case downPressed:
//             moveSketchDown();
//         case rightPressed:
//             moveSketchRight();
//         case leftPressed:
//             moveSketchLeft();
//         default:
//             break;
//     }
//     return state;
// }

int main(void) {
    DDRA = 0x06; //SNES Controller
    DDRC = 0xFF; PORTC = 0x00; //LCD
    DDRD = 0xFF; PORTD = 0x00; //LCD

    LCD_init();
    LCD_ClearScreen();
    NokiaLCD_Init();
    drawBox();
    // TimerSet(50);
    // TimerOn();
    // NokiaLCD_Clear();

    // unsigned char i = 0;
    // tasks[i].state = buttonReleased;
    // tasks[i].period = SNES_Button_Period;
    // tasks[i].elapsedTime = SNES_Button_Period;
    // tasks[i].TickFunction = &MOVE_Tick;
    // i++;

    // tasks[i].state = SM8_init;
    // tasks[i].period = 50;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].TickFunction = &SMTick8;
    // i++;

    // tasks[i].state = SM8_init;
    // tasks[i].period = 50;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].TickFunction = &SMTick8;
    // i++;
    // NokiaLCD_Render();

    while (1) {
        SNES_ReadController();
        if(SNES_UP) {
            moveSketchUp();
            LCD_WriteData('^');
        }
        if(SNES_DOWN) {
            moveSketchDown();
            LCD_WriteData('v');
        }
        if(SNES_RIGHT) {
            moveSketchRight();
            LCD_WriteData('>');
        }
        if(SNES_LEFT) {
            moveSketchLeft();
            LCD_WriteData('<');
        }
        drawEtchSketch();
        NokiaLCD_Render();

        // _delay_ms(10);
    }
    return 1;
}
