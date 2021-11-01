/*
  Line Following Robot 
    > Both IRs Will Positioned On A White Line 
      >The Forward State Is when Both IRs Read 0, 0
    > This Code Is based on moore FSM
    > Motors Drivers Are ( 2 * BTS7960 )
*/


#include <Arduino.h>

//Define Pins
int PWMA_RIGHT = 9;
int PWMB_RIGHT = 8;

int PWMA_LEFT = 7;
int PWMB_LEFT = 6;

#define IR_R A1
#define IR_L 10

int Right, Left;

#define Center 0
#define Right 1
#define Left 2

#define spd 70

typedef struct 
{
  int out;
  int time;
  int next[4];
} State_t;

const State_t fsm[3] =
{
  {0x03, 10, {Center, Left, Right, Center}},
  {0x02, 10, {Center, Left, Center, Right}},
  {0x01, 10, {Center, Center, Right, Left}}
};


void DriveMotors(int st);
int ReadSensors();

int state;
int input;
int output;
void setup() {
  // put your setup code here, to run once:
  state = Center;

}

void loop() {
  // put your main code here, to run repeatedly:
  output = fsm[state].out;
  DriveMotors(output);
  delay(fsm[state].time);
  input = ReadSensors();
  state = fsm[state].next[input];

}

int ReadSensors()
{
  int right = digitalRead(IR_R);
  int left = digitalRead(IR_L);
  // return rightleft adjacent bits
  return ( (right << 1) | (left) );
}

void DriveMotors(int out)
{
  out = ~out;
  int right = out & 0x02; // second bit in output 
  int left = out & 0x01;  // first bit in output

  if ( right && left)
  {
    // right on forward
    analogWrite(PWMA_RIGHT, spd);
    analogWrite(PWMB_RIGHT, 0);
    // left on forward
    analogWrite(PWMA_LEFT, spd);
    analogWrite(PWMB_LEFT, 0);
  }
  else if (right)
  {
    // right on forward
    analogWrite(PWMA_RIGHT, spd);
    analogWrite(PWMB_RIGHT, 0);
    // left off 
    analogWrite(PWMA_LEFT, 0);
    analogWrite(PWMB_LEFT, 0);
  }
  else if (left)
  {
    // left on forward
    analogWrite(PWMA_LEFT, spd);
    analogWrite(PWMB_LEFT, 0);
    // right off
    analogWrite(PWMA_RIGHT, 0);
    analogWrite(PWMB_RIGHT, 0);
  }
  else 
  {
    // Stop
    // left off
    analogWrite(PWMA_LEFT, 0);
    analogWrite(PWMB_LEFT, 0);
    // right off
    analogWrite(PWMA_RIGHT, 0);
    analogWrite(PWMB_RIGHT, 0);
  }
}
