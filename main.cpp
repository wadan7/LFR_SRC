/*
  Line Following Robot 
    > Both IRs Will Positioned On A White Line 
      >The Forward State Is when Both IRs Read 0, 0
    > This Code Is based on moore FSM
    > Motors Drivers Are ( 2 * BTS7960 )
*/

// IF White Line define WHITE
// IF Black Line define BLACK
#define WHITE
/////////////////////////////


#include <Arduino.h>

//Define Pins
int PWMA_RIGHT = 9;
int PWMB_RIGHT = 8;

int PWMA_LEFT = 7;
int PWMB_LEFT = 6;

#define IR_R 3
#define IR_L 4

//Spedd of The Robot
#define spd 70
////////////////////


//All Possible States The System Can present
#define Center 0
#define Right 1
#define Left 2
#define Stop 3
/////////////////////////////////////////////

// define a state type that can present one of the previousely defined
// each state shoud have
//   > output
//   > Time (delay)
// each state shoud refer to the next state based on the input data
typedef struct 
{
  int out;
  int time;
  int next[4];
} State_t;

// Define the State transition table
#ifdef WHITE
  const State_t fsm[4] =
  {

        {0x03, 10, {Center,    Right,     Left,      Center}}, // Center 0
        {0x02, 10, {Center,    Center,    Left,      Right}}, // Right 1
        {0x01, 10, {Center,    Right,     Center,    Left}}, // Left 2
        {0x00, 10, {Center,    Right,     Left,      Stop}} // Stop 3

  };

#elif BLACK
  const State_t fsm[4] =
  {

        {0x03, 10, {Stop,      Left,      Right,       Center}}, // Center 0
        {0x02, 10, {Right,     Left,      Center,      Center}}, // Right 1
        {0x01, 10, {Left,      Center,    Right,       Center}}, // Left 2
        {0x00, 10, {Stop,      Left,      Right,       Center}} // Stop 3

  };
#endif
////////////////////////////////////////////////////////////////////////////////


/////////// Function Declarations /////////////
void DriveMotors(int st); // Control the motors
int ReadSensors(); // Read the two IRs
//////////////////////////////////////////////


int state;
int input;
int output;

void setup() {
  // put your setup code here, to run once:
  state = Center;

  pinMode(IR_R, INPUT);
  pinMode(IR_L, INPUT);

  Serial.begin(9600);

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
  return ( (right) + (left<< 1) );
}

void DriveMotors(int out)
{

  int right = out & 0x01; // first bit in output 
  int left = out & 0x02;  // second bit in output

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
