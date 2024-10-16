#include <WiFi.h>
#include <WebServer.h>
#include <QTRSensors.h>

#define RH 19  // Right motor control
#define LH 13  // Left motor control
#define RL   2 // Reverse Right motor control
#define LL 0 // Reverse Left motor control

// WiFi credentials
const char* ssid = "Net";
const char* password = "take1234";

// Web server on port 80
WebServer server(80);

// Pololu sensor setup
QTRSensors qtrrc;
int basespeed = 75; // Base motor speed
int maxSpeed = 100; // Maximum motor speed
uint16_t sensorValues[12]; // Array to store sensor values

// PID constants
float Kp = 0.005;  // Proportional gain
float Ki = 0.0;  // Integral gain
float Kd = 0.05;  // Derivative gain
int integral = 0;
int lastError = 0;

// Function to serve the PID control webpage
void handleRoot() {
  String html = "<html><body><h1>PID Tuning</h1>"
                "<form action='/update'>"
                "Kp: <input type='text' name='kp' value='" + String(Kp) + "'><br>"
                "Ki: <input type='text' name='ki' value='" + String(Ki) + "'><br>"
                "Kd: <input type='text' name='kd' value='" + String(Kd) + "'><br>"
                "Base Speed: <input type='text' name='basespeed' value='" + String(basespeed) + "'><br>"
                "Max Speed: <input type='text' name='maxspeed' value='" + String(maxSpeed) + "'><br>"
                "<input type='submit' value='Update'>"
                "</form></body></html>";
  server.send(200, "text/html", html);
}

// Function to update PID values
void handleUpdate() {
  if (server.hasArg("kp")) Kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) Ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) Kd = server.arg("kd").toFloat();
  if (server.hasArg("basespeed")) basespeed = server.arg("basespeed").toInt();
  if (server.hasArg("maxspeed")) maxSpeed = server.arg("maxspeed").toInt();
  
  String message = "Updated: <br>Kp: " + String(Kp) + "<br>Ki: " + String(Ki) + "<br>Kd: " + String(Kd) + 
                   "<br>Base Speed: " + String(basespeed) + "<br>Max Speed: " + String(maxSpeed);
  server.send(200, "text/html", message + "<br><a href='/'>Go Back</a>");
}

void setup() {
  // Set motor pins as outputs
  pinMode(RH , OUTPUT);
  pinMode(RL , OUTPUT);
  pinMode(LL , OUTPUT);
  pinMode(LH , OUTPUT);

  // Initialize sensor array
  qtrrc.setTypeRC();
  qtrrc.setSensorPins((const uint8_t[]){  15, 12, 4, 14, 16, 27, 17, 26, 5, 25, 18, 33 }, 12);
  Serial.begin(9600);

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start web server
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.begin();
  Serial.println("Web server started");

  // Calibrate sensors
  for (int i = 0; i < 250; i++) {
    qtrrc.calibrate();
    delay(20);
    Serial.println("Calibrating...");
  }
}

void loop() {
  server.handleClient();  // Handle incoming client requests

  // Read sensor values
  int position = qtrrc.readLineWhite(sensorValues);  // Get position based on weighted sum of sensor readings

  // Compute error (desired position is 5500 for 12-sensor array)
  int error = position - 5500;
  integral += error;  // Calculate integral component
  int derivative = error - lastError;  // Calculate derivative component
  int turn = Kp * error + Ki * integral + Kd * derivative;  // Calculate PID value
  lastError = error;  // Store current error as lastError for next iteration

  // Adjust motor speeds based on PID value
  int leftMotorSpeed = basespeed + turn;
  int rightMotorSpeed = basespeed - turn;

  // Ensure motor speeds are within bounds
  if (leftMotorSpeed > maxSpeed) leftMotorSpeed = maxSpeed;
  if (rightMotorSpeed > maxSpeed) rightMotorSpeed = maxSpeed;
  if (leftMotorSpeed < -maxSpeed) leftMotorSpeed = -maxSpeed;
  if (rightMotorSpeed < -maxSpeed) rightMotorSpeed = -maxSpeed;

  // Motor control logic
  if (leftMotorSpeed < 0) {
    analogWrite(LL, -leftMotorSpeed);  // Reverse left motor
    analogWrite(LH, 0);
  } else {
    analogWrite(LH, leftMotorSpeed);
    analogWrite(LL, 0);
  }

  if (rightMotorSpeed < 0) {
    analogWrite(RL, -rightMotorSpeed);  // Reverse right motor
    analogWrite(RH, 0);
  } else {
    analogWrite(RH, rightMotorSpeed);
    analogWrite(RL, 0);
  }

  // Output debugging information
  Serial.print("Error: ");
  Serial.print(error);
  Serial.print("\t");
  Serial.print("Left Speed: ");
  Serial.print(leftMotorSpeed);
  Serial.print("\t");
  Serial.print("Right Speed: ");
  Serial.println(rightMotorSpeed);
  
  delay(10); // Add delay to avoid overloading the motors
}