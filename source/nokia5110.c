/* Nokia 5110 LCD AVR Library
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 * Updated by Edward Carrasco
 */

/*
Edward Carrasco helped me setup the Nokia5110

*/

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "nokia5110.h"
#include "nokia5110_chars.h"
//#include "sprites.h"

/************************************************************************/
/* PRIVATE                                                                     */
/************************************************************************/

static struct {
	uint8_t screen[504]; // 84 x 48 = 4032 --> 4032 / 8 = 504
	uint8_t cursorX;
	uint8_t cursorY;
	uint8_t scale; // 1: fits 7 lines of 14 characters
} nokiaLCD = {
	.cursorX = 0,
	.cursorY = 0,
	.scale = 1
};

static void write(uint8_t bytes, uint8_t isData) {
	register uint8_t i;
	PORT_LCD &= ~(1 << LCD_SCE); // Enable controller
	if (isData) { // Sending data or commands
		PORT_LCD |= (1 << LCD_DC);
	} else {
		PORT_LCD &= ~(1 << LCD_DC);
	}
	// Send bytes
	for (i = 0; i < 8; i++) {
		// Set data pin to byte state
		if ((bytes >> (7-i)) & 0x01) {
			PORT_LCD |= (1 << LCD_DIN);
		} else {
			PORT_LCD &= ~(1 << LCD_DIN);
		}
		// Blink clock
		PORT_LCD |= (1 << LCD_CLK);
		PORT_LCD &= ~(1 << LCD_CLK);
	}
	PORT_LCD |= (1 << LCD_SCE); // Disable controller
}

static void writeCommand(uint8_t command) {
	write(command, 0);
}

static void writeData(uint8_t data) {
	write(data, 1);
}

/************************************************************************/
/* PUBLIC FUNCTIONS                                                                     */
/************************************************************************/

void NokiaLCD_Init(void) {
	register unsigned i;
	// Set pins as output
	DDR_LCD |= (1 << LCD_SCE);
	DDR_LCD |= (1 << LCD_RST);
	DDR_LCD |= (1 << LCD_DC);
	DDR_LCD |= (1 << LCD_DIN);
	DDR_LCD |= (1 << LCD_CLK);

	// Reset display
	PORT_LCD |= (1 << LCD_RST);
	PORT_LCD |= (1 << LCD_SCE);
	_delay_ms(10);
	PORT_LCD &= ~(1 << LCD_RST);
	_delay_ms(70);
	PORT_LCD |= (1 << LCD_RST);
	
	// Initialize display
	PORT_LCD &= ~(1 << LCD_SCE); // Enable controller
	writeCommand(0x21); // LCD Extended Commands mode
	writeCommand(0x13); // LCD bias mode 1:48
	writeCommand(0x06); // Set temperature coefficient
	writeCommand(0xC2); // Default VOP (3.06 + 66 * 0.06 = 7V
	writeCommand(0x20); // Standard Commands mode, powered down
	writeCommand(0x09); // LCD in normal mode
	writeCommand(0x80); // Clear LCD RAM
	writeCommand(LCD_CONTRAST);
	
	for (i = 0; i < 504; i++) {
		writeData(0x00);
	}

	// Activate LCD
	writeCommand(0x08);
	writeCommand(0x0C);
}

void NokiaLCD_Clear(void) {
	register unsigned i;
	// Set column and row to 0
	writeCommand(0x80);
	writeCommand(0x40);
	// Reset the cursor position
	nokiaLCD.cursorX = 0;
	nokiaLCD.cursorY = 0;
	// Clear all pixels
	for (i = 0; i < 504; i++) {
		nokiaLCD.screen[i] = 0x00;
	}
}

void NokiaLCD_SetPower(uint8_t bool) {
	writeCommand(bool ? 0x20 : 0x24);
}

void NokiaLCD_SetPixel(uint8_t x, uint8_t y, uint8_t bool) {
	uint8_t *byte = &nokiaLCD.screen[y/8*84+x];
	if (bool) {
		*byte |= (1 << (y % 8));
	} else {
		*byte &= ~(1 << (y % 8));
	}
}

void NokiaLCD_SetScale(uint8_t scale) {
	nokiaLCD.scale = scale;
}

