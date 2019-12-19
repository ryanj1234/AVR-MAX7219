#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include "MAX7219.h"

// Register Address Map
#define MAX2719_NOOP            0x00
#define MAX2719_DECODE_MODE     0x09
#define MAX2719_INTENSITY       0x0A
#define MAX2719_SCAN_LIMIT      0x0B
#define MAX2719_SHUTDOWN        0x0C
#define MAX2719_DISPLAY_TEST    0x0F

// Shutdown Register Definitions
#define MAX2719_SHUTDOWN_MODE   0x00
#define MAX2719_NORMAL_OPER     0x01

// Code B Definitions
#define MAX2719_CODEB_DASH      0x0A
#define MAX2719_CODEB_E         0x0B
#define MAX2719_CODEB_H         0x0C
#define MAX2719_CODEB_L         0x0D
#define MAX2719_CODEB_P         0x0E
#define MAX2719_CODEB_BLANK     0x0F

#define MOSI                    PINB3
#define MISO                    PINB4
#define SCK                     PINB5

#define CS_ENABLE()             PORTB &= ~(1 << MAX7219_CS)
#define CS_DISABLE()            PORTB |= (1 << MAX7219_CS)

const unsigned char seven_seg_digits_decode_abcdefg[75]= {
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
    0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x00, 0x00,
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x00, 0x4E, 0x00, 0x4F, 0x47, 0x5E,
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
    0x37, 0x06, 0x3C, 0x00, 0x0E, 0x00, 0x00, 0x7E, 0x67, 0x00, 0x00, 0x5B,
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
    0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
    0x00, 0x7D, 0x1F, 0x0D, 0x3D, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00,
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
    0x00, 0x00, 0x15, 0x1D, 0x00, 0x73, 0x05, 0x00, 0x0F, 0x1C, 0x00, 0x00,
/*  x     y     z     */
    0x00, 0x3B, 0x00
};

/* Invalid letters are mapped to all segments off (0x00). */
unsigned char decode_7seg(unsigned char chr)
{ /* assuming implementation uses ASCII */
    if (chr > (unsigned char)'z')
        return 0x00;
    return seven_seg_digits_decode_abcdefg[chr - '0'];
    /* or  seven_seg_digits_decode_gfedcba */
}

void MAX7219_write(uint8_t);
void MAX7219_write_register(uint8_t reg, uint8_t data);

void MAX7219_init()
{
    // set CS, MOSI and SCK to output
    DDRB |= (1 << MAX7219_CS) | (1 << MOSI) | (1 << SCK);

    // enable pull up resistor in MISO
    DDRB |= (1 << MISO);

    // enable SPI, set as master, and clock to fosc/128
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void MAX7219_default_init()
{
    MAX7219_init();

    MAX7219_set_codeb(0xFF);
    MAX7219_set_num_dig(MAX7219_NUM_DIG);
    MAX7219_set_intensity(MAX7219_INTENSITY_HALF);

    MAX7219_display_on();
    MAX7219_clear_display_codeb();
}

void MAX7219_write(uint8_t data)
{
	// Start transmission
	SPDR = data;

	// Wait for transmission complete
    loop_until_bit_is_set(SPSR, SPIF);
}

void MAX7219_write_register(uint8_t reg, uint8_t data)
{
    CS_ENABLE();

    MAX7219_write(reg);
    MAX7219_write(data);

    CS_DISABLE();
}

void MAX7219_clear_display_codeb()
{
    for(uint8_t i = 1; i <= MAX7219_NUM_DIG; i++)
    {
        MAX7219_write_register(i, MAX2719_CODEB_BLANK);
    }
}

void MAX7219_clear_display_none()
{
    for(uint8_t i = 1; i <= MAX7219_NUM_DIG; i++)
    {
        MAX7219_write_register(i, decode_7seg(' '));
    }
}

void MAX7219_display_on()
{
    MAX7219_write_register(MAX2719_SHUTDOWN, MAX2719_NORMAL_OPER);
}

void MAX7219_display_off()
{
    MAX7219_write_register(MAX2719_SHUTDOWN, MAX2719_SHUTDOWN_MODE);
}

void MAX2719_set_codeb(uint8_t dig_mask)
{
    MAX7219_write_register(MAX2719_DECODE_MODE, dig_mask);
}

void MAX7219_set_num_dig(uint8_t num_dig)
{
    MAX7219_write_register(MAX2719_SCAN_LIMIT, num_dig - 1);
}

void MAX7219_set_codeb(uint8_t dig_mask)
{
    MAX7219_write_register(MAX2719_DECODE_MODE, dig_mask);
}

void MAX7219_set_digit(uint8_t dig, uint8_t val)
{
    MAX7219_write_register(MAX7219_NUM_DIG - dig, val);
}

void MAX7219_set_intensity(uint8_t intensity)
{
    MAX7219_write_register(MAX2719_INTENSITY, intensity);
}

void print_string(const char *c, uint8_t offset)
{
    while((*c != 0) && (offset <= MAX7219_NUM_DIG))
    {
        MAX7219_set_digit(offset++, decode_7seg(*c++));
    }
}

#define DELAY_TIME  250

void sweep_string(const char *c)
{
    for(uint8_t i = MAX7219_NUM_DIG; i > 0; i--)
    {
        print_string(c, i);
        _delay_ms(DELAY_TIME);
        MAX7219_clear_display_none();
    }

    while(*c != 0)
    {
        print_string(c++, 0);
        _delay_ms(DELAY_TIME);
        MAX7219_clear_display_none();
    }
}

int main(void)
{
    MAX7219_default_init();
    MAX7219_set_codeb(0x00);
    MAX7219_clear_display_none();

    while(1)
    {
        sweep_string("HELLO");
    }
}
