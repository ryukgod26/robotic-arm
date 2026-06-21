#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <ESP32PWM.h>

const char* ssid = "Bablu";
const char* password = "Ravisumit";

WebServer server(80);

Servo motor1;
Servo motor2;
Servo motor3;

const int servoPin1 = 22;
const int servoPin2 = 23;
const int servoPin3 = 25;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Robotic Arm</title>
    <style>
        body{
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            background-color: #f4f4f9;
        }
        
        h2{
            color: #333;
        }

        .slider-container{
            background: #fff;
            padding: 20px;
            margin: 15px auto;
            border-radius: 8px;
            max-width: 400px;
            box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.1);
        }
        
        p{
            font-size: 1.2rem;
            font-weight: bold;
            color: #555;
        }

    </style>
</head>
<body>
    <h2>Robotic Arm Control</h2>

    <div class="slider-container">
        <p>Motor1: <span id="val1">90</span>&deg;</p>
        <input type="range" min="0" max="180" value="90" class="slider" id="motor1" oninput="updateMotor(1, this.value)">
    </div>

    <div class="slider-container">
        <p>Motor2: <span id="val2">90</span>&deg;</p>
        <input type="range" min="0" max="180" value="90" class="slider" id="motor2" oninput="updateMotor(2, this.value)">
    </div>

    <div class="slider-container">
        <p>Motor3: <span id="val3">90</span>&deg;</p>
        <input type="range" min="0" max="180" value="90" class="slider" id="motor3" oninput="updateMotor(3, this.value)">
    </div>

    <script>
        function updateMotor(motor, angle){
            document.getElementById("val" + motor).innerHTML = angle;
            fetch(`/set?motor=${motor}&angle=${angle}`);
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot(){
  server.send(200, "text/html", index_html);
}

void handleSet(){
  if (server.hasArg("motor") && server.hasArg("angle")){
    int motor = server.arg("motor").toInt();
    int angle = server.arg("angle").toInt();

    angle = constrain(angle, 0, 100);

    if (motor == 1) motor1.write(angle);
    else if (motor == 2) motor2.write(angle);
    else if (motor == 3) motor3.write(angle);

  server.send(200, "text/plain", "OK");
  } else{
  server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  motor1.setPeriodHertz(50);
  motor2.setPeriodHertz(50);
  motor3.setPeriodHertz(50);

  motor1.attach(servoPin1, 500, 2400);
  motor2.attach(servoPin2, 500, 2400);
  motor3.attach(servoPin3, 500, 2400);

  motor1.write(90);
  motor2.write(90);
  motor3.write(90);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Wifi Connected Successfully.");
  Serial.print("Go o this link to access portal : http:/");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSet);

  server.begin();
}

void loop() {
  server.handleClient();
}
