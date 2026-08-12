/*=============================================================
 * Project : Bluetooth-Based Secure Locker with Access Logging
 * MCU     : NXP LPC2148 (ARM7TDMI-S)
 * IDE     : Keil MDK / uVision, Flash Magic for programming
 *
 * Overview
 * --------
 * A two-factor locker: a phone sends a Level-1 password over
 * Bluetooth (HC-05), and if it matches, the user must then enter a
 * Level-2 password on the physical keypad to actually open the
 * locker. An admin button (EINT1) gives access to a menu for
 * setting the RTC and changing either password. Every event is
 * timestamped and streamed to a PC over UART0 as a plain-text log.
 *
 * Hardware map
 * ------------
 *  LCD (8-bit) : DATA=P0.16-P0.23, RS=P0.4, RW=P0.7, EN=P0.5
 *  Keypad      : Rows=P1.16-P1.19, Cols=P1.20-P1.23
 *  UART0       : TXD=P0.0, RXD=P0.1   (PC activity log, 9600 baud)
 *  UART1       : TXD=P0.8, RXD=P0.9   (HC-05 Bluetooth, 9600 baud)
 *  I2C0/EEPROM : SCL=P0.2, SDA=P0.3   (AT24C256 on an HW-669 module)
 *  Motor L293D : IN1=P0.12, IN2=P0.13
 *  Buzzer      : P0.11
 *  Admin EINT1 : P0.14 (falling edge, 10K pull-up)
 *
 * EEPROM map  : 0x0000 = Level-1 Bluetooth password
 *               0x0010 = Level-2 keypad password
 * Default pwd : Level-1 = "1234"   Level-2 = "5678"
 *=============================================================*/

#include <LPC21xx.h>
#include "types.h"
#include "define.h"
#include "delay.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "uart0.h"
#include "uart1.h"
#include "kpm.h"
#include "eeprom.h"
#include "rtc.h"
#include "motor.h"
#include "eint.h"
#include "config.h"
#include <string.h>

/*-------------------------------------------------------------
 * StrMatch - true if two null-terminated strings are identical.
 * Used to compare an entered password against the one stored
 * in EEPROM.
 *-------------------------------------------------------------*/
u8 StrMatch(u8 *a, u8 *b)
{
	while(*a && *b)
		if(*a++ != *b++) return 0;
	return (*a == '\0' && *b == '\0');
}

/*-------------------------------------------------------------
 * DisplayStandbyscreen - idle-screen prompt shown on the LCD
 * whenever the locker is waiting for a new Bluetooth password.
 *-------------------------------------------------------------*/
void DisplayStandbyscreen(void)
{
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);  StrLCD((u8*)"BT Secure Locker");
	CmdLCD(GOTO_LINE2_POS0);  StrLCD((u8*)"Send BT Password");
}

/*-------------------------------------------------------------
 * DisplayAccessDenied - common "access denied" handler: logs the
 * failure reason with a timestamp, shows it on the LCD, sounds
 * the buzzer, then pauses briefly before the caller returns to
 * standby.
 *-------------------------------------------------------------*/
void DisplayAccessDenied(u8 *msg)
{

    /* Activity log */
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"ACCESS DENIED");
    UART0_Str((u8*)"Time   : ");
    RTC_LogTimestamp();

    UART0_Log((u8*)"Status : FAILED");
    UART0_Str((u8*)"Reason : ");
    UART0_Log(msg);

    UART0_Log((u8*)"System : Locker Locked");
    UART0_Log((u8*)"Action : Returning to Standby");
    UART0_Log((u8*)"========================================");

    /* LCD feedback */
    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"ACCESS DENIED");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD(msg);

    Buzzer_Beep();

    delay_ms(1500);

}

/*-------------------------------------------------------------
 * AdminEditRTC - admin sub-menu: prompts for hour, minute, second,
 * day, month and year (one field per screen, keypad entry with
 * '#'=confirm and '*'=backspace via GetNumber()), validates the
 * ranges, writes the new date/time to the RTC, then reads it back
 * to confirm the write succeeded before logging and returning.
 *-------------------------------------------------------------*/

