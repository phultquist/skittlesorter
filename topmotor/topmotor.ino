#include <Servo.h>
#include <Wire.h>
int number = 15;
/*
  Max Knutson, Ajay Jayaraman, Patrick Hultquist
  Skittle Sorter and interpreter
  Fall 2019

  SOME OF THE COLOR SENSING CODE: https://howtomechatronics.com/tutorials/arduino/arduino-color-sensing-tutorial-tcs230-tcs3200-color-sensor/
  SOME OF THE SERVO MOTOR CODE: https://www.arduino.cc/en/reference/servo
  REST OF CODE WRITTEN BY GROUP
*/

boolean STOPPED = false;

int servoPin = 13; //pin for the top servo motor
 
Servo servo; //bottom servo motor object

int rgbPins[3] = {9,10,11}; //pins to measure the respective r, g, and b, of skittle
 
int servoAngle = 0;   // servo position in degrees

//the following defines important pins for the color sensor
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define sensorOut 7

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
 
void setup()
{
    // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,LOW);
  digitalWrite(S1,HIGH);
  
   // Begins serial communication. I don't require it to be connected to the computer so I don't use while(!Serial);
  Serial.begin(9600);  
  servo.attach(servoPin);

  Wire.begin(); //begins communication with other arduinos
}
 
 
void loop()
{
  if (STOPPED){
    return;
  }
  servo.write(68); //first angle to pick up skittle
  delay(500);
  for(servoAngle = 68; servoAngle < 108; servoAngle++)  //moves the skittle under the sensor
  {                                  
    servo.write(servoAngle);              
//    delay(10);                  
  }
  measure(); //function, defined below, which measures the color of a skittle and works from there
  int areadpin = A0;
  int val = 0;
//  delay(5000);
  while (val < 800){
    val = analogRead(areadpin);
  }
//  delay(10000);
  for(servoAngle = 108; servoAngle < 160; servoAngle++)  //moves the skittle to the release position
  {                                
    servo.write(servoAngle);          
//    delay(10);      
  }

  delay(200);  

}

void measure(){
  //this function takes 15 measurements and averages them (original code)
  int redFrequencies[number] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int blueFrequencies[number] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int greenFrequencies[number] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int x = 0;
  delay(100);
  while (x<number){

    //this code is from the source listed at the top; well, then heavily modified
      // Setting RED (R) filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    
    // Reading the output frequency
    redFrequency = pulseIn(sensorOut, LOW);
    redFrequencies[x] = redFrequency;
    
     // Printing the RED (R) value
    delay(100);
    
    // Setting GREEN (G) filtered photodiodes to be read
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    
    // Reading the output frequency
    greenFrequency = pulseIn(sensorOut, LOW);
    
    // Printing the GREEN (G) value  
    greenFrequencies[x] = greenFrequency;
    delay(100);
   
    // Setting BLUE (B) filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);
    
    // Reading the output frequency
    blueFrequency = pulseIn(sensorOut, LOW);
    blueFrequencies[x] = blueFrequency;
    
    // Printing the BLUE (B) value 
    delay(100);
    x++;
  }
  //basically the function that works to do the rest
  findColor(average(redFrequencies), average(greenFrequencies), average(blueFrequencies));
  Serial.println();
}

void findColor(float rValue, float gValue, float bValue){
  //finds the color using other function smallest();

  /*
   * I'll break this down: originally, I had a range for each color.
   * Now I have approximate frequency values for each color and I find which color is closest
   * so the dist() function is a 3-dimensional distance and the smallest() finds the smallest of the distances
   * It's a 'best guess' algorithm that is a lot better than anything else
   */
  float redD = dist(rValue, gValue, bValue, 1250, 1550, 410);
  float yellowD = dist(rValue, gValue, bValue, 1050, 1125, 340);
  float greenD = dist(rValue, gValue, bValue, 1260, 1240, 385);
  float orangeD = dist(rValue, gValue, bValue, 1130, 1500, 365);
  float purpleD = dist(rValue, gValue, bValue, 1470, 1605, 445);
  float blankD = dist(rValue, gValue, bValue, 1580, 1600, 440);
  
  Serial.println();
  String color = smallest(redD, orangeD, yellowD, greenD, purpleD, blankD);
  Serial.println(color);

  //prints r, g, b for testing
  Serial.println(rValue);
  Serial.println(gValue);
  Serial.println(bValue);
}

float average(int a[]) 
{ 
    //gets the average of the array. Note it uses the hard-coded number of measurements so it doesn't work for any array
    int n = number;
    // Find sum of array element 
    int sum = 0; 
    for (int i=0; i<n; i++) 
       sum += a[i]; 
  
    return sum/n; 
} 

float dist(int a, int b, int c, int a1, int b1, int c1){
  //finds three-dimensional distance between three points using pythagorean theorem
  return (pow(pow((a-a1),2) + pow((b-b1),2) + pow((c-c1),2), 0.5));
}

void prettyPrint(String line1, float line2){
  //testing function, not really in use anymore. Keep it here for good times.
  Serial.println(line1);
  Serial.print("   ");
  Serial.println(line2);
}

String smallest(float r, float o, float y, float g, float v, float b){
  //finds the correct color and does all the work to interpret it, including returning the color and setting the motor
  float values[6] = {r, o, y, g, v, b};
  
  int mini = getMin(values, 6); // wrote this simple function so I wasn't annoyed with if-else statements
  writeWire(mini+1);
  
  switch (mini) {
    case 0:
      return "red";
      break;
    case 1:
      return "orange";
      break;
    case 2:
      return "yellow";
      break;
    case 3:
      return "green";
      break;
    case 4:
      return "purple";
      break;
    case 5:
      return "blank";
      break;
    default:
      return "this is an impossible scenario";
   }
  
  return "uh-oh spaghetti-os"; //"have some fun once in a while"
}

int getMin(float* array, int size)
{
  //gets the smallest val of array
  int minIndex = 0;
  int minimum = array[0];
  for (int i = 0; i < size; i++)
  {
    if (array[i] < minimum) {minimum = array[i]; minIndex = i;}
  }
  return minIndex;
}

void writeWire(int n){
  //function that writes to another arduino via I2C communication
  Wire.beginTransmission(9);//9 here is the address of the slave board 
  Wire.write(n);//Transfers the value of potentiometer to the slave board            
  Wire.endTransmission();
}
