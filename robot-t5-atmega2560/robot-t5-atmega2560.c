/*
▪ * Team Id: 34
▪ * Author List: Abdul Khaliq Almel,Shashank K Holla,Amrathesh,Naman Jain
▪ * Filename: robot-t5-atmega2560.c
▪ * Theme: Rapid Rescuer(RR)
▪ * Functions: init_switch_interrupt,ISR(INT7_vect),getDis,goStraight,turnLeft,turnRight,follow_wall,follow_wall2
               readLine
▪ * Global Variables: start,addr,Sensor,shift,rx-byte
 */

#define F_CPU 16000000

#include <avr/io.h>		   // Standard AVR IO Library
#include <util/delay.h>	// Standard AVR Delay Library
#include <avr/interrupt.h> // Standard AVR Interrupt Library
#include "uart.h"		   // Third Party UART Library
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mcp23017.h" // LFA

//distance sensors
// define the XHSUT pins for 3 ToF sensors
#define xshut_1 PA5
#define xshut_2 PA3
#define xshut_3 PA1
#define PIN_USER_SW PE7			 // Macro for Pin Number of User Switch

volatile unsigned int count = 0; // Used in ISR of Timer2 to store ms elasped
unsigned int seconds = 0;		 // Stores seconds elasped
char rx_byte;
int start = 1;

uint8_t addr[3] = {22, 24, 26};
uint16_t Sensor[3];
// array with new addresses for 3 TOF Sensors

//MOtor definitions
#define ML1 PL3
#define ML2 PH4
#define MR1 PL5
#define MR2 PL4
#define i_delay 5

//Led related functions
#define PIN_LED_RED PH3
#define PIN_LED_GREEN PH5
#define PIN_LED_BLUE PH4

int left, right, mid, ans; //to store the left,right,mid,distance whenever required
char buffer[100];
char distanceString[100];
char myStr[100];
char recvFrom[200];
char toSend[40];
int shift = 0;
char rx_byte;

int speed_left = 155;  //speed factor for left wheel
int speed_right = 155; //speed factor for right wheel
int f_correction = 10; //used in iterative follow_wall function
int l_correction = 10; //used in iterative follow_wall function
int r_correction = 10; //used in iterative follow_wall function
int turn_left = 40;	//used in turnLeft function for the initial random turn
int turn_right = 40;   //used in turnRight fucntion for the initial random turn

void init_speedfactors(int a, int b, int p, int q, int r, int x, int y)
{
	speed_left = a;
	speed_right = b;
	f_correction = p;
	l_correction = q;
	r_correction = r;
	turn_left = x;
	turn_right = y;
}

void init_button(void)
{
	DDRE &= ~(1 << PIN_USER_SW); // Make PIN_USER_SW input
	PORTE |= (1 << PIN_USER_SW); // Turn on Internal Pull-Up resistor of PIN_USER_SW (Optional)
}

/*
▪ * Function Name: init_switch_interrupt
▪ * Input: none
▪ * Output: none
▪ * Example Call: init_switch_interrupt(void)
▪ */

void init_switch_interrupt(void)
{

	// all interrupts have to be disabled before configuring interrupts
	cli(); // Disable Interrupts Globally

	EIMSK |= (1 << INT7); // Turn ON INT7 (alternative function of PE7 i.e Button Pin)

	EICRB |= (1 << ISC71); // Falling Edge detection on INT7
	EICRB &= ~(1 << ISC70);

	sei(); // Enable Interrupts Gloabally
}

// Interrupt Servie Routine of INT7
ISR(INT7_vect)
{
	uart0_puts(" @started@\0");
	start = 0;
}

/*******************************************************************************************************************************************************
 *************************************************************** TIMER SECTION *************************************************************************
 *******************************************************************************************************************************************************           
 * */

// Timer 4 initialized in PWM mode for brightness control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz

