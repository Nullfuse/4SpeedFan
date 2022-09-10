#include <Keypad.h>

// LEDS
const int b_size = 4;
const int b[b_size] = {A2, A3, A4, A5};
// Output Buffer
int b_buf = 0x00;

int s_buf = 0x00;
const int s_size = 4;
int s[s_size];

// Buttons
const int a_size = 2;
const int a[a_size] = {A0, A1};
const int a_type[a_size] = {0, 1}; // 0 for pulldown, 1 for pullup

// Motor
int motorPin = 10;

// Ultrasonic Sensor
int echoPin = 11;
int trigPin = 12;

// Keypad
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1','2','3', 'A'},
    {'4','5','6', 'B'},
    {'7','8','9', 'C'},
    {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// Global Variables
char emergencyShutoff;
int motorSpeed;

// State Machines
enum Control_States{CONTROL_INIT, CONTROL_WAIT, CONTROL_KEYPADHOLD, CONTROL_BUTTONHOLD, CONTROL_RELEASE} Control_state;
enum Ultrasonic_States{ULTRASONIC_INIT, ULTRASONIC_HIGH, ULTRASONIC_LOW, ULTRASONIC_PROCESS} Ultrasonic_state;
enum Output_States{OUTPUT_INIT, SM_OUTPUT} Output_state;

typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
    
} task;

unsigned long lastRan;
int delay_gcd;
const unsigned short tasksNum = 3;
task tasks[tasksNum];




// Reset the Output Buffer. 
void resetBuffer() {
    for(int i = 0; i < b_size; i++) {
        // Note this is an arduino function call to the pins
        digitalWrite(b[i], LOW);
    }
}

// Writes to the buffer. Note this function ORs the current value with the new value
// Note that size is an optional argument with default size the same as the buffer
void writeBuffer(unsigned char b_temp, int size = b_size)
{
   for (int i = 0; i < size; i++) {
        if (b_temp > 0){
            b_temp--;
            digitalWrite(b[i], HIGH);
        }
        else {
            break;
        }
    }
}

void outputProcedure(){
    resetBuffer();
    // Note that here we arbitrarily connect b_buf & s_buf together. The important point is that we preserve the current value of the output buffer into a variable b_buf before writing it to the pins. If we resetBuffer or readData() again, we still know the values the output is set to, because it is in b_buf. 
    b_buf = s_buf;
    writeBuffer(b_buf);
}

int buttonPressed(const int buttonPin){
    for(int i = 0; i < a_size; ++i){
        if(a[i] == buttonPin){
            if(a_type[i] == 0){
                if(digitalRead(a[i]) == HIGH){
                  return HIGH;
                }else{
                  return LOW;
                }
            }else{
                if(digitalRead(a[i]) == LOW){
                  return HIGH;
                }else{
                  return LOW;
                }
            }
        }
    }
}




int Control_Tick(int state){
    static char key;
    switch(state) { // State transitions
        case CONTROL_INIT:
            key = 0;
            motorSpeed = 0;
            state = CONTROL_WAIT;
        break;
        case CONTROL_WAIT:
            state = CONTROL_WAIT;
            key = keypad.getKey();
            if((key == '0') || (key == '1') || (key == '2') || (key == '3') || (key == '4')){
                motorSpeed = key - 48;
                state = CONTROL_KEYPADHOLD;
                break;
            }
            for(int i = 0; i < a_size; ++i){
                if(buttonPressed(a[i])){
                    if(a_type[i] == 0){
                        if(motorSpeed != 0){
                            --motorSpeed;
                        }
                    }else{
                        if(motorSpeed != 4){
                            ++motorSpeed;
                        }
                    }
                    state = CONTROL_BUTTONHOLD;
                    break;
                }
            }
        break;
      case CONTROL_KEYPADHOLD:
          state = CONTROL_KEYPADHOLD;
          key = keypad.getKey();
          if(!key){
              state = CONTROL_RELEASE;
          }
      break;
      case CONTROL_BUTTONHOLD:
          state = CONTROL_RELEASE;
          for(int i = 0; i < a_size; ++i){
              if(buttonPressed(a[i])){
                  state = CONTROL_BUTTONHOLD;
              }
          }
      break;
      case CONTROL_RELEASE:
          state = CONTROL_WAIT;
      break;
    }
                
    switch(state) { // State Action
        case CONTROL_INIT:
        break;
        case CONTROL_WAIT:
        break;
        case CONTROL_KEYPADHOLD:
        break;
        case CONTROL_BUTTONHOLD:
        break;
        case CONTROL_RELEASE:
        break;
    }
                
    return state;
}