void AdminEditRTC(void)
{
	s32 h, m, s, d, mo, y;
	s32 hh,mm,ss,dd,mth,yy;
  u8 num[5];

   UART0_Log((u8*)"========================================");
   UART0_Log((u8*)"RTC CONFIGURATION");
   UART0_Str((u8*)"Time   : ");
   RTC_LogTimestamp();
   UART0_Log((u8*)"Status : STARTED");
   UART0_Log((u8*)"========================================");

      /* ---- Hour (0-23) ---- */
      CmdLCD(CLEAR_LCD);
      CmdLCD(GOTO_LINE1_POS0);
      StrLCD((u8*)"Hour(0-23):");

      CmdLCD(GOTO_LINE2_POS0);

         GetNumber(num,3);

  if(strlen((char *)num) != 2)
 {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Hour");

    delay_ms(1500);
    return;
  }

   h = (num[0]-'0')*10 +
    (num[1]-'0');

	/* ---- Minute (0-59) ---- */
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Min(0-59):  ");
	CmdLCD(GOTO_LINE2_POS0);

	 GetNumber(num,3);

  if(strlen((char *)num) != 2)
  {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Minute");

    delay_ms(1500);
    return;
   }

    m = (num[0]-'0')*10 +
    (num[1]-'0');



	/* ---- Second (0-59) ---- */
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Sec(0-59):  ");
	CmdLCD(GOTO_LINE2_POS0);

	 GetNumber(num,3);

  if(strlen((char *)num) != 2)
  {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Second");

    delay_ms(1500);
    return;
 }

s = (num[0]-'0')*10 +
    (num[1]-'0');


	/* ---- Day (1-31) ---- */
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Day(1-31):  ");
	CmdLCD(GOTO_LINE2_POS0);

	GetNumber(num,3);

  if(strlen((char *)num) != 2)
  {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Day");

    delay_ms(1500);
    return;
  }

  d = (num[0]-'0')*10 +
    (num[1]-'0');

	/* ---- Month (1-12) ---- */
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Month(1-12):");
	CmdLCD(GOTO_LINE2_POS0);

	 GetNumber(num,3);

  if(strlen((char *)num) != 2)
  {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Month");

    delay_ms(1500);
    return;
  }

  mo = (num[0]-'0')*10 +
     (num[1]-'0');

	/* ---- Year (4 digits) ---- */
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Year(4 dig):");
	CmdLCD(GOTO_LINE2_POS0);

	GetNumber(num,5);

  if(strlen((char *)num) != 4)
  {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid Year");

    delay_ms(1500);
    return;
  }

  y = (num[0]-'0')*1000 +
    (num[1]-'0')*100 +
    (num[2]-'0')*10 +
    (num[3]-'0');

    /* ---- Range sanity check across all six fields ---- */
    if(h<0 || h>23 ||
   m<0 || m>59 ||
   s<0 || s>59 ||
   d<1 || d>31 ||
   mo<1 || mo>12 ||
   y<2000 || y>2099)

 {
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"Invalid RTC");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Update Failed");

    UART0_Log((u8*)"Status : FAILED");
    UART0_Log((u8*)"Reason : Invalid Date/Time");
    UART0_Log((u8*)"========================================");

    delay_ms(2000);
    return;
 }

       UART0_Str((u8*)"Entered Date : ");
UART0_TxNum(d);
UART0_TX('/');
UART0_TxNum(mo);
UART0_TX('/');
UART0_TxNum(y);
UART0_TX('\r');
UART0_TX('\n');

UART0_Str((u8*)"Entered Time : ");
UART0_TxNum(h);
UART0_TX(':');
UART0_TxNum(m);
UART0_TX(':');
UART0_TxNum(s);
UART0_TX('\r');
UART0_TX('\n');

	/* Commit to the RTC, then read it back to verify the write. */
	SetRTCTime(h, m, s);
	SetRTCDate(d, mo, y);

	delay_ms(50);

GetRTCTime(&hh,&mm,&ss);
GetRTCDate(&dd,&mth,&yy);

 if(h!=hh || m!=mm || s!=ss ||
   d!=dd || mo!=mth || y!=yy)
  {
    UART0_Log((u8*)"RTC Verification Failed");
  }

UART0_Log((u8*)"----- RTC VERIFY -----");

UART0_Str((u8*)"DATE : ");
UART0_TxNum(dd);
UART0_TX('/');
UART0_TxNum(mth);
UART0_TX('/');
UART0_TxNum(yy);
UART0_TX('\r');
UART0_TX('\n');

