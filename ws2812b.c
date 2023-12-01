#include "ws2812b.h"

void __attribute__((noinline)) update_led_strip(color* colors, uint16_t number_of_leds)
{	
#if ( __AVR_ARCH__ >= 100 )

	//set pin as output
	LED_PORT.DIRSET = (1 << LED_PIN);
	
	//pointer to io base memory address
	uint8_t* led_strip_port_pointer;
	led_strip_port_pointer = (uint8_t*)(&LED_PORT);
	
	uint8_t pin = (1 << LED_PIN);

	cli();
	while(number_of_leds--)
	{
		asm volatile(
		
		//send red component
		"ld		__tmp_reg__,	%a0+		\n\t"
		"ld		__tmp_reg__,	%a0			\n\t"
		"rcall	send_byte%=					\n\t"
		
		//send green component
		"ld		__tmp_reg__,	-%a0		\n\t"
		"rcall	send_byte%=					\n\t"
		
		//send blue component
		"ld		__tmp_reg__,	%a0+		\n\t"
		"ld		__tmp_reg__,	%a0+		\n\t"
		"ld		__tmp_reg__,	%a0+		\n\t"
		"rcall	send_byte%=					\n\t"
		"rjmp	asm_end%=					\n\t"

		//send byte subroutine
		"send_byte%=:						\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"rcall	send_bit%=					\n\t"
		"ret								\n\t"

		//send bit subroutine
		"send_bit%=:						\n\t"
		
	#if F_CPU == 8000000
		"rol	__tmp_reg__					\n\t"
	#endif
		//set outout high
		"std	Y+5,	%[led_strip_pin]	\n\t"

	#if F_CPU != 8000000
		"rol	__tmp_reg__					\n\t"
	#endif

	#if F_CPU == 16000000
		"nop								\n\t"
		"nop								\n\t"
	#elif F_CPU == 20000000
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
	#endif
		"brcs	.+2							\n\t"
		//set outout low
		"std	Y+6,	%[led_strip_pin]	\n\t"
	#if F_CPU == 8000000
		"nop								\n\t"
		"nop								\n\t"
	#elif F_CPU == 16000000
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
	#elif F_CPU == 20000000
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
		"nop								\n\t"
	#endif
		"brcc	.+2							\n\t"
		//set outout low
		"std	Y+6,	%[led_strip_pin]	\n\t"
		
		"ret								\n"
		"asm_end%=:"
		
		//constraints
		: "=b" (colors)																//output operands
		: "0" (colors), [led_strip_pin] "r" (pin), "y" (led_strip_port_pointer));	//input operands
	}
	
	//enable interrupt
	sei();
	
	//send reset pulse
	_delay_us(80);

#else
	
	//set led pin as an output
	LED_STRIP_PORT &= ~(1 << LED_STRIP_PIN);
	LED_STRIP_DDR |= (1 << LED_STRIP_PIN);

	cli();
	while(number_of_leds--)
	{
		asm volatile(
		
		//send red component
		"ld		__tmp_reg__,	%a0+					\n\t"
		"ld		__tmp_reg__,	%a0						\n\t"
		"rcall	send_byte%=								\n\t"
		
		//send green component
		"ld		__tmp_reg__,	-%a0					\n\t"
		"rcall	send_byte%=								\n\t"
		
		//send blue component
		"ld		__tmp_reg__,	%a0+					\n\t"
		"ld		__tmp_reg__,	%a0+					\n\t"
		"ld		__tmp_reg__,	%a0+					\n\t"
		"rcall	send_byte%=								\n\t"
		"rjmp	asm_end%=								\n\t"

		//send byte subroutine
		"send_byte%=:									\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"rcall	send_bit%=								\n\t"
		"ret											\n\t"

		//send bit subroutine
		"send_bit%=:									\n\t"
		
		#if F_CPU == 8000000
		"rol	__tmp_reg__								\n\t"
		#endif
		//set outout high
		"sbi	%[led_strip_port],	%[led_strip_pin]	\n\t"

		#if F_CPU != 8000000
		"rol	__tmp_reg__								\n\t"
		#endif

		#if F_CPU == 16000000
		"nop											\n\t"
		"nop											\n\t"
		#elif F_CPU == 20000000
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		#endif
		"brcs	.+2										\n\t"
		//set outout low
		"cbi	%[led_strip_port],	%[led_strip_pin]	\n\t"
		#if F_CPU == 8000000
		"nop											\n\t"
		"nop											\n\t"
		#elif F_CPU == 16000000
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		#elif F_CPU == 20000000
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		"nop											\n\t"
		#endif
		"brcc	.+2										\n\t"
		//set outout low
		"cbi	%[led_strip_port],	%[led_strip_pin]	\n\t"
		
		"ret											\n\t"
		"asm_end%=:"
		
		//constraints
		: "=b" (colors)																								//output operands
		: "0" (colors), [led_strip_port] "I" (_SFR_IO_ADDR(LED_STRIP_PORT)), [led_strip_pin] "I" (LED_STRIP_PIN));	//input operands
	}
	
	//enable interrupt
	sei();
	
	//send reset pulse
	_delay_us(80);
#endif
}