void timer4_init()
{
	cli(); //disable all interrupts

	TCCR4B = 0x00; //Stop

	TCNT4H = 0xFF; //Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT4L = 0x00; //Counter lower 8-bit value to which OCR5xH value is compared with

	OCR4AH = 0x00; //Output compare register high value for Red Led
	OCR4AL = 0x00; //Output compare register low value for Red Led

	OCR4BH = 0x00; //Output compare register high value for Blue Led
	OCR4BL = 0x00; //Output compare register low value for Blue Led

	OCR4CH = 0x00; //Output compare register high value for Green Led
	OCR4CL = 0x00; //Output compare register low value for Green Led

	//  Clear OC4A, OC4B & OC4C on compare match (set output to low level)
	TCCR4A |= (1 << COM4A1) | (1 << COM4B1) | (1 << COM4C1);
	TCCR4A &= ~((1 << COM4A0) | (1 << COM4B0) | (1 << COM4C0));

	// FAST PWM 8-bit Mode
	TCCR4A |= (1 << WGM40);
	TCCR4A &= ~(1 << WGM41);
	TCCR4B |= (1 << WGM42);

	// Set Prescalar to 64
	TCCR4B |= (1 << CS41) | (1 << CS40);
	TCCR4B &= ~(1 << CS42);

	sei(); //re-enable interrupts
}

void timer5_init()
{
	cli(); //disable all interrupts

	TCCR5B = 0x00; //Stop

	TCNT5H = 0xFF; //Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x00; //Counter lower 8-bit value to which OCR5xH value is compared with

	OCR5AH = 0x00; //Output compare register high value for Red Led
	OCR5AL = 0x00; //Output compare register low value for Red Led

	OCR5BH = 0x00; //Output compare register high value for Blue Led
	OCR5BL = 0x00; //Output compare register low value for Blue Led

	OCR5CH = 0x00; //Output compare register high value for Green Led
	OCR5CL = 0x00; //Output compare register low value for Green Led

	//  Clear OC4A, OC4B & OC4C on compare match (set output to low level)
	TCCR5A |= (1 << COM5A1) | (1 << COM5B1) | (1 << COM5C1);
	TCCR5A &= ~((1 << COM5A0) | (1 << COM5B0) | (1 << COM5C0));

	// FAST PWM 8-bit Mode
	TCCR5A |= (1 << WGM50);
	TCCR5A &= ~(1 << WGM51);
	TCCR5B |= (1 << WGM52);

	// Set Prescalar to 64
	TCCR5B |= (1 << CS51) | (1 << CS50);
	TCCR5B &= ~(1 << CS52);

	sei(); //re-enable interrupts
}

void timer0_init()
{
	cli(); //disable all interrupts

	TCCR0B = 0x00; //Stop

	TCNT0 = 0xFF; //Counter higher 8-bit value to which OCR5xH value is compared with
	//TCNT0L = 0x00;	//Counter lower 8-bit value to which OCR5xH value is compared with

	OCR0A = 0x00; //Output compare register high value for Red Led
	//OCR0AL = 0x00;	//Output compare register low value for Red Led

	OCR0B = 0x00; //Output compare register high value for Blue Led
	//OCR0BL = 0x00;	//Output compare register low value for Blue Led

	//OCR0C = 0x00;	//Output compare register high value for Green Led
	//OCR0CL = 0x00;	//Output compare register low value for Green Led

	//  Clear OC4A, OC4B & OC4C on compare match (set output to low level)
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
	TCCR0A &= ~((1 << COM0A0) | (1 << COM0B0));

	// FAST PWM 8-bit Mode
	TCCR0A |= (1 << WGM00);
	TCCR0A &= ~(1 << WGM01);
	TCCR0B |= (1 << WGM02);

	// Set Prescalar to 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	TCCR0B &= ~(1 << CS02);

	sei(); //re-enable interrupts
}

/******************************************************************END OF ********************************************************************************
 *************************************************************** TIMER SECTION ***************************************************************************
 *********************************************************************************************************************************************************           
 * */

/*********************************************************************************************************************************************************
 *************************************************************** READ form UART **************************************************************************
 *********************************************************************************************************************************************************           
 * */