UART0_Str((u8*)"TIME : ");
UART0_TxNum(hh);
UART0_TX(':');
UART0_TxNum(mm);
UART0_TX(':');
UART0_TxNum(ss);
UART0_TX('\r');
UART0_TX('\n');

	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"RTC Updated ");

	CmdLCD(GOTO_LINE2_POS0);
  StrLCD((u8*)"Saved Successfully");

	UART0_Log((u8*)"Status : SUCCESS");
  UART0_Log((u8*)"RTC Updated Successfully");

   UART0_Str((u8*)"Updated Time : ");
   RTC_LogTimestamp();

UART0_Log((u8*)"========================================");


	delay_ms(1500);
}

/*-------------------------------------------------------------
 * ChangePassword - verifies the current password stored at addr,
 * then prompts for and saves a new one (with confirmation match).
 * Each keypad prompt has its own 10-second timeout; any timeout,
 * wrong old password, or new/confirm mismatch aborts without
 * touching EEPROM.
 *-------------------------------------------------------------*/
void ChangePassword(u32 addr)
{
	u8 old[PASSWORD_LENGTH], newp[PASSWORD_LENGTH], conf[PASSWORD_LENGTH], stored[PASSWORD_LENGTH];
   UART0_Log((u8*)"PASSWORD UPDATE STARTED");
   RTC_LogTimestamp();

	CmdLCD(CLEAR_LCD);
  CmdLCD(GOTO_LINE1_POS0);
  StrLCD((u8*)"Old Password:");
  CmdLCD(GOTO_LINE2_POS0);

if(!GetPwdTimeout(old, PASSWORD_LENGTH, 10000))
{
    UART0_Log((u8*)"PASSWORD CHANGE TIMEOUT");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"TIMEOUT");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Returning...");

    delay_ms(1500);
    return;
}

EE_ReadStr(addr, stored, PASSWORD_LENGTH);


	if(!StrMatch(old, stored))
	{
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Wrong Old Pwd!  ");
		CmdLCD(GOTO_LINE2_POS0); StrLCD((u8*)"Aborted.        ");

		UART0_Log((u8*)"Status : FAILED");
    UART0_Log((u8*)"Reason : Wrong Old Password");
		Buzzer_Beep();
		delay_ms(2000);
		return;
	}

	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"New Password:   ");
	CmdLCD(GOTO_LINE2_POS0);
	if(!GetPwdTimeout(newp, PASSWORD_LENGTH, 10000))
{
    UART0_Log((u8*)"PASSWORD CHANGE TIMEOUT");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"TIMEOUT");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Returning...");

    delay_ms(1500);
    return;
}

	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Confirm Pwd:    ");
	CmdLCD(GOTO_LINE2_POS0);

	if(!GetPwdTimeout(conf, PASSWORD_LENGTH, 10000))
{
    UART0_Log((u8*)"PASSWORD CHANGE TIMEOUT");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"TIMEOUT");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Returning...");

    delay_ms(1500);
    return;
}

	if(!StrMatch(newp, conf))
	{
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Mismatch!       ");
		CmdLCD(GOTO_LINE2_POS0); StrLCD((u8*)"Aborted.        ");

		UART0_Log((u8*)"Status : FAILED");
    UART0_Log((u8*)"Reason : Password Mismatch");
		delay_ms(2000);
		return;
	}

	EE_WriteStr(addr, newp);
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Password Saved! ");

  UART0_Log((u8*)"Status : SUCCESS");
  UART0_Log((u8*)"Password Updated");
	 RTC_LogTimestamp();
	delay_ms(1500);
}

/*-------------------------------------------------------------
 * AdminMenu - entry point for the admin flow, triggered by the
 * EINT1 button. Shows a "1.RTC 2.PWDS 3.EXIT" menu; if no key is
 * pressed within ~10 seconds the menu auto-exits back to standby.
 * Dispatches to AdminEditRTC() or the password sub-menu
 * (ChangePassword() for Level-1 Bluetooth or Level-2 keypad).
 *-------------------------------------------------------------*/
