#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

const char* ssid = "Redmi Note 10S";
const char* password = "123456789";
const char* mqtt_server = "5.196.78.28";  

// GPIO Pin Definitions
#define PIN_4 4
#define PIN_5 5
#define PIN_18 18
#define PIN_19 19
#define PIN_22 22
#define PIN_23 23
#define PIN_13 13
#define PIN_12 12

WiFiClient espClient;
PubSubClient client(espClient);

// Default Latitude and Longitude
float default_latitude = 11.2746893;  
float default_longitude = 77.6078799;

// Constants for distance calculations
const float EARTH_RADIUS_KM = 6371.0;
const float EMERGENCY_THRESHOLD = 3.0; // 3 km

// Haversine formula to calculate distance
float haversine(float lat1, float lon1, float lat2, float lon2) {
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(lat1) * cos(lat2) *
            sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return EARTH_RADIUS_KM * c;
}

// Calculate bearing from one point to another
float calculate_bearing(float lat1, float lon1, float lat2, float lon2) {
  float dLon = radians(lon2 - lon1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  
  float y = sin(dLon) * cos(lat2);
  float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
  float bearing = atan2(y, x);
  bearing = degrees(bearing);
  
  return (bearing < 0) ? bearing + 360.0 : bearing;
}

// Get cardinal direction based on bearing
String get_cardinal_direction(float bearing) {
  if (bearing >= 337.5 || bearing < 22.5) return "North";
  if (bearing >= 22.5 && bearing < 67.5) return "Northeast";
  if (bearing >= 67.5 && bearing < 112.5) return "East";
  if (bearing >= 112.5 && bearing < 157.5) return "Southeast";
  if (bearing >= 157.5 && bearing < 202.5) return "South";
  if (bearing >= 202.5 && bearing < 247.5) return "Southwest";
  if (bearing >= 247.5 && bearing < 292.5) return "West";
  if (bearing >= 292.5 && bearing < 337.5) return "Northwest";
  return "Unknown";
}

// Check if coordinates are within emergency range
void check_emergency(float lat, float lon) {
  if (haversine(default_latitude, default_longitude, lat, lon) <= EMERGENCY_THRESHOLD) {
    client.publish("device/emergency", "Emergency");
  }
}

// Connect to Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT message callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == "device/coordinates") {
    float received_latitude, received_longitude;
    int commaIndex = message.indexOf(',');
    
    if (commaIndex > 0) {
      received_latitude = message.substring(0, commaIndex).toFloat();
      received_longitude = message.substring(commaIndex + 1).toFloat();
    } else {
      Serial.println("Invalid message format. Using default coordinates.");
      received_latitude = default_latitude;
      received_longitude = default_longitude;
    }

    float distance = haversine(default_latitude, default_longitude, received_latitude, received_longitude);
    Serial.print("Distance to received coordinates: ");
    Serial.print(distance);
    Serial.println(" km");

    check_emergency(received_latitude, received_longitude);
    
    String lat_lon_message = String(received_latitude) + "," + String(received_longitude);
    client.publish("device/coordinates_response", lat_lon_message.c_str());

    if (distance <= EMERGENCY_THRESHOLD) {
      float bearing = calculate_bearing(default_latitude, default_longitude, received_latitude, received_longitude);
      Serial.print("Direction (Bearing): ");
      Serial.print(bearing);
      Serial.println(" degrees");

      String direction = get_cardinal_direction(bearing);
      Serial.print("Cardinal Direction: ");
      Serial.println(direction);

      String bearing_message = String(bearing) + " degrees, " + direction + ", Distance: " + String(distance) + " km";
      client.publish("device/traffic1", bearing_message.c_str());

      // Publish direction-specific messages
      if (direction == "North") client.publish("device/traffic1", "A");
      else if (direction == "East") client.publish("device/traffic1", "B");
      else if (direction == "South") client.publish("device/traffic1", "C");
      else if (direction == "West") client.publish("device/traffic1", "D");
      else Serial.println("Direction is unknown. No message sent.");
    } else {
      Serial.println("Out of range");
    }
  }

  // Toggle LEDs based on the message
  if (message == "A") {
    digitalWrite(PIN_5, HIGH);
    digitalWrite(PIN_18, HIGH);
    digitalWrite(PIN_22, HIGH);
    digitalWrite(PIN_13, HIGH);
    digitalWrite(PIN_4, LOW);
    digitalWrite(PIN_23, LOW);
    digitalWrite(PIN_12, LOW);
    digitalWrite(PIN_19, LOW);
  } else if (message == "D") {
    digitalWrite(PIN_19, HIGH);
    digitalWrite(PIN_4, HIGH);
    digitalWrite(PIN_22, HIGH);
    digitalWrite(PIN_13, HIGH);
    digitalWrite(PIN_5, LOW);
    digitalWrite(PIN_18, LOW);
    digitalWrite(PIN_23, LOW);
    digitalWrite(PIN_12, LOW);
  } else if (message == "C") {
    digitalWrite(PIN_23, HIGH);
    digitalWrite(PIN_4, HIGH);
    digitalWrite(PIN_18, HIGH);
    digitalWrite(PIN_13, HIGH);
    digitalWrite(PIN_5, LOW);
    digitalWrite(PIN_19, LOW);
    digitalWrite(PIN_22, LOW);
    digitalWrite(PIN_12, LOW);
  } else if (message == "B") {
    digitalWrite(PIN_12, HIGH);
    digitalWrite(PIN_4, HIGH);
    digitalWrite(PIN_18, HIGH);
    digitalWrite(PIN_22, HIGH);
    digitalWrite(PIN_5, LOW);
    digitalWrite(PIN_19, LOW);
    digitalWrite(PIN_23, LOW);
    digitalWrite(PIN_13, LOW);
  }
}

// Reconnect to MQTT server
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("device/status", "MQTT Server is Connected");
      client.subscribe("device/coordinates");
      client.subscribe("device/traffic1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Setup function
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Initialize GPIO pins
  pinMode(PIN_4, OUTPUT);
  pinMode(PIN_5, OUTPUT);
  pinMode(PIN_18, OUTPUT);
  pinMode(PIN_19, OUTPUT);
  pinMode(PIN_22, OUTPUT);
  pinMode(PIN_23, OUTPUT);
  pinMode(PIN_13, OUTPUT);
  pinMode(PIN_12, OUTPUT);

  // Ensure all pins are LOW initially
  digitalWrite(PIN_4, LOW);
  digitalWrite(PIN_5, LOW);
  digitalWrite(PIN_18, LOW);
  digitalWrite(PIN_19, LOW);
  digitalWrite(PIN_22, LOW);
  digitalWrite(PIN_23, LOW);
  digitalWrite(PIN_13, LOW);
  digitalWrite(PIN_12, LOW);
}

// Main loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}