//readLine
char uart0_readByte(void)
{

	uint16_t rx;
	uint8_t rx_status, rx_data;

	rx = uart0_getc();
	rx_status = (uint8_t)(rx >> 8);
	rx = rx << 8;
	rx_data = (uint8_t)(rx >> 8);

	if (rx_status == 0 && rx_data != 0)
	{
		return rx_data;
	}
	else
	{
		return -1;
	}
}

/*
▪ * Function Name:readLine
▪ * Input: readString [char *]
▪ * Output:none
▪ * Example Call: readLine(char *readString);
▪ */

void readLine(char *readString)
{
	char c = uart0_readByte();
	char strx[50];
	int i = 0;
	while (1)
	{

		if (c != -1)
		{
			//uart0_putc(c);
			strx[i++] = c;
			if (c == '#' && i > 2)
				break;
		}

		c = uart0_readByte();
	}
	strcpy(readString, strx);
}

/*******************************************************************END OF*****************************************************************************
 *************************************************************** READ form UART ***********************************************************************
 ******************************************************************************************************************************************************           
 * */

/***************************************************************************************************************************************************
 *************************************************************** RGB LED ***************************************************************************
 ***************************************************************************************************************************************************           
 * */

void led_init(void)
{
	DDRH |= (1 << PIN_LED_RED) | (1 << PIN_LED_GREEN) | (1 << PIN_LED_BLUE);
	PORTH |= (1 << PIN_LED_RED) | (1 << PIN_LED_GREEN) | (1 << PIN_LED_BLUE);
	OCR4AL = 255;
	OCR4CL = 255;
	OCR4BL = 255;
	OCR5AL = 255;
}

void led_redOn(void)
{
	OCR4AL = 0;
	OCR4CL = 255;
	OCR4BL = 255;
	OCR5AL = 255;
}

void led_redOff(void)
{
	OCR4AL = 255;
	OCR4CL = 255;
	OCR4BL = 255;
	OCR5AL = 255;
}

void led_greenOn(void)
{
	OCR4AL = 255;
	OCR4CL = 0;
	OCR4BL = 255;
	OCR5AL = 255;
}

void led_greenOff(void)
{
	OCR4AL = 255;
	OCR4CL = 255;
	OCR4BL = 255;
	OCR5AL = 255;
}

void blink_green(int count, int delay)
{
	int kk;
	int d;
	for (kk = 0; kk < count; kk++)
	{
		led_greenOn();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
		led_greenOff();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
	}
}
void blink_red(int count, int delay)
{
	int kk;
	int d;
	for (kk = 0; kk < count; kk++)
	{
		led_redOn();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
		led_greenOff();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
	}
}
void blink(int count, int delay)
{
	int kk;
	int d;
	for (kk = 0; kk < count; kk++)
	{
		led_greenOn();
		led_redOn();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
		led_greenOff();
		for (d = 0; d < delay; d++)
		{
			_delay_ms(100);
		}
	}
}

/*****************************************************************END OF****************************************************************************
 *************************************************************** RGB LED ***************************************************************************
 ***************************************************************************************************************************************************           
 * */

/***************************************************************************************************************************************************
 *************************************************************** DISTANCE SENSORS ******************************************************************
 ***************************************************************************************************************************************************           
 * */

// function to configure the XSHUT pins of 3 TOF sensors
void init_Xshut_Low(void)
{
	DDRA |= ((1 << xshut_1) | (1 << xshut_2) | (1 << xshut_3));
	PORTA &= ~((1 << xshut_1) | (1 << xshut_2) | (1 << xshut_3));
}

// initialize I2C, MILLIS and UART
void init_i2c_uart(void)
{
	i2c_init();
	initMillis();

	uart0_init(UART_BAUD_SELECT(115200, F_CPU)); // initialize UART0 with baud rate of 115200
	uart0_flush();
	uart0_puts("*** STARTED ***\n");

	sei();
}

// initialise all 3 TOF sensors and change their default address
void init_Sensor_Vl53l0x()
{
	init_Xshut_Low();

	DDRA &= ~(1 << xshut_1);
	// initialize First VL53L0X sensor
	initVL53L0X(1);
	// Change the address to "addr[0] = 22"
	setAddress(addr[0]);

	DDRA &= ~(1 << xshut_2);
	// initialize Second VL53L0X sensor
	initVL53L0X(1);
	// Change the address to "addr[1] = 24"
	setAddress(addr[1]);

	DDRA &= ~(1 << xshut_3);
	// initialize Third VL53L0X sensor
	initVL53L0X(1);
	// Change the address to "addr[0] = 26"
	setAddress(addr[2]);
}

