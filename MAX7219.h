#ifndef _MAX7219_H_
#define _MAX7219_H_

#define MAX7219_CS          PINB2
#define MAX7219_NUM_DIG     8

#define MAX7219_INTENSITY_NONE  0x00
#define MAX7219_INTENSITY_HALF  0x08
#define MAX7219_INTENSITY_FULL  0x0F

void MAX7219_init(void);
void MAX7219_default_init(void);
void MAX7219_display_on(void);
void MAX7219_display_off(void);
void MAX7219_clear_display_codeb(void);
void MAX7219_clear_display_none(void);
void MAX7219_set_codeb(uint8_t dig_mask);
void MAX7219_set_num_dig(uint8_t num_dig);
void MAX7219_set_digit(uint8_t dig, uint8_t val);
void MAX7219_set_intensity(uint8_t intensity);

#endif
