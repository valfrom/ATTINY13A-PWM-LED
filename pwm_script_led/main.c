#define F_CPU 9600000UL // Define software reference clock for delay duration
 #include <avr/io.h>
 #include <util/delay.h>
#include <avr/interrupt.h>

#define ABSDIFF(a, b) ((a) < (b)? ((b) - (a)): ((a) - (b))) 

 #define LED PB0
#define SOUND PB3
 int i ;

volatile int timer_overflow_count = 0;
volatile  unsigned char dir = 0;

ISR(TIM0_OVF_vect) {
   if (++timer_overflow_count > 30) {   // a timer overflow occurs 4.6 times per second

	if ( dir == 0 )
	{
		OCR0A+=4;
		if (  OCR0A > 250 ) dir = 1 ;
	}
	else{
		OCR0A-=4;
		if ( OCR0A < 5 ) dir = 0;
	}

        timer_overflow_count = 0;
	}
   
}

void sound(char k, int l){
int ii = 0;
int i2 = 0;
 for(ii = 0 ; ii<425 - l;ii++) 
{
	PORTB &=~(1<<SOUND);
	  for(i2 = 0 ; i2<100-k*ii/8 ;i2++) //do this 100 times, so lowering this number gives a higher pitch!  __asm__("nop\n\t"); 
	PORTB |=(1<<SOUND);
	  for(i2 = 0 ; i2<100 -k* ii/8 ;i2++) //do this 100 times, so lowering this number gives a higher pitch!  __asm__("nop\n\t"); 
}
	 
}
 
void loopServo(){
 
	
	sound(1,0); 
PORTB |=(1<<PB4);
 
 sound(1,150); 
	OCR0A = 250;
	PORTB &=~(1<<PB4);
	_delay_ms(5);
	PORTB |=(1<<PB4);
	_delay_ms(50);
	PORTB &=~(1<<PB4);
 	_delay_ms(10);
sound(-1,-1250); 
}

 int main (void)
 { 
 DDRB |= (1 << PB4)|(1 << SOUND);
PORTB &=~(1<<PB4);
//init PWM

 DDRB |= (1 << LED); // OC0A on PB0
 
 TCCR0A |= (1 << COM0A1)|(1 << COM0A0)  // COM0A1 - COM0A0 (Set OC0A on Compare Match, clear OC0A at TOP)
 | (1 << WGM00) | (1 << WGM02); // WGM01 - WGM00 (set fast PWM)
 OCR0A = 100; // initialize Output Compare Register A to 0
 
TCCR0B |= (1 << CS01)  ; // Start timer at Fcpu / 256
 //TCCR0B |= (1<<CS02) | (1<<CS00);
 TIMSK0 |=1<<TOIE0;
   sei();

//init ADC
 ADCSRA |= (1 << ADEN)| // Analog-Digital enable bit
 (1 << ADPS1)| // set prescaler to 8 (clock / 8)
 (1 << ADPS0); // set prescaler to 8 (clock / 8)
 
 ADMUX |= (1 << ADLAR)| // AD result store in (more significant bit in ADCH)
 (1 << MUX0); // Choose AD input AD2 (BP 4)

unsigned char oldVal = 0;

sound(1,0); 



 for (;;)
 {
	//read add
	ADCSRA |= (1 << ADEN); // Analog-Digital enable bit
 	ADCSRA |= (1 << ADSC); // Discarte first conversion
 	while (ADCSRA & (1 << ADSC)); // wait until conversion is done
 	ADCSRA |= (1 << ADSC); // start single conversion
 	while (ADCSRA & (1 << ADSC)) // wait until conversion is done
 	ADCSRA &= ~(1<<ADEN); // shut down the ADC
	
	if ( ABSDIFF(ADCH,oldVal) > 3 ){
		loopServo();
		PORTB |=(1<<PB4);
		//int j = 0;
		//for (i = 5 ; i > 0 ; i-- ) 
 			
		PORTB &=~(1<<PB4);
		_delay_ms(10);
		PORTB |=(1<<PB4);
		_delay_ms(10);
		PORTB &=~(1<<PB4);
		_delay_ms(10);

	ADCSRA |= (1 << ADEN); // Analog-Digital enable bit
 	ADCSRA |= (1 << ADSC); // Discarte first conversion
 	while (ADCSRA & (1 << ADSC)); // wait until conversion is done
 	ADCSRA |= (1 << ADSC); // start single conversion
 	while (ADCSRA & (1 << ADSC)) // wait until conversion is done
 	ADCSRA &= ~(1<<ADEN); // shut down the ADC


		oldVal = ADCH;
	}
	_delay_ms(10);


 
 }
 }