/*
 * motor.c
 * -----------------------------------------------------------------------
 * Drives the L293D H-bridge that operates the locker's DC motor, plus
 * the buzzer used for access-granted/denied feedback.
 *
 *   L293D : IN1 = P0.12, IN2 = P0.13, EN1 tied HIGH on the board
 *           IN1=1, IN2=0 -> forward (locker OPEN)
 *           IN1=0, IN2=1 -> reverse (locker CLOSE)
 *   Buzzer: P0.11, active HIGH
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "define.h"
#include "delay.h"
#include "motor.h"

#define BUZZER  15    /* physical buzzer pin used on this board (P0.15) */
#define MOTOR1  12
#define MOTOR2  13

/* Configure motor and buzzer pins as outputs, all initially off. */
void Motor_Init(void)
{
	SETBIT(IODIR0, BUZZER);
	SETBIT(IODIR0, MOTOR1);
	SETBIT(IODIR0, MOTOR2);
	CLRBIT(IOPIN0, BUZZER);
	CLRBIT(IOPIN0, MOTOR1);
	CLRBIT(IOPIN0, MOTOR2);
}

/* Drive the motor forward for a fixed time to unlock, then stop. */
void Locker_Open(void)
{
	IOSET0 = (1 << MOTOR1);
	IOCLR0 = (1 << MOTOR2);                   /* IN1=1, IN2=0 -> forward */
	delay_ms(2000);
	IOCLR0 = (1 << MOTOR1) | (1 << MOTOR2);   /* stop (both low)         */
}

/* Drive the motor in reverse for a fixed time to lock, then stop. */
void Locker_Close(void)
{
	IOCLR0 = (1 << MOTOR1);
	IOSET0 = (1 << MOTOR2);                   /* IN1=0, IN2=1 -> reverse */
	delay_ms(2000);
	IOCLR0 = (1 << MOTOR1) | (1 << MOTOR2);   /* stop */
}

/* Sound the buzzer with 5 short beeps, used for both access-denied and
 * access-granted feedback. */
void Buzzer_Beep(void)
{
	u8 i;
	for(i = 0; i < 5; i++)
	{
		IOSET0 = (1 << BUZZER);  delay_ms(200);
		IOCLR0 = (1 << BUZZER);  delay_ms(200);
	}
}