void AdminMenu(void)
{
    u8 ch;
    u32 t=0;
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"ADMIN CONFIGURATION MODE");
    UART0_Str((u8*)"Time   : ");
    RTC_LogTimestamp();

    UART0_Log((u8*)"Status : ENTERED");
    UART0_Log((u8*)"========================================");


	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD((u8*)"1.RTC 2.PWDS");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD((u8*)"3.EXIT");

	/* No key pressed within ~10 s (100 x 100 ms polls) -> bail out. */
	while(Colscan())
	{
	   delay_ms(100);
	   t++;
	   if(t>100)
	   {
	     CmdLCD(CLEAR_LCD);
		 DisplayStandbyscreen();
		  admin_flag=0;
		  return;
	   }
	}
	ch = KeyScanTimeout(10000);

	  if(ch==0)
	  {
	    CmdLCD(CLEAR_LCD);
		DisplayStandbyscreen();
		admin_flag=0;
		return;
	  }
  if(ch=='1')
  {
    UART0_Log((u8*)"Selected : RTC Configuration");
    AdminEditRTC();
  }
	else if(ch=='2')
  {
    UART0_Log((u8*)"Selected : Password Configuration");
		CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"1:L1-BT 2:L2-KP");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Wait 10 Sec...");

    ch = KeyScanTimeout(10000);

 if(ch == 0)
{
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"ADMIN PASSWORD MENU");
    UART0_Log((u8*)"Status : TIMEOUT");
    UART0_Log((u8*)"Reason : No Selection");
    UART0_Log((u8*)"Action : Returned to Standby");
    UART0_Log((u8*)"========================================");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"MENU TIMEOUT");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Returning...");

    delay_ms(1500);

    DisplayStandbyscreen();

    admin_flag = 0;
    return;
}

if(ch=='1')
{
    UART0_Log((u8*)"Password Type : Bluetooth");
    ChangePassword(L1_ADDR);
}
else if(ch=='2')
{
    UART0_Log((u8*)"Password Type : Keypad");
    ChangePassword(L2_ADDR);
}
else
{
    UART0_Log((u8*)"Invalid Password Menu Option");
}
}
	else
	{
		UART0_Log((u8*)"Exited Admin Mode");
    UART0_Log((u8*)"========================================");
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0); StrLCD((u8*)"Exiting...      ");
		delay_ms(1000);
	}
  UART0_Log((u8*)"----------------------------------------");
  UART0_Log((u8*)"ADMIN MODE EXIT");
  UART0_Str((u8*)"Time   : ");
  RTC_LogTimestamp();

 UART0_Log((u8*)"Status : EXITED");
 UART0_Log((u8*)"Returning to Standby");
 UART0_Log((u8*)"========================================");

  admin_flag=0;
}

/*-------------------------------------------------------------
 * InitializeSystem - bring up every peripheral driver in the
 * order the hardware needs: LCD first (for status messages),
 * then both UARTs, EEPROM (I2C), RTC, keypad, motor/buzzer, and
 * finally the admin-button interrupt.
 *-------------------------------------------------------------*/
void InitializeSystem(void)
{
    delay_ms(500);                 /* let supply rails settle before touching peripherals */

    InitLCD();          /* LCD data bus on P0.16-P0.23 (RS/RW/EN, see lcd_defines.h) */

	   UART0_Init();     /* activity log to PC, P0.0/P0.1 */

	   UART1_Init();     /* HC-05 Bluetooth, P0.8/P0.9, interrupt-driven RX */

	   EE_Init();        /* EEPROM over I2C0, P0.2/P0.3 */

	  Init_RTC();         /* on-chip RTC, 32 kHz crystal */

	     InitKPM();       /* 4x4 keypad, PORT1 P1.16-P1.23 */

         Motor_Init();    /* locker motor P0.12/P0.13, buzzer P0.11 */

  	    EINT1_Init();     /* admin button, P0.14 */
}

/*-------------------------------------------------------------
 * FirstBootInitialize - writes the two default passwords into
 * EEPROM. Not called from main() by default (see the commented-out
 * call below); intended to be run once manually the first time the
 * board's EEPROM is blank, then left disabled so it never
 * overwrites passwords the admin has since changed.
 *-------------------------------------------------------------*/
void FirstBootInitialize(void)
{
        EE_WriteStr(L1_ADDR, (u8*)DEFAULT_BT_PASSWORD);
        EE_WriteStr(L2_ADDR, (u8*)DEFAULT_KP_PASSWORD);

        UART0_Log((u8*)"[INIT] Default passwords written");

}
/*-------------------------------------------------------------
 * GrantAccess - both authentication levels passed: unlocks the
 * locker, holds it open for LOCKER_OPEN_TIME_MS, then re-locks it
 * and returns the LCD to the standby screen. Every step is logged
 * with a timestamp.
 *-------------------------------------------------------------*/
