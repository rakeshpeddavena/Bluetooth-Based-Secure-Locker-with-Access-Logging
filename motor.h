/*
 * motor.h - locker motor (L293D H-bridge) and buzzer driver
 */
#ifndef MOTOR_H
#define MOTOR_H

void Motor_Init(void);
void Locker_Open(void);
void Locker_Close(void);
void Buzzer_Beep(void);

#endif /* MOTOR_H */