void NokiaLCD_WriteChar(char code) {
	// Set the pixels that compose the character
	register uint8_t x, y;
	uint8_t width = 5 * nokiaLCD.scale;
	uint8_t height = 7 * nokiaLCD.scale;
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			uint8_t bool = pgm_read_byte(&CHARSET[code-32][x/nokiaLCD.scale]) & (1 << y/nokiaLCD.scale);
			NokiaLCD_SetPixel(nokiaLCD.cursorX+x, nokiaLCD.cursorY+y, bool);
		}
	}
	
	// Move the cursor to the right. If out of bounds, move to the next line
	nokiaLCD.cursorX += width + 1;
	if (nokiaLCD.cursorX >= 84) {
		nokiaLCD.cursorX = 0;
		nokiaLCD.cursorY += height + 1;
	}
	if (nokiaLCD.cursorY >= 48) {
		nokiaLCD.cursorX = 0;
		nokiaLCD.cursorY = 0;
	}
}

void NokiaLCD_CustomBitmap(const uint8_t bitmap[20][3], int8_t xoffset, int8_t yoffset, uint8_t flipX) {
	const uint8_t ROWS = 20, COLS = 3;
	uint8_t col, row, bit;
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			for (bit = 0; bit < 8; bit++) {
				int8_t x = xoffset + (col*8) + bit;
				if (x < 0 || x >= 84) continue;
				int8_t y = yoffset + row;
				if (y < 0 || y >= 48) continue;
				uint8_t r = row;
				uint8_t c = (flipX) ? COLS-col-1 : col;
				uint8_t b = (flipX) ? bit : 7-bit;
				uint8_t value = (bitmap[r][c] >> b) & 0x01;
				NokiaLCD_SetPixel(x, y, value);
			}
		}
	}
}

void NokiaLCD_HealthBar(uint8_t xoffset, uint8_t yoffset, uint8_t health, uint8_t healthMax, uint8_t healthLost) {
	uint8_t barWidth = 40;
	uint8_t barHeight = 5;
	uint8_t healthWidth = (uint8_t)(((double) health / (double)healthMax) * (double)(barWidth));
	if (health > 0 && healthWidth < 1) healthWidth = 1;
	uint8_t healthLostWidth = (uint8_t)( (double)healthLost / (double)healthMax * (double)barWidth );
	if (healthLost > 0 && healthLostWidth < 1) healthLostWidth = 1;
	
	uint8_t row, col;
	for (row = 0; row < barHeight; row++) {
		for (col = 0; col < barWidth; col++) {
    		if (row == 0 || row == barHeight-1) {
				if (col != 0 && col != barWidth-1) {
					NokiaLCD_SetPixel(xoffset+col, yoffset+row, 1);
				}
			} else {
				if ((col >= 0 && col <= healthWidth) || (col == barWidth-1)) {
					NokiaLCD_SetPixel(xoffset+col, yoffset+row, 1);
				} else if (col <= healthWidth + healthLostWidth && ((row%2==0 && col%2==0) || (row%2==1 && col%2==1))) {
					NokiaLCD_SetPixel(xoffset+col, yoffset+row, 1);
				} else {
					NokiaLCD_SetPixel(xoffset+col, yoffset+row, 0);
				}
			}
		}
	}
}

void NokiaLCD_WriteString(const char* str) {
	while (*str) {
		NokiaLCD_WriteChar(*str++);
	}
}

void NokiaLCD_SetCursor(uint8_t x, uint8_t y) {
	nokiaLCD.cursorX = x;
	nokiaLCD.cursorY = y;
}

void NokiaLCD_SetLine(uint8_t line, uint8_t clearLine) {
	nokiaLCD.cursorX = 0;
	nokiaLCD.cursorY = (line - 1) * 8 * nokiaLCD.scale;
	
	if (clearLine) {
		uint8_t x,y;
		for (y = nokiaLCD.cursorY; y < 48; y++) {
			for (x = 0; x < 84; x++) {
				NokiaLCD_SetPixel(x,y, 0);
			}
		}
	}
}

void NokiaLCD_Render(void) {
	register unsigned i;
	// Set column and row to 0
	writeCommand(0x80);
	writeCommand(0x40);
	// Write screen to the Nokia LCD
	for (i = 0; i < 504; i++) {
		writeData(nokiaLCD.screen[i]);
	}
}