void GrantAccess(void)
{
   UART0_Log((u8*)"========================================");
   UART0_Log((u8*)"LOCKER ACCESS");
   UART0_Str((u8*)"Time : ");
   RTC_LogTimestamp();

   UART0_Log((u8*)"Status : SUCCESS");
   UART0_Log((u8*)"Action : Opening Locker");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"ACCESS GRANTED!");
	Buzzer_Beep();

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Opening Locker");

    Locker_Open();

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"LOCKER OPEN");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Closing in 5s");

    delay_ms(LOCKER_OPEN_TIME_MS);

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"Closing...");

	  UART0_Log((u8*)"========================================");
   UART0_Log((u8*)"LOCKER CLOSED");
   UART0_Str((u8*)"Time : ");
   RTC_LogTimestamp();



    Locker_Close();
   UART0_Log((u8*)"Action : Locker Closed");
   UART0_Log((u8*)"System : Returned to Standby");
   UART0_Log((u8*)"========================================");

    DisplayStandbyscreen();
}


/*=============================================================
 * MAIN
 *
 * Boot sequence, then an infinite loop that:
 *   1. Services the admin menu if the admin button was pressed.
 *   2. Waits (up to 30 s) for a Level-1 password over Bluetooth.
 *   3. On a Level-1 match, prompts for the Level-2 keypad password
 *      (10 s timeout).
 *   4. On both matching, calls GrantAccess() to open the locker;
 *      any failure/timeout at any stage logs the reason and loops
 *      back to standby.
 *=============================================================*/
int main(void)
{
  u8 bt_pwd[PASSWORD_LENGTH], kp_pwd[PASSWORD_LENGTH], stored[PASSWORD_LENGTH];
  u32 bt_wait = 0;
  u8 bt_wait_log = 0;

	InitializeSystem();

    /* FirstBootInitialize();  -- run once manually on a blank EEPROM, then leave disabled */

	CmdLCD(CLEAR_LCD);

   CmdLCD(GOTO_LINE1_POS0);
   StrLCD((u8*)"BT SECURE");

   CmdLCD(GOTO_LINE2_POS0);
   StrLCD((u8*)"LOCKER SYSTEM");

   delay_ms(1500);

   CmdLCD(CLEAR_LCD);

   CmdLCD(GOTO_LINE1_POS0);
   StrLCD((u8*)"Initializing");

   CmdLCD(GOTO_LINE2_POS0);
   StrLCD((u8*)"Please Wait...");

    delay_ms(1000);

	  DisplayStandbyscreen();

	while(1)
	{
		/* Admin button pressed? Service the admin menu, then resume standby. */
		if(admin_flag)
		{
			AdminMenu();
			DisplayStandbyscreen();
			continue;
		}

		/* Wait for the Bluetooth Level-1 password (30 s max). Log the
		 * "waiting" message only once per standby cycle. */
		if(bt_wait_log == 0)
   {
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"BLUETOOTH AUTHENTICATION");
    UART0_Str((u8*)"Time   : ");
    RTC_LogTimestamp();
    UART0_Log((u8*)"Status : WAITING");
    UART0_Log((u8*)"Waiting for Bluetooth Password...");
    UART0_Log((u8*)"========================================");

    bt_wait_log = 1;
   }



    bt_wait = 0;

   while(!UART1_Available())
  {
    delay_ms(100);
    bt_wait += 100;

    if(bt_wait >= 30000)
    {
        UART0_Log((u8*)"========================================");
        UART0_Log((u8*)"BLUETOOTH TIMEOUT");
        UART0_Str((u8*)"Time   : ");
        RTC_LogTimestamp();

        UART0_Log((u8*)"Status : FAILED");
        UART0_Log((u8*)"Reason : No Bluetooth Password Received");
        UART0_Log((u8*)"Action : Returned to Standby");
        UART0_Log((u8*)"========================================");

        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD((u8*)"BT TIMEOUT");

        CmdLCD(GOTO_LINE2_POS0);
        StrLCD((u8*)"Returning...");

        delay_ms(1500);

        DisplayStandbyscreen();

        bt_wait = 0;

          break;    /* exit the 30 s Bluetooth wait and re-loop to standby */

	}
}


    if(!UART1_Available())
    {
    continue;

    }



     if(!UART1_RxStr(bt_pwd,PASSWORD_LENGTH))
     {
       /* Oversized / malformed input on UART1 - reject and go back to standby. */
	   UART0_Log((u8*)"========================================");
     UART0_Log((u8*)"BLUETOOTH INPUT REJECTED");

        UART0_Str((u8*)"Time   : ");
        RTC_LogTimestamp();

		UART0_Str((u8*)"Status: Failed ");
		 UART0_Str((u8*)"Reason: password too much ");
		 UART0_Str((u8*)"Action:input discard ");

		  DisplayAccessDenied((u8*)"Invalid Bluetooth Password");
		   	DisplayStandbyscreen();


		   bt_wait = 0;
           bt_wait_log = 0;

			 continue;

		}

		 bt_wait = 0;
         bt_wait_log = 0;



		/* Verify the received string against the stored Level-1 password. */
		EE_ReadStr(L1_ADDR, stored, PASSWORD_LENGTH);

		if(!StrMatch(bt_pwd, stored))
		{  UART0_Log((u8*)"Status : FAILED");
       UART0_Log((u8*)"Reason : Invalid Bluetooth Password");
       UART0_Log((u8*)"Action : Access Denied");
       UART0_Log((u8*)"System : Returned to Standby");
       UART0_Log((u8*)"========================================");


			DisplayAccessDenied((u8*)"Invalid Bluetooth Password");
             DisplayStandbyscreen();

      bt_wait = 0;
      bt_wait_log = 0;

			 continue;

		}



		  UART0_Log((u8*)"Status : SUCCESS");
      UART0_Log((u8*)"Bluetooth Authentication Passed");
      UART0_Log((u8*)"Waiting for Level-2 Password");
      UART0_Log((u8*)"========================================");

		/* Level-1 passed: prompt for the Level-2 keypad password. */

		CmdLCD(CLEAR_LCD);

/* First screen */
   CmdLCD(GOTO_LINE1_POS0);
   StrLCD((u8*)"LEVEL-1 PASSED");

   CmdLCD(GOTO_LINE2_POS0);
   StrLCD((u8*)"Please Wait");


   delay_ms(1000);

 /* Second screen */
  CmdLCD(CLEAR_LCD);

  CmdLCD(GOTO_LINE1_POS0);
  StrLCD((u8*)"Enter L2 PWD");

  CmdLCD(GOTO_LINE2_POS0);     /* cursor at start of line 2 for the masked entry */


    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"LEVEL-2 AUTHENTICATION");
    UART0_Log((u8*)"Source : Keypad");
    UART0_Str((u8*)"Time   : ");
    RTC_LogTimestamp();


	/* Verify the Level-2 (keypad) password, 10 s to enter it. */

