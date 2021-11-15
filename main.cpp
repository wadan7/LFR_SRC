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

// Case WHITE
//               Possible Inputs
// 00  =>  On Line              State: Center             Out: 3 (11)2
// 01  =>  Off To The Right     State: Right              Out: 1 (01)2
// 10  =>  Off To The Left      State: Left               Out: 2 (10)2
// 11  =>  Off Line             State: Stop               Out: 0 (00)2


// Possible outputs
/*
    left motor stop      <----     ----> right motor stop
                              |    |
                              0    0
                              

    left motor stop      <----     ----> right motor move
                              |    |
                              0    1
                              
    left motor move      <----     ----> right motor stop
                              |    |
                              1    0
                              
    left motor move      <----     ----> right motor move
                              |    |
                              1    1                                                
*/

#ifdef WHITE
  const State_t fsm[4] =
  {

        {0x03, 10, {Center,    Right,     Left,      Center}}, // Center 0  0x03 => go straight
        {0x01, 10, {Center,    Center,    Left,      Right}}, // Right 1    0x01 => turn left
        {0x02, 10, {Center,    Right,     Center,    Left}}, // Left 2      0x02 => turn right
        {0x00, 10, {Center,    Right,     Left,      Stop}} // Stop 3       0x00 => stop

  };

// Case BLACK
//               Possible Inputs
// 11  =>  On Line              State: Center             Out: 3 (11)2
// 10  =>  Off To The Right     State: Right              Out: 1 (01)2
// 01  =>  Off To The Left      State: Left               Out: 2 (10)2
// 00  =>  Off Line             State: Stop               Out: 0 (00)2

#elif BLACK
  const State_t fsm[4] =
  {

        {0x03, 10, {Stop,      Left,      Right,       Center}},   // Center 0  0x03 => go straight
        {0x01, 10, {Right,     Left,      Center,      Center}},  // Right 1    0x01 => turn left
        {0x02, 10, {Left,      Center,    Right,       Center}}, // Left 2      0x02 => turn right
        {0x00, 10, {Stop,      Left,      Right,       Center}} // Stop 3       0x00 => stop

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

int possible_input[] = {
    0, 1, 2, 3
};

void loop() {

  for (int i=0; i<=3; i++)
  {
    // put your main code here, to run repeatedly:
    output = fsm[state].out;
    //Serial.print("state: "); Serial.print(state);
    //Serial.print("  out: "); Serial.print(output);
    DriveMotors(output);
    delay(fsm[state].time);
    input = ReadSensors();
    state = fsm[state].next[input];
    //Serial.print("  in: "); Serial.print(possible_input[i]);
    //Serial.print("  new state: "); Serial.println(state);
  }

}

int ReadSensors()
{
  int right = digitalRead(IR_R);
  int left = digitalRead(IR_L);
  // return left-right adjacent bits
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

    //Serial.print(" Straigt");
  }
  else if (right)
  {
    // right on forward
    analogWrite(PWMA_RIGHT, spd);
    analogWrite(PWMB_RIGHT, 0);
    // left off 
    analogWrite(PWMA_LEFT, 0);
    analogWrite(PWMB_LEFT, 0);
    //Serial.print(" go left");

  }
  else if (left)
  {
    // left on forward
    analogWrite(PWMA_LEFT, spd);
    analogWrite(PWMB_LEFT, 0);
    // right off
    analogWrite(PWMA_RIGHT, 0);
    analogWrite(PWMB_RIGHT, 0);
    //Serial.print(" go right");

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
    //Serial.print(" stop");

  }
}