/*
▪ * Function Name:getDis
▪ * Input:  sensor number
▪ * Output: distance_in_mm
▪ * Example Call: dist = getDis(sens_no)
▪ */
uint16_t getDis(int i)
{
	Select_ToF_Addr(addr[i]);
	return readRangeSingleMillimeters(0);
}

void getDistance(char *tx_str)
{

	sprintf(tx_str, "Sensor_1:%d \nSensor_2:%d \nSensor_3:%d \n", getDis(0), getDis(1), getDis(2));
	//sprintf(msg, "\n @{tof1:%d (%d) #### tof2: %d (%d) #### tof3: %d (%d) \n right-left : %d (%d)}", d1,d1%200,d2,d2%200,d3,d3%100,d1-d3,(d1%200)-(d3%200));
}
int *d_vector()
{
	int Sensor_no = 0;

	init_i2c_uart();
	init_Sensor_Vl53l0x();

	while (1)
	{
		Select_ToF_Addr(addr[Sensor_no]);				   //Select the TOF whose reading is required
		Sensor[Sensor_no] = readRangeSingleMillimeters(0); // take the sensor readings from the sensor
		Sensor_no = Sensor_no + 1;
		if (Sensor_no == 3)
		{
			Sensor_no = 0;
			return (Sensor);
		}
	}
}

/*********************************************************************END OF*****************************************************************************
 *************************************************************** DISTANCE SENSORS ***********************************************************************
 ********************************************************************************************************************************************************           
 * */

/********************************************************************************************************************************************************
 *************************************************************** MOTORS *********************************************************************************
 ********************************************************************************************************************************************************           
 * */
void init_Motor(void)
{

	DDRL |= (1 << ML1) | (1 << MR1) | (1 << MR2);
	PORTL |= (1 << ML1) | (1 << MR1) | (1 << MR2);
	DDRH |= (1 << ML2);
	PORTH |= (1 << ML2);
}

//Standard functions for stop
void stopMotor()
{
	OCR5CL = 0;
	OCR5AL = 0;
	OCR4BL = 0;
	OCR5BL = 0;
}
//Standard functions for left turn
void sleft()
{
	OCR5CL = speed_right;
	OCR5BL = 0;
	OCR5AL = 0;
	OCR4BL = speed_left;
}
//Standard functions for right turn
void sright()
{
	OCR5CL = 0;
	OCR5BL = speed_right;
	OCR5AL = speed_left;
	OCR4BL = 0;
}
//Standard functions for forward
void sforward()
{
	if (getDis(1) > 40)
	{
		OCR5CL = speed_right;
		OCR5BL = 0;
		OCR5AL = speed_left;
		OCR4BL = 0;
	}
}
//Standard functions for reverse
void sreverse()
{
	OCR5CL = 0;
	OCR5BL = speed_right;
	OCR5AL = 0;
	OCR4BL = speed_left;
}
//iterative left proportionsal to ii value
void ileft(int ii)
{
	int k;
	OCR5CL = speed_right;
	OCR5BL = 0;
	OCR5AL = 0;
	OCR4BL = 0;

	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}
//iterative right proportional to ii value
void iright(int ii)
{
	int k;
	OCR5CL = 0;
	OCR5BL = 0;
	OCR5AL = speed_left;
	OCR4BL = 0;

	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}
//differential left proportional to ii value
void dleft(int ii)
{
	int k;
	sleft();
	for (k = 0; k < ii; k++)
	{
		_delay_ms(5);
	}
	stopMotor();
}
//differential right proportional to ii value
void dright(int ii)
{
	int k;
	sright();

	for (k = 0; k < ii; k++)
	{
		_delay_ms(5);
	}
	stopMotor();
}
// iterative forward motion
void forward(int ii)
{
	int k;
	sforward();
	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}
