#include <avr/io.h>
 #include <avr/sleep.h>
 #include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>


#ifndef F_CPU
#define F_CPU 128000UL
#endif

#define CENTER PB4
#define TLBR PB0
#define LR PB1
#define BLTR PB3

volatile uint8_t stop=0;


void show(uint8_t number){ //numBer to Dice 

	switch(number){
		case 1:
			PORTB &= ~(1 << CENTER);
			break;
		case 2:
			PORTB &= ~(1 << TLBR);
			break;
		case 3:
			PORTB &= ~((1 << BLTR)|(1 << CENTER));
			break;
		case 4:
			PORTB &= ~((1 << TLBR)|(1 << BLTR));
			break;
		case 5:
			PORTB &= ~((1 << TLBR)|(1 << BLTR)|(1 << CENTER));
			break;
		case 6:
			PORTB &= ~((1 << TLBR)|(1 << BLTR)|(1 << LR));
			break;
		default: 
			PORTB &= ~((1 << TLBR)|(1 << BLTR)|(1 << CENTER)|(1 << LR));
			break;
	}
}

void clear(){//clear dice
	PORTB |= ((1 << PB0)|(1 << PB1)|(1 << PB3)|(1 << PB4));
	}


void setup(){

  // Configure LED pins as output, turn LEDS off
  DDRB |= ((1 << CENTER)|(1 << TLBR)|(1 << BLTR)|(1 << LR));
  PORTB |= ((1 << CENTER)|(1 << TLBR)|(1 << BLTR)|(1 << LR));
  //configure Button pin as input with Pullup
  DDRB &= ~(1 << PB5);
  PORTB |= (1 << PB5);
	sleep_enable();
	PCMSK |= (1 << PCINT5); //Pin change interrupt mask for PB5 enabled
	GIMSK |= (1 << PCIE); //Pin change interrupt enable
}


void cast(void){
	TIMSK0= 0x00;
  	TCCR0B = 0x00;  //Timer-Stop
	TCNT0=0x00;
	while(!( PINB & ( 1<<PB5 ) )){
        uint8_t a=1;//rand()%6+1;
	clear();
	show(a);
	}

	stop=0;
	TCCR0B|=((1<<CS02)|(1<<CS00));  //Prescaler=1024, Timer-Start
	TIMSK0|=(1<<TOIE0);        //IRQ-Enable;
}





int main(void) {
	
	setup();
	sei();

    TCCR0B|=((1<<CS02)|(1<<CS00));  //Prescaler=1024, Timer-Start
	TIMSK0|=(1<<TOIE0);        //IRQ-Enable;
	show(7);
while(1){
	if(stop){
		clear();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  		sleep_mode();
		}		
	}
}

ISR(TIM0_OVF_vect){

	TIMSK0= 0x00;
  	TCCR0B = 0x00;  //Timer-Stop
	stop=1;
}

ISR(PCINT0_vect){
	cli();
	cast();
	sei();
}

