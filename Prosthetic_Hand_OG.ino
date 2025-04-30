//#include <Dynamixel2Arduino.h>
#include <DynamixelShield.h>
#include <Smoothed.h>
using namespace ControlTableItem;

// Define pins
const int pot0Pin = A0;
const int pot1Pin = A1;
const int pot2Pin = A2;
const int pot3Pin = A3;
const int pot4Pin = A4;

int potList[] = {pot0Pin, pot1Pin, pot2Pin, pot3Pin, pot4Pin};
String motorList[] = {"Motor 0: ", "Motor 1: ", "Motor 2: ", "Motor 3: ", "Motor 4: "};
int pos = 0;
int total = 0;
int numReadings = 10;
int smoothed = 0;
int graspForce = 100; //in milli-amps

int minGraspPosition[5] = {200, 200, 200, 800, 200};
int maxGraspPosition[5] = {4500, 3300, 3300, 4500, 5500};

Smoothed <int> myPots;

float presentLoad = 0.0;
float graspSpeed = 50;
float grasp_force_threshold = 500;
float grasp_force = 30;
float grasp_pos_max = 5500.0;
bool pos_ctrl = true;

float posError = 0.0;

// ************************ Motor Settings and Variables ******************************* //
#define DXL_SERIAL Serial1
#define DEBUG_SERIAL Serial
const int DXL_DIR_PIN = -1;

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
// ************************************************************************************** //

void enable_pos_ctrl();
void enable_voltage_ctrl();
void grasp();
float filter();

void setup() {
//Serial.begin(9600);
  DEBUG_SERIAL.begin(9600);
  Serial.println("test");

  initialize();

}

void loop() {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < numReadings; j++) {
      total = total + analogRead(potList[i]);
      delay(1);
    }
    smoothed = total/numReadings;
    pos = map(smoothed, 0, 1023, 200, 5500);

    if (pos < minGraspPosition[i]) {
      pos = minGraspPosition[i];
    }
    else if (pos > maxGraspPosition[i]) {
     pos = maxGraspPosition[i];
    }
    dxl.setGoalPosition(i, pos);
    Serial.print(motorList[i]);
    Serial.print(String(pos) + "\t");
    smoothed = 0;
    total = 0;
    delay(1);
  }
  Serial.println(); //start new line


}
// ************************************************************************************** //
// ********************************** Functions ***************************************** //
// ************************************************************************************** //
void initialize() {
// Initialize pushbuttons
  pinMode(pot0Pin, INPUT);
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  pinMode(pot3Pin, INPUT);
  pinMode(pot4Pin, INPUT);

  myPots.begin(SMOOTHED_AVERAGE, 10);

  //Initialize motors 0 through 4
  dxl.begin(57600);
  dxl.setPortProtocolVersion(2.0);
  for (int i = 0; i < 5; i++) {
    dxl.ping(i);
    dxl.torqueOff(i);
    dxl.setOperatingMode(i, OP_CURRENT_BASED_POSITION);
    dxl.torqueOn(i);
    dxl.setGoalCurrent(i, graspForce, UNIT_MILLI_AMPERE);
    //dxl.setGoalPosition(i, minGraspPosition[i]);
    delay(10);
  }
}

void enable_pos_ctrl() {
  if (!pos_ctrl)
  {
    dxl.torqueOff(1);
    dxl.setOperatingMode(1, OP_CURRENT_BASED_POSITION);
    dxl.torqueOn(1);
    pos_ctrl = true;
  }
}

void enable_voltage_ctrl() {
  if (pos_ctrl)
  {
    dxl.torqueOff(1);
    dxl.setOperatingMode(1, OP_PWM);
    dxl.torqueOn(1);
    pos_ctrl = false;
  }
}

void grasp() {
  if (dxl.getPresentPosition(1) >= grasp_pos_max) { // If position is over max, set it back to the max
    enable_pos_ctrl();
    dxl.setGoalPosition(1, grasp_pos_max);
//newData = false;
    return;
  }
  for (int i = 0; i < 5; i++) {
    while (dxl.getPresentPosition(i) <= grasp_pos_max) {
      dxl.setGoalCurrent(i, 80, UNIT_MILLI_AMPERE);
      dxl.setGoalPosition(i, maxGraspPosition[i]);
      //dxl.setGoalPosition(1, dxl.getPresentPosition(1) + grasp_speed);
      //DEBUG_SERIAL.print("Current position: ");
      //DEBUG_SERIAL.println(dxl.getPresentPosition(1));
      //DEBUG_SERIAL.print("Current current: ");
      DEBUG_SERIAL.println(dxl.getPresentCurrent(i));
//delay(10);
    }
  }

}