//reverse
void reverse(int ii)
{
	int k;
	sreverse();
	for (k = 0; k < ii; k++)
	{
		_delay_ms(10);
	}
	stopMotor();
}

void l_ileft(int ii)
{
	int k;
	OCR5CL = 0;
	OCR5BL = 0;
	OCR5AL = 0;
	OCR4BL = speed_left;

	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}
//iterative right proportional to ii value
void l_iright(int ii)
{
	int k;
	OCR5CL = 0;
	OCR5BL = speed_right;
	OCR5AL = 0;
	OCR4BL = 0;
	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}

void l_forward(int ii)
{
	int k;
	OCR5CL = 0;
	OCR5BL = speed_right;
	OCR5AL = 0;
	OCR4BL = speed_left;
	for (k = 0; k < ii; k++)
	{
		_delay_ms(i_delay);
	}
	stopMotor();
}

// check if two numbers are different with max error of
int err(int a, int b, int er)
{
	return abs(a - b) < er;
}

/*
▪ * Function Name:follow_wall
▪ * Input: none
▪ * Output:none
▪ * Logic:calulate left and right distance and if difference is lesser than 8 cm ,go straight else move accordingly 
▪ * Example Call: follow_wall();
▪ */

void follow_wall()
{

	left = getDis(0);
	right = getDis(2);
	left %= 200;
	right %= 200;

	//calulate left and right distance and if diffrence is lesser than 8 cm ,go straight else move accordingly
	if (err(left, right, 8))
	{
		forward(f_correction);
		return;
	}
	if (left > right)
	{
		ileft(l_correction);
		forward(f_correction);
		return;
	}
	if (left < right)
	{
		iright(r_correction);
		forward(f_correction);
		return;
	}
}

void follow_wall2()
{

	left = getDis(0);
	right = getDis(2);
	left %= 200;
	right %= 200;
	sprintf(distanceString, " left : %d right : %d \n", left, right);
	uart0_puts(distanceString);

	//calulate left and right distance and if diffrence is lesser than 8 cm ,go straight else move accordingly

	if (left < 50)
	{
		iright(l_correction);
		forward(f_correction);
		return;
	}
	if (right < 50)
	{
		ileft(r_correction);
		forward(f_correction);
		return;
	}

	forward(f_correction * 3);
	return;
}

/*
▪ * Function Name:goStraight
▪ * Input: none
▪ * Output:none
▪ * Logic: Distance values from Tof Sensors is read and accordingly descision is taken,LFA sensor is used to 
           correct the orientation of the bot
▪ * Example Call: goStraight();
▪ */

void goStraight()
{
	mid = getDis(1);
	int k = mid;
	mid = mid - 200;

	forward(l_correction * 3);
	int lfaRead = mcp23017_readpinsA(0);

	//move forward untill it moves by 200cm
	while (lfaRead == 0)
	{
		follow_wall();
		lfaRead = mcp23017_readpinsA(0);
	}

	if (lfaRead > 250)
	{

		stopMotor();
		blink(1, 2);
		return;
	}

	if (lfaRead > 8)
	{
		while (lfaRead < 250)
		{
			iright(l_correction);
			lfaRead = mcp23017_readpinsA(0);
		}
		stopMotor();
		blink(1, 2);
		return;
	}
	else
	{
		while (lfaRead < 250)
		{
			ileft(l_correction);
			lfaRead = mcp23017_readpinsA(0);
		}
		stopMotor();
		blink(1, 2);
		return;
	}

	stopMotor();
	blink(1, 2);
}

/*
▪ * Function Name:turnLeft
▪ * Input: none
▪ * Output:none
▪ * Logic: Distance is measured from left and middle ToF sensors then bot is turned left till the value read 
           by middle sensor nears the previously recorded value of left sensor.
▪ * Example Call: turnLeft();
▪ */

void turnLeft()
{
	forward(f_correction * 2);
	mid = getDis(1);
	left = getDis(0);
	dleft(turn_left);
	blink_red(2, 3);
	while (!err(left, getDis(1), 15) && !err(mid, getDis(2), 15))
	{
		dleft(l_correction);
	}
	stopMotor();
	blink(1, 2);
}

