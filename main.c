#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 1200000

#define PIN_A PB0
#define PIN_B PB1
#define PIN_C PB2

#define PIN_D PB3
#define PIN_E PB4
#define uchar unsigned char


void clear_leds()
{
	PORTB &= ~_BV(PIN_A);
	PORTB &= ~_BV(PIN_B);
	PORTB &= ~_BV(PIN_C);
	PORTB &= ~_BV(PIN_D);
}


void go_to_sleep()
{
	clear_leds();
	//           .vv
	MCUCR |= 0b00110000; // bit 5 - Sleep Enable; bits 4:3 - Sleep Mode
	asm("nop");	
	asm("sleep");
}



unsigned char demo_stage = 1;
unsigned char max_demo_stage = 1;
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

void arm_button1()
{
	is_button1_armed = 1;
}


void unarm_button1()
{
	is_button1_armed = 0;
}

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
				arm_button1();
				_delay_ms(1);
			}
		} else {
			if (is_button1_up()) {
				rotate_demo_stage();
				unarm_button1();
				_delay_ms(1);
			}
		}
}


void on(uchar a, uchar b, uchar c, uchar d, uchar period)
{
	clear_leds();

	if (a) {
		PORTB |= _BV(PIN_A);
	}

	if (b) {
		PORTB |= _BV(PIN_B);
	}

	if (c) {
		PORTB |= _BV(PIN_C);
	}

	if (d) {
		PORTB |= _BV(PIN_D);
	}

	uchar i;
	for (i = 0; i < period; i++) {
		_delay_ms(1);
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
	DDRB |= _BV(PIN_A);
	DDRB |= _BV(PIN_B);
	DDRB |= _BV(PIN_C);
	DDRB |= _BV(PIN_D);

	DDRB &= ~_BV(PIN_E); // input
	PORTB |= _BV(PIN_E);


	clear_leds();



	signed long counter_sleep = 15000; // ~ 10 seconds

		char speed = 4;

	while (1) {

		switch (demo_stage)
		{
			case 0:
				speed = 4;
				on(1, 0, 0, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 1, 0, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 0, 1, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 0, 0, 1, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 0, 1, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 1, 0, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				break;

			case 1:
				speed = 6;
				on(1, 1, 1, 1, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				on(0, 0, 0, 0, speed);
				if (demo_stage_changed) { demo_stage_changed = 0; break; }
				break;
		}


		counter_sleep--;
		if (counter_sleep < 0) {
			//go_to_sleep();
		}


	}

}