int Ultrasonic_Tick(int state){
    static long duration;
    static long distance;
    switch(state) { // State transitions
        case ULTRASONIC_INIT:
            duration = 0;
            digitalWrite(trigPin, LOW);
            state = ULTRASONIC_HIGH;
        break;
        case ULTRASONIC_HIGH:
            state = ULTRASONIC_LOW;
        break;
        case ULTRASONIC_LOW:
            state = ULTRASONIC_PROCESS;
        break;
        case ULTRASONIC_PROCESS:
            state = ULTRASONIC_HIGH;
        break;
    }
               
    switch(state) { // State Action
        case ULTRASONIC_INIT:
        break;
        case ULTRASONIC_HIGH:
            digitalWrite(trigPin, HIGH);
        break;
        case ULTRASONIC_LOW:
            digitalWrite(trigPin, LOW);
            duration = pulseIn(echoPin, HIGH);
        break;
        case ULTRASONIC_PROCESS:
            distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.println(" cm");
            if(distance <= 10){ // Distance less than or equal to 10 cm
                emergencyShutoff = 1;
                analogWrite(motorPin, 0);
            }else{
                emergencyShutoff = 0;
            }
        break;
    }
               
    return state;
}

int Output_Tick(int state){
    static int count;
    static int pwmOutput;
    switch(state) { // State transitions
        case OUTPUT_INIT:
            count = 0;
            pwmOutput = 0;
            state = SM_OUTPUT;
        break;
        case SM_OUTPUT:
            state = SM_OUTPUT;
        break;
    }
               
    switch(state) { // State Action
        case OUTPUT_INIT:
        break;
        case SM_OUTPUT:
            if(!emergencyShutoff){
                s_buf = motorSpeed;
                outputProcedure();
                pwmOutput = map(motorSpeed,0,4,0,255);
                analogWrite(motorPin, pwmOutput);
            }else{
                analogWrite(motorPin, 0);
                if((count % 2) == 0){
                    s_buf = 0;
                    outputProcedure();
                    ++count;
                }else{
                    s_buf = motorSpeed;
                    outputProcedure();
                    count = 0;
                }
            }
        break;
    }
                
    return state;
}




void setup() {
    // OUTPUT is a defined macro in Arduino!
    for(int i = 0; i < b_size; i++)
    {
        pinMode(b[i], OUTPUT);
    }

  	//INPUTS
    for(int i = 0; i < a_size; i++)
    {
        pinMode(a[i], a_type[i]?INPUT_PULLUP:INPUT);
    }
  
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  
    unsigned char i = 0;
    tasks[i].state = CONTROL_INIT;
    tasks[i].period = 500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Control_Tick;
  	++i;
    tasks[i].state = ULTRASONIC_INIT;
    tasks[i].period = 250;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Ultrasonic_Tick;
  	++i;
    tasks[i].state = OUTPUT_INIT;
    tasks[i].period = 250;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Output_Tick;
  
    delay_gcd = 250; // GCD
  
    Serial.begin(9600);
}

void loop() {
    if ((millis() - lastRan) > delay_gcd) {
        unsigned char i;
        for (i = 0; i < tasksNum; ++i) {
            if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
                tasks[i].state = tasks[i].TickFct(tasks[i].state);
                tasks[i].elapsedTime = millis(); // Last time this task was ran
            }
        } 
        lastRan = millis();
    } 
}