/*
▪ * Function Name:turnRight
▪ * Input: none
▪ * Output:none
▪ * Logic: Distance is measured from Right and middle ToF sensors then bot is turned right till the value read 
           by middle sensor nears the previously recorded value of right sensor.
▪ * Example Call: turnRight();
▪ */


void turnRight()
{

	mid = getDis(1);
	right = getDis(2);
	dright(turn_left);
	while (!err(right, getDis(1), 4) && !err(mid, getDis(0), 4))
	{
		dright(r_correction);
	}
	stopMotor();
	blink(1, 2);
}

/**************************************************************END OF*********************************************************************************
 ************************************************************* MOTORS ********************************************************************************
 *****************************************************************************************************************************************************           
 * */

/*********************************************************************************************************************************************************
 *************************************************************** LFA *************************************************************************************
 *********************************************************************************************************************************************************           
 * */

void initLFA()
{
	uart0_puts("Initializing LFA\n");
	mcp23017_init();
	mcp23017_setmodeA(0, MCP23017_MODEINPUTALL);
	mcp23017_setmodeB(0, MCP23017_MODEOUTPUTALL);
	mcp23017_writepinB(0, 0, MCP23017_PINSTATEOFF);
	mcp23017_writepinB(0, 1, MCP23017_PINSTATEOFF);
}
void testLFA()
{
	while (1)
	{
		uint8_t lfaRead = mcp23017_readpinsA(0);
		char x[20];
		sprintf(x, "%d", lfaRead);
		uart0_puts("\nLFAA:");
		char temp[20];
		sprintf(temp, "%d", (int)lfaRead);
		uart0_puts(temp);
	}
}
/*****************************************************************END OF*************************************************************************************
 ****************************************************************** LFA ***************************************************************************
 *********************************************************************************************************************************************************           
 * */

/*********************************************************************************************************************************************************
 *************************************************************** EXTRAS***************************************************************************
 *********************************************************************************************************************************************************           
 * */

//use this function to initialize all devices
void init_devices(void)
{
	timer5_init();
	timer4_init();
	init_Motor();
	led_init();
	initLFA();
	stopMotor();
	init_i2c_uart();
	init_Sensor_Vl53l0x();
	init_button();
	init_switch_interrupt();
}

void testUS(int ii)
{
	while (ii--)
	{
		getDistance(distanceString);
		uart0_puts(distanceString);
		_delay_ms(500);
	}
}

/* ######################################################################################################################################################
#########################################################################################################################################################
#########################################################################################################################################################
*/

