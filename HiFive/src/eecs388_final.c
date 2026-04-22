#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"
#define SERVO_PULSE_MAX (2400) /* 2400 us */
#define SERVO_PULSE_MIN (544) /* 544 us */
#define SERVO_PERIOD (20000) /* 20000 us (20ms) */
#define MIN_ANGLE (0) /* degrees */
#define MAX_ANGLE (180) /* degrees */
void auto_brake(int devid)
{
// Task1 & 2:
// Your code here (Use Lab 02 - Lab 04 for reference)
// Use the directions given in the project document
if('Y' == ser_read(devid) && 'Y' == ser_read(devid)){
uint8_t dist_l = ser_read(devid);
uint8_t dist_h = ser_read(devid);
uint16_t dist = 0;
dist = dist_l | (dist_h << 8);
if(dist > 200){
gpio_write(RED_LED,OFF);
gpio_write(GREEN_LED, ON);
}
else if(dist > 100 && dist <= 200){
gpio_write(RED_LED,ON);
gpio_write(GREEN_LED, ON);
}
else if(dist > 60 && dist <= 100){
gpio_write(GREEN_LED, OFF);
gpio_write(RED_LED, ON);
}
else if(dist <= 60){
gpio_write(GREEN_LED,OFF);
gpio_write(RED_LED, ON);
delay(100);
gpio_mode(RED_LED, OFF);
}
}
}

int read_from_pi(int devid)
{
// Task-3:
// You code goes here (Use Lab 09-option1 for reference)
// After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.

//return the byte read as an int
char info[50]; //creates a buffer to save the info from the pi
if(ser_isready(devid)){ //checks if the device is ready/recieving input
ser_readline(devid,50,info); //reads the info from the device id and saves it to info
int angle = 0; //initializes the angle var
if(sscanf(info, "%d",&angle)==1){
return angle;
} //saves info as an int val into the address of angle
//returns angle
}
return -1; //if no val is found return 0


}

void steering(int gpio, int pos)
{
// Task-4:
// Your code goes here (Use Lab 05 for reference)
// Check the project document to understand the task

if(pos < MIN_ANGLE){
pos = MIN_ANGLE;
}
if(pos > MAX_ANGLE){
pos = MAX_ANGLE;
}
uint32_t scaled = 544 +((1856*pos)/180);
gpio_write(gpio,ON);
delay_usec(scaled);
gpio_write(gpio,OFF);
delay_usec(SERVO_PERIOD-scaled);
}


int main()
{
// initialize UART channels
ser_setup(0); // uart0
ser_setup(1); // uart1
int pi_to_hifive = 1; //The connection with Pi uses uart 1
int lidar_to_hifive = 0; //the lidar uses uart 0
printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
//Initializing PINs
gpio_mode(PIN_19, OUTPUT);
gpio_mode(RED_LED, OUTPUT);
gpio_mode(BLUE_LED, OUTPUT);
gpio_mode(GREEN_LED, OUTPUT);

printf("Setup completed.\n");
printf("Begin the main loop.\n");

while (1) {

auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
printf("\nangle=%d", angle);
int gpio = PIN_19;
for (int i = 0; i < 10; i++){
// Here, we set the angle to 180 if the prediction from the DNN is a positive angle
// and 0 if the prediction is a negative angle.
// This is so that it is easier to see the movement of the servo.
// You are welcome to pass the angle values directly to the steering function.
// If the servo function is written correctly, it should still work,
// only the movements of the servo will be more subtle
if(angle>0){
steering(gpio, 180);
}
else {
steering(gpio,0);
}
// Uncomment the line below to see the actual angles on the servo.
// Remember to comment out the if-else statement above!
// steering(gpio, angle);
}

}
return 0;
}

