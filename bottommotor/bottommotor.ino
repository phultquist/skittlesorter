//Patrick Hultquist, Ajay Jayaraman, Max Knutson

#include <Wire.h> //communication library
int servoPin = 9;
int ms = 180;
int n = 3;
int onTime = 85;
int minimum;
int measured = 0;
boolean isNew = false;

#define S0 3
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
int frequency = 0;

int color;
int lastcolor;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  Serial.begin(9600);

  Wire.begin(9);
  analogWrite(9,100);
  Wire.onReceive(receiveEvent);
//  receiveEvent(4);
}

void receiveEvent(int bytes){
  Serial.println("****************receiveEvent Called****************");
  digitalWrite(10, LOW);
  isNew = true;
//  digitalWrite(13, HIGH);
  color = Wire.read();
  prettyPrint("color: ", color);
}

void loop(){
//  digitalWrite(10, HIGH);
//  delay(1000);
//  digitalWrite(10, LOW);
//  delay(1000);
  goToWanted(color);
  isNew = false;
}
void goToWanted(int wanted) {
  digitalWrite(10, LOW);
//  int wanted = 5;
//  Serial.print("measured: ");
//  Serial.println(measure());
  if (wanted == 6) {
    digitalWrite(10, HIGH);
    return;
  }
  int distance = wanted - lastcolor;
  Serial.print("*******distance");
  Serial.println(distance);
  Serial.print("*******lastcolor");
  Serial.println(lastcolor);
  Serial.print("*******wanted");
  Serial.println(wanted);
  if (distance > 1){
    Serial.println("BIGSTEP");
    for (int p = 0; p<distance; p++){
      bigStep();
    }
  }
  if (distance < 0){
    for (int y = 0; y < 5+distance; y++){
      bigStep();
      delay(50);
    }
  }
  while (wanted != measured){
    measured = measure();
    prettyPrint("cup color: ", measured);
    if (measured != wanted){
      next();
    }
  }
  digitalWrite(10, HIGH);
  delay(1000);
  digitalWrite(10, LOW);
  lastcolor = wanted;
}

int measure(){
  int redFrequencies[n] = {0,0,0};
  int greenFrequencies[n] = {0,0,0};
  int blueFrequencies[n] = {0,0,0};
  int x = 0;
  while (x<n){
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    
    // Reading the output frequency
    frequency = pulseIn(sensorOut, LOW);
    redFrequencies[x] = frequency;
    
    delay(100);
    
    // Setting Green filtered photodiodes to be read
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    
    // Reading the output frequency
    frequency = pulseIn(sensorOut, LOW);
    greenFrequencies[x] = frequency;
    
    delay(100);
    
    // Setting Blue filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);
    
    // Reading the output frequency
    frequency = pulseIn(sensorOut, LOW);
    blueFrequencies[x] = frequency;
    x++;
  }
  int c = findColor(average(redFrequencies), average(greenFrequencies), average(blueFrequencies));
  return c; //1:red, 2:orange, 3:yellow, etc
}

int findColor(float rVal, float gVal, float bVal){
  float redD = dist(rVal, gVal, bVal, 65,101,75);
  float yellowD = dist(rVal, gVal, bVal, 36,42,59);
  float greenD = dist(rVal, gVal, bVal, 112,83,81);
  float orangeD = dist(rVal, gVal, bVal, 58,87,78);
  float purpleD = dist(rVal, gVal, bVal, 61,113,70);
  float blankD = dist(rVal, gVal, bVal, 154,155,113);
  Serial.println(rVal);
  Serial.println(gVal);
  Serial.println(bVal);
  int c1 = smallest(redD, orangeD, yellowD, greenD, purpleD, blankD);
//  prettyPrint("findColor color: ", c1);
  return c1;
}

float average(int a[]) 
{ 
    //gets the average of the array. Note it uses the hard-coded number of measurements so it doesn't work for any array
    // Find sum of array element 
    int sum = 0; 
    for (int i=0; i<n; i++) 
       sum += a[i]; 
  
    return sum/n; 
} 
int smallest(float r, float o, float y, float g, float v, float b){
  //finds the correct color and does all the work to interpret it, including returning the color and setting the motor
  float values[6] = {r, o, y, g, v, b};
  
  int mini = getMin(values, 6)+1; // wrote this simple function so I wasn't annoyed with if-else statements
  if (minimum > 15){
    return -1*(mini);
  }
//  prettyPrint("smallest color: ", mini);
  return mini; 
}

int getMin(float* array, int size)
{
  //gets the smallest val of array
  int minIndex = 0;
  minimum = array[0];
  for (int i = 0; i < size; i++)
  {
    if (array[i] < minimum) {minimum = array[i]; minIndex = i;}
  }
  prettyPrint("Recognized Color Distance: ", minimum);
  return minIndex;
}

void prettyPrint(String line1, float line2){
  //testing function, not really in use anymore. Keep it here for good times.
  Serial.println(line1);
  Serial.print("   ");
  Serial.println(line2);
}

float dist(int a, int b, int c, int a1, int b1, int c1){
  //finds three-dimensional distance between three points using pythagorean theorem
  return (pow(pow((a-a1),2) + pow((b-b1),2) + pow((c-c1),2), 0.5));
}

void next(){
  digitalWrite(4, HIGH);
  delay(onTime);
  digitalWrite(4, LOW);
}

void bigStep(){
  digitalWrite(4, HIGH);
  delay(190);
  digitalWrite(4, LOW);
}