int main(void)
{

	//while(PE7);
	cli();
	init_devices();
	uart0_init(UART_BAUD_SELECT(115200, F_CPU));
	uart0_flush();

	start = 1;
	while (1)
	{
		if (start == 0)
		{
			break;
		}
	}

	int path_len;
	int i = 0;
	//vaiables to keep track of current position and direction (x,y) (a,b)
	//next position (nx,ny) (na,nb)
	/*
	bot can have four directions, and represented by (a,b)
	(a,b)=(0,1)   UP
	(a,b)=(1,0)	  Right
	(a,b)=(0,-1)  LEFT
	(a,b)=(-1,0)  DOWN
	*/
	int nx, ny, x, y, a, b, na, nb, fx, fy;

	int path[50][2];
	int digits_list[20];
	int planned_firezones[10][3]; //for storing the planned firezones (x,y,n) n-> no of victims in (x,y)
	int no_planned_firezones;
	int hosp = 0; //no of hospital to be visited
	int complete = 0;

	int no_victims = 0;	//current victims
	int total_victims = 0; //total victims

	//final positions (fx,fy)
	fx = 9;
	fy = 9;

	//recieving digits list
	readLine(recvFrom);

	int n = strlen(recvFrom);
	int o = 1;
	int point = 0;
	while (recvFrom[o] != '[')
	{
		o++;
	}
	while (recvFrom[o] != ']')
	{
		if (!(recvFrom[o] == "," && recvFrom[o] == " "))
		{
			digits_list[point++] = recvFrom[o++] - 48;
		}
	}
	//end of parsing digitlist
	//sending acknowledgement
	uart0_puts("@['ok']@\0");

	//recieving planned firezones
	o = 0;
	point = 0;
	while (recvFrom[o] != '{')
	{
		o++;
	}
	int s_temp = 0;
	while (recvFrom[o] != '}')
	{
		if (!(recvFrom[o] == "," || recvFrom[o] == " " || recvFrom[o] == ")" || recvFrom[o] == "(" || recvFrom[o] == ":"))
		{
			if (s_temp == 0)
			{
				planned_firezones[point][s_temp] = recvFrom[o++] - 48;
				s_temp = 1;
				point++;
			}
			else if (s_temp = 1)
			{
				planned_firezones[point][s_temp] = recvFrom[o++] - 48;
				s_temp = 2;
			}
			else
			{
				planned_firezones[point][s_temp] = recvFrom[o++] - 48;
				s_temp = 0;
				point++;
			}
		}
		o++;
	}
	no_planned_firezones = point;

	//end of parsing planned firezones

	while (1)
	{
		//recive the path from client
		//decode the recved path to array

		readLine(recvFrom);

		int n = strlen(recvFrom);
		o = 0;
		point = 0;

		//parsing the recieved data into path cordinates
		while (recvFrom[o] != '[')
		{
			o++;
		}
		o++;
		s_temp = 0;
		while (recvFrom[o] != ']')
		{
			if (!(recvFrom[o] == "," || recvFrom[o] == " " || recvFrom[o] == ")" || recvFrom[o] == "("))
			{
				if (s_temp)
				{
					path[point][1] = recvFrom[o++] - 48;
					s_temp = 0;
					point++;
				}
				else
				{
					path[point][0] = recvFrom[o++] - 48;
					s_temp = 1;
				}
			}
			o++;
		}

		x = path[0][0];
		y = path[0][1];

		a = 0;
		b = 1;

		path_len = point;
		i = 1;
		while (1)
		{
			//get the positions of next cell from path(nx,ny)
			nx = path[i][0];
			ny = path[i][1];
			//calculate which directions it has to move (na,nb)
			//(a,b) represents the current directions
			//(na,nb) represents the relativae directions to turn to reach next cell
			na = nx - x;
			nb = ny - y;
			if (a == na && b == nb)
			{
				//check if straight
				//go straight (one cell))

				//uart0_puts("\nstraight:");
			}
			else if (nb == a && (-1 * na) == b)
			{
				//check for left
				//rotate left
				//uart0_puts("\nleft:");
				turnLeft();
				//update directions (a,b)
				a = na;
				b = nb;
			}
			else if (na == b && (-1 * nb) == a)
			{
				//check for right
				//turn right
				//uart0_puts("\nRight:");
				turnRight();
				//update directions (a,b)
				a = na;
				b = nb;
			}
			else if (na == -1 * a && nb == (-1 * b))
			{
				//checking for full turn
				turnRight();
				turnRight();
				a = na;
				a = nb;
			}

			//check if (nx,ny) is final position
			if (nx == fx && ny == fy)
			{
				goStraight();
				_delay_ms(1000);
				hosp = 1;
				break;
			}
			//checking for reposition
            char a = uart0_getc();
            if (a == '&')
            {
                //if client request for repositions
                //sending the prev co-orinate
                sprintf(toSend, " @$|reposition|(%d %d)@\0", nx, ny);
                uart0_puts(toSend);
                point--;
                continue;
            }
			//check if there are special task (firezones)
			//performs the respective task

			//check if there is a obstacle
			if (getDis(1) < 150)
			{
				blink_red(8, 10);
				sprintf(toSend, " @$|obstacle|(%d %d)@\0", nx, ny);
				uart0_puts(toSend);
				_delay_ms(1000);
				break;
			}
			//check if (nx,ny) is firezone
			if (i == point - 1) ////if it is a firezone
			{
				//blink_red(5, 6);
				int temp;
				for (temp = 0; temp < no_planned_firezones; temp++)
				{
					if (planned_firezones[0] == nx && planned_firezones[0] == ny)
					{
						no_victims = planned_firezones[2];
						break;
					}
				}
				total_victims += no_victims;
				sprintf(toSend, " @$|%d|(%d,%d)@\0", no_victims, nx, ny);
				uart0_puts(toSend);
				_delay_ms(1000);
				break;
			}
			else
			{
				goStraight();
				//update positions (x,y)
				x = x + a;
				y = y + b;
				i++;
			}
		}
		if (hosp == 1)
		{
			break;
		}
	}

	forward(15);
	hosp = 0;

	///line follower dropping the victims to the hospitals
	while (1)
	{
		uint8_t lfaRead = mcp23017_readpinsA(0);

		if (lfaRead == 24 || lfaRead == 8 || lfaRead == 16 || lfaRead == 48 || lfaRead == 32 || lfaRead == 12 || lfaRead == 4 || lfaRead == 96)
		{
			shift = 0;
		}
		if (lfaRead == 231 || lfaRead == 247 || lfaRead == 239)
		{
			shift = 1;
		}

		if (shift == 1)
		{
			lfaRead = ~lfaRead;
		}

		switch (lfaRead)
		{

		case 24: // B00011000:
			forward(1);
			break;

		case 0: //B00000000: // on white paper
			forward(1);
			break;

		case 128: //B10000000: // leftmost sensor on the line
			ileft(1);
			break;

		case 64: //B01000000:
			ileft(1);
			break;

		case 32: // B00100000:
			ileft(1);
			break;

		case 16: // B00010000:
			ileft(1);
			break;

		case 8: // B00001000:
			iright(1);
			break;

		case 4: //B00000100:
			iright(1);
			break;

		case 2: //B00000010:
			iright(1);
			break;

		case 1: //B00000001:
			iright(1);
			break;

		case 192: //B11000000:
			ileft(1);
			break;

		case 96: //B01100000:
			ileft(1);
			break;

		case 48: //B00110000:
			ileft(1);
			break;

		case 12: // B00001100:
			iright(1);
			break;

		case 6: // B00000110:
			iright(1);
			break;

		case 3: //B00000011:
			iright(1);
			break;

		case 224: //B11100000:
			ileft(1);
			break;

		case 112: //B01110000:
			ileft(1);
			break;

		case 56: //B00111000:
			ileft(1);
			break;

		case 28: //B00011100:
			iright(1);
			break;

		case 14: //B00001110:
			iright(1);
			break;

		case 7: //B00000111:
			iright(1);
			break;

		case 240: //B11110000:
			ileft(1);
			break;

		case 120: //B01111000:
			ileft(1);
			break;

		case 60: //B00111100:
			iright(1);
			break;

		case 30: //B00011110:
			iright(1);
			break;

		case 15: //B00001111:
			iright(1);
			break;

		case 248: //B11111000:
			ileft(60);
			break;

		case 124: //B01111100:
			ileft(1);
			break;

		case 62: //B00111110:
			iright(1);
			break;

		case 31: //B00011111:
		{

			forward(10);
			dright(4);
			led_greenOn();
			_delay_ms(1500);
			if (hosp == 1)
			{
				sprintf(toSend, " @HA reached@\0");
			}
			if (hosp == 2)
			{
				sprintf(toSend, " @HB reached@\0");
			}
			if (hosp == 3)
			{
				sprintf(toSend, " @HC reached, Task accomplished!@\0");
				complete = 1;
			}
			uart0_puts(toSend);
			hosp++;
			led_greenOff();
			_delay_ms(1000);
			dleft(4);
			break;
		}
		case 252: //B11111100:
			ileft(1);
			break;

		case 126: //B01111110:
			forward(1);
			break;

		case 63: //B00111111:
			iright(1);
			break;

		case 254: //B11111110:
			ileft(1);
			break;

		case 127: //B01111111:
			iright(1);
			break;

		case 255: //B11111111:
			ileft(60);
			break;

		default:
			break;
		}
		if (complete)
		{
			break;
		}
	}

	return 0;
}
