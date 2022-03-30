#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

Servo myservo;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "pZvbstT2o9K5yf4g_4l7QYLlrvcFiSO0";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MSI 6691";
char pass[] = "123456789";

int Turn = 0;
int Drive = 0;

int servoPin = 27;
int motorPin = 26;

int led_blue = 25;
int led_green = 32;
int led_red = 33;

int ctsPin = 34;
bool cts = false;

void setup() {
  Serial.begin(115200);
  // wifi and blynk
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);

  // Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);

  // DC motor
  pinMode(motorPin, OUTPUT);

  // CTS
  pinMode(ctsPin, INPUT);

  // RGB
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
}

void loop() {
  if (digitalRead(ctsPin) == LOW)
  {
    if (cts == true)
    {
      cts = false;
      Serial.println("Program stop");
    }
    else
    {
      cts = true;
      Serial.println("Program start");
    }
    delay(4000);
  }

  if (cts == true)
  {
    Blynk.run();

    myservo.write(map(Turn, 0, 1023, 0, 180));

    analogWrite(motorPin, Drive);
    if (Drive == 512)
    {
      digitalWrite(led_red, HIGH);
      digitalWrite(led_blue, LOW);
      digitalWrite(led_green, LOW);
      Blynk.virtualWrite(V2, 200);
      Blynk.virtualWrite(V3, 0);
    }
    else if (Drive < 512 && Drive > 23)
    {
      digitalWrite(led_red, LOW);
      digitalWrite(led_blue, HIGH);
      digitalWrite(led_green, LOW);
      Blynk.virtualWrite(V2, 600);
      Blynk.virtualWrite(V3, map(Drive, 512, 23, 0, 119));
    }
    else if (Drive < 23)
    {
      digitalWrite(led_red, LOW);
      digitalWrite(led_blue, LOW);
      digitalWrite(led_green, HIGH);
      Blynk.virtualWrite(V2, 100);
      Blynk.virtualWrite(V3, 120);
    }
  }
  else
  {
    digitalWrite(led_red, LOW);
    digitalWrite(led_blue, LOW);
    digitalWrite(led_green, LOW);
  }
}

BLYNK_WRITE(V1)
{
  Turn = param.asInt();
}

BLYNK_WRITE(V0)
{
  Drive = param.asInt();
}
