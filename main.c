#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 1200000

#define PIN_A PB0
#define PIN_B PB1
#define PIN_C PB2

#define PIN_D PB3
#define PIN_E PB4
#define uchar unsigned char

#define BB if (demo_stage_changed) { break; }

uchar i;

void clear_leds()
{
	PORTB &= 0b11110000;
}

#define COUNTER_SLEEP_DEFAULT 2000

signed short int counter_sleep = COUNTER_SLEEP_DEFAULT;

void reset_counter_sleep()
{
	counter_sleep = COUNTER_SLEEP_DEFAULT;
}

void go_to_sleep()
{
	clear_leds();
	//           .vv
	MCUCR |= 0b00110000; // bit 5 - Sleep Enable; bits 4:3 - Sleep Mode
	asm("nop");	
	asm("sleep");
}


unsigned char on_period;

unsigned char demo_stage = 0;
unsigned char max_demo_stage = 11;
unsigned char demo_stage_changed = 0;
	
void rotate_demo_stage()
{
	demo_stage++;
	if (demo_stage > max_demo_stage) {
		demo_stage = 0;
	}

	demo_stage_changed = 1;
}


char is_button1_armed = 0;

char is_button1_up()
{
	return (PINB & _BV(PIN_E));
}

char is_button1_down()
{
	return ! is_button1_up();
}


void handle_button()
{
		if (! is_button1_armed) {
			if (is_button1_down()) {
				is_button1_armed = 1;
				_delay_us(100);
			}
		} else {
			if (is_button1_up()) {
				reset_counter_sleep();
				rotate_demo_stage();
				is_button1_armed = 0;
				_delay_us(100);
			}
		}
}


void on(uchar leds_bitmask)
{
	clear_leds();

	if (leds_bitmask & 0b1000) {
		PORTB |= _BV(PIN_A);
	}

	if (leds_bitmask & 0b0100) {
		PORTB |= _BV(PIN_B);
	}

	if (leds_bitmask & 0b0010) {
		PORTB |= _BV(PIN_C);
	}

	if (leds_bitmask & 0b0001) {
		PORTB |= _BV(PIN_D);
	}

	for (i = 0; i < on_period; i++) {
		_delay_us(800);
		handle_button();
	}
}

/*** MAIN ***/

int __attribute__((noreturn)) main(void)
{
	//        ....    
	CLKPR = 0b10000000; // (bit 7) Clock Prescaler Change Enable
	CLKPR = 0b00001000; // Clock Division Factor (prescaler) 256


	// diretions of pins
	DDRB |= 0b00001111;

	DDRB &= ~_BV(PIN_E); // input
	PORTB |= _BV(PIN_E);


	cli();
	TCCR0B |= (1 << CS00);
	TIMSK0 |= (1 << TOIE0);
	TCNT0 = 0;
	sei();


	while (1) {

		switch (demo_stage)
		{
				case 0:
					on_period = 5;
					on(0b0010); BB;
					on(0b0000); BB;
					on(0b0100); BB;
					on(0b0000); BB;
					on(0b0001); BB;
					on(0b0000); BB;
					on(0b1000); BB;
					on(0b0000); BB;
					break;

				case 1:
					on_period = 9;
					on(0b0101); BB;
					on(0b1010); BB;
					break;

				case 2:
					on_period = 8;
					on(0b1100); BB;
					on(0b0011); BB;
					break;

				case 3:
					on_period = 4;
					on(0b1000); BB;
					on(0b0100); BB;
					on(0b0010); BB;
					on(0b0001); BB;
					on(0b0000); BB;
					break;

				case 4:
					on_period = 3;
					on(0b1000); BB;
					on(0b1100); BB;
					on(0b1110); BB;
					on(0b1111); BB;
					on(0b1111); BB;
					on(0b0111); BB;
					on(0b0011); BB;
					on(0b0001); BB;
					on(0b0000); BB;
					break;

				case 5:
					on_period = 4;
					on(0b1000); BB;
					on(0b0100); BB;
					on(0b0010); BB;
					on(0b0001); BB;
					on(0b0010); BB;
					on(0b0100); BB;
					break;

				case 6:
					on_period = 3;
					on(0b1000); BB;
					on(0b1100); BB;
					on(0b1110); BB;
					on(0b1111); BB;
					on(0b1111); BB;
					on(0b1110); BB;
					on(0b1100); BB;
					on(0b1000); BB;
					on(0b0000); BB;
					break;

				case 7:
					on_period = 8;
					on(0b1111);
					on(0b0000); BB;
					break;

				case 8:
					on_period = 5;
					on(0b1111);
					on(0b0000); BB;
					break;

				case 9:
					on_period = 4;
					on(0b1111);
					on(0b0000); BB;
					break;

				case 10:
					on_period = 2;
					on(0b1111);
					on(0b0000); BB;
					break;

				case 11:
					on_period = 1;
					on(0b0111);
					on(0b1011);
					on(0b1101);
					on(0b1110);
					on(0b1111);
					on(0b1111);
					on(0b1111);
					on(0b1111);
					on(0b1111);
					on(0b1111);
					on(0b1111);
					on(0b1111); BB;
					break;

		}

		demo_stage_changed = 0;


		if (counter_sleep < 0) {
			go_to_sleep();
		}


	}

}

/**
 *  * Called when TIMER0 overflows which is
 *   * ca. F_CPU / 256 times a second (without timer prescaling).
 *    */
ISR(TIM0_OVF_vect)
{
	counter_sleep--;
}
