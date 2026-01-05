#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#define DHTPIN D5
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
Servo servo;

const char* ssid = ""; // masukan wifi
const char* password = ""; // masukan password wifi

const char* mqtt_user   = "";       // sama dengan di mosquitto_passwd
const char* mqtt_pass   = "";  // password user tersebut

const char* mqtt_server = "";
const int mqtt_port = 1883;


// Topic MQTT
const char* topic_temp      = "home/temp";
const char* topic_humidity  = "home/humidity";
const char* topic_ldr       = "home/brightness";
const char* topic_pir       = "home/pir";

const char* topic_lamp_cmd  = "home/lamp/cmd";
const char* topic_lamp_stat = "home/lamp/state";
const char* topic_lamp_mode = "home/lamp/mode";

const char* topic_ac_cmd  = "home/ac/cmd";
const char* topic_ac_stat = "home/ac/state";


int ldrPin   = A0;
int servoPin = D0;
int ledPin   = D2;
int pirPin   = D6;
int button   = D7;

bool lampState = false;
float humidity = 0.0;
float temperatureC = 0.0;
int servoPos = 0;

bool autoModeLamp = true;    // default = AUTO
int pressCount = 0;
unsigned long lastDebounceLamp = 0;

bool getLampMode() {
  return (pressCount % 2 == 0);
}


WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    
    if (client.connect("ESP8266_Client", mqtt_user, mqtt_pass)) {
      Serial.println("Connected.");

      client.subscribe(topic_lamp_cmd);
      client.subscribe(topic_ac_cmd);

    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi 3 detik.");
      delay(3000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  };

  Serial.println(msg);
  msg.trim();
  msg.toUpperCase();

  if (String(topic) == topic_ac_cmd) {
    if (msg == "ON") {
      servo.write(90);
      servoPos = 180;
      Serial.println("Servo bergerak ke 90°");
      client.publish("home/ac/state", "ON");
    } 
    
    else if (msg == "OFF") {
      servo.write(0);
      servoPos = 0;
      Serial.println("Servo kembali ke 0°");
      client.publish("home/ac/state", "OFF");
    }
  }

  if (String(topic) == topic_lamp_cmd && autoModeLamp == false) {
      if (msg == "ON") {
          digitalWrite(ledPin, HIGH);
          client.publish(topic_lamp_stat, "ON");
      } 
      else if (msg == "OFF") {
          digitalWrite(ledPin, LOW);
          client.publish(topic_lamp_stat, "OFF");
      }
  }

}

void handleLampAuto(int adc) {
  if (autoModeLamp) {
    if (adc < 400) {  // semakin kecil = semakin gelap
      digitalWrite(ledPin, HIGH);
      client.publish(topic_lamp_stat, "ON");
      Serial.println("Lampu ON (AUTO - gelap)");
    } else {
      digitalWrite(ledPin, LOW);
      client.publish(topic_lamp_stat, "OFF");
      Serial.println("Lampu OFF (AUTO - terang)");
    }
  }
}

bool lastButtonLamp = HIGH;

void toggleLampMode() {
  bool btn = digitalRead(button);

  // Deteksi transisi (HIGH → LOW)
  if (btn == LOW && lastButtonLamp == HIGH && millis() - lastDebounceLamp > 250) {
    pressCount++;  
    lastDebounceLamp = millis();

    autoModeLamp = getLampMode();

    Serial.print("PressCount: ");
    Serial.println(pressCount);
    Serial.print("Mode Lamp: ");
    Serial.println(autoModeLamp ? "AUTO" : "MANUAL");

    // publish mode
    client.publish(topic_lamp_mode, autoModeLamp ? "AUTO" : "MANUAL");
  }

  lastButtonLamp = btn;
}



void setup() {
  // timeClient.begin();
  Serial.begin(115200);

  Serial.println("Testing DHT22...");
  dht.begin();
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  servo.attach(servoPin);

    // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT22 tidak terbaca (NAN)");
  } else {
    Serial.print("Humidity: "); Serial.println(h);
    Serial.print("Temperature: "); Serial.println(t);
    client.publish(topic_temp, String(t).c_str());
    client.publish(topic_humidity, String(h).c_str());
  }

  // LDR
  int adc = analogRead(ldrPin);
  Serial.print("LDR ADC: "); Serial.println(adc);
  client.publish(topic_ldr, String(adc).c_str());

  // PIR
  int pirValue = digitalRead(pirPin);
  Serial.print("PIR: "); Serial.println(pirValue);
  client.publish(topic_pir, String(pirValue).c_str());

  toggleLampMode(); // cek tombol mode lampu
  if (autoModeLamp){
    handleLampAuto(adc);
  }
  delay(20000);
}
