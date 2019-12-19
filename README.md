# AVR-MAX7219
AVR driver code for a MAX7219

## Connection
Connect DIN to MOSI and CS to MAX7219_CS (defined as PINB2 in header).  
When programming with ISP it sends display into state where all LEDs are lit. Need to power cycle for it to work.

## Notes about operation
There are two types of coding - BCD code B (0-9, E, H, L, P and -) or no decoding. For digits only, use code-b decoding, for individual control of segments use no decoding.
'''c
// no codeb
MAX7219_set_codeb(0x00);

// all codeb
MAX7219_set_codeb(0x00);
'''