if(!GetPwdTimeout(kp_pwd, PASSWORD_LENGTH, 10000))
{
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"LEVEL-2 AUTHENTICATION");
    UART0_Log((u8*)"Status : FAILED");
    UART0_Log((u8*)"Reason : Timeout");
    UART0_Log((u8*)"Action : Returned to Standby");
    UART0_Log((u8*)"========================================");

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD((u8*)"LEVEL-2");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD((u8*)"TIMEOUT");

    delay_ms(2000);

    DisplayStandbyscreen();

	  bt_wait = 0;
    bt_wait_log = 0;

    continue;
}

EE_ReadStr(L2_ADDR, stored, PASSWORD_LENGTH);

if(!StrMatch(kp_pwd, stored))
{
    UART0_Log((u8*)"========================================");
    UART0_Log((u8*)"LEVEL-2 AUTHENTICATION");
    UART0_Log((u8*)"Status : FAILED");
    UART0_Log((u8*)"Reason : Wrong Keypad Password");
    UART0_Log((u8*)"Action : Access Denied");
    UART0_Log((u8*)"========================================");

    DisplayAccessDenied((u8*)"Wrong KP Pwd");

    DisplayStandbyscreen();
	  bt_wait = 0;
    bt_wait_log = 0;

    continue;
}

UART0_Log((u8*)"========================================");
UART0_Log((u8*)"LEVEL-2 AUTHENTICATION");
UART0_Log((u8*)"Status : SUCCESS");
UART0_Log((u8*)"Action : Opening Locker");
UART0_Log((u8*)"========================================");

/* Both factors verified - open the locker. */
GrantAccess();

	}
}
