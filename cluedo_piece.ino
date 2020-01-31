#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

#include "settings.h"

#define LIGHT_UPDATE_INTERVAL 5

#define PIN         5
#define NUMPIXELS  13

// Reusable colours
#define COLOUR_OFF              pixels.Color(0, 0, 0)
#define COLOUR_WIFI_CONNECTING  pixels.Color(255, 0, 255)
#define COLOUR_WIFI_CONNECTED   pixels.Color(0, 255, 0)
#define COLOUR_STANDBY          pixels.Color(50, 50, 100)
#define COLOUR_ERROR            pixels.Color(255, 0, 0)

#define COLOUR_AVAILABLE          pixels.Color(0, 255, 0)
#define COLOUR_BUSY_SOON          pixels.Color(255, 191, 0)
#define COLOUR_BUSY               pixels.Color(0, 0, 255)
#define COLOUR_MEETING_ENDS_SOON  pixels.Color(255, 0, 0)

// We do some timing
unsigned long current_clock;
unsigned long last_data_update_clock = 0;
unsigned long last_light_refresh_clock = 0;

// Set up the pixels and the room colour variable
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint32_t colour_room;

// So we can actually connect to things
HTTPClient http;

// Here's the room URL we hit up for the data
String ROOM_JSON_URL = (String) "https://" + ROOMS_DASHBOARD_HOST + "/room/" + ROOM_SLUG + ".json?compact=true";

const size_t capacity = 2000;  // Use https://arduinojson.org/v6/assistant/ to calculate this, then round up for sanity
DynamicJsonDocument doc(capacity);

// Declare things we use from the JSON
unsigned int room_colour_r;
unsigned int room_colour_g;
unsigned int room_colour_b;
bool enable_presence_device;
bool empty;
int minutes_to_next_event;
int minutes_to_end_of_event;
bool upcoming_event_today;

// Declare things we use for fancy lighting
unsigned int fade_step = 0;
bool light_top_pulse = false;
uint8_t top_colour_r;
uint8_t top_colour_g;
uint8_t top_colour_b;
uint32_t base_colour;
uint32_t top_colour;

void set_base_colour(uint32_t colour)
{
  base_colour = colour;
}

void set_top_colour(uint32_t colour)
{
  top_colour_r = colour >> 16;
  top_colour_g = colour >> 8;
  top_colour_b = colour;
}

void set_top_pulsing(bool pulse)
{
  light_top_pulse = pulse;
}

void update_lights()
{

  current_clock = millis();

  if (current_clock - last_light_refresh_clock >= LIGHT_UPDATE_INTERVAL)
  {
    
    float SpeedFactor = 0.008; // I don't actually know what would look good
  
    if (light_top_pulse)
    {
      float intensity_r = top_colour_r / 2.0 * (1.0 + sin(SpeedFactor * fade_step));
      float intensity_g = top_colour_g / 2.0 * (1.0 + sin(SpeedFactor * fade_step));
      float intensity_b = top_colour_b / 2.0 * (1.0 + sin(SpeedFactor * fade_step));
      top_colour = pixels.Color(intensity_r, intensity_g, intensity_b);
    }
    else
    {
      top_colour = pixels.Color(top_colour_r, top_colour_g, top_colour_b);
    }

    pixels.fill(base_colour, 0, 12);
    
    pixels.setPixelColor(12, top_colour);
    
    pixels.show();

    if (fade_step < 65535)
    {
      fade_step++;
    }
    else
    {
      fade_step = 0;
    }

    last_light_refresh_clock = current_clock;
    
  }
  
}

void update_from_server()
{

  Serial.println("Updating data from server");
  
  http.begin(ROOM_JSON_URL, ROOMS_DASHBOARD_SHA1_FINGERPRINT);

  int httpCode = http.GET();

  String json;

  if (httpCode == 200) { //Check the returning code
 
    json = http.getString();
    Serial.println(json);
    deserializeJson(doc, json);

    // Should we even be switched on?
    enable_presence_device = doc["enable_presence_device"];
  
    if (enable_presence_device)
    {
  
      // Pluck the pretty colour for the room from the JSON
      JsonArray colour = doc["colour"];
      room_colour_r = colour[0];
      room_colour_g = colour[1];
      room_colour_b = colour[2];
      colour_room = pixels.Color(room_colour_r, room_colour_g, room_colour_b);
    
      // Set the base colour
      set_base_colour(colour_room);
  
      // Some logic about what colour to actually be
  
      empty = doc["empty"];
      minutes_to_next_event = doc["minutes_to_next_event"];
      minutes_to_end_of_event = doc["minutes_to_end_of_event"];
      upcoming_event_today = doc["upcoming_event_today"];
  
      // First of all, are we empty?
      if (empty)
      {
        Serial.println("Room currently empty.");
        set_top_pulsing(false);
        
        // If there an event starting soon?
        if (upcoming_event_today and minutes_to_next_event <= MINUTES_BEFORE_BUSY_TO_WARN)
        {
          Serial.println("Event starting soon.");
          set_top_colour(COLOUR_BUSY_SOON);
        }
        else
        {
          Serial.println("No event starting soon.");
          set_top_colour(COLOUR_AVAILABLE);
        }
      }
      // We're not empty, so apply the 'room busy' logic
      else
      {
        Serial.println("Room currently occupied.");
        // Is the end of the event soon?
        if (minutes_to_end_of_event <= MINUTES_BEFORE_END_OF_MEETING_TO_WARN)
        {
          Serial.println("Current event ending soon.");
          set_top_colour(COLOUR_MEETING_ENDS_SOON);
  
          // Is there another event coming up soon?
          if (upcoming_event_today and minutes_to_next_event <= MINUTES_BEFORE_BUSY_TO_WARN)
          {
            Serial.println("Other event starting soon.");
            set_top_pulsing(true);
          }
          else
          {
            Serial.println("No other event starting soon.");
            set_top_pulsing(false);
          }
        }
        else{
          Serial.println("Current event not ending soon.");
          set_top_pulsing(false);
          set_top_colour(COLOUR_BUSY);
          upcoming_event_today;
        }
      }
    }
    else
    {
      Serial.println("Asleep.");
      set_top_pulsing(false);
      set_base_colour(COLOUR_OFF);
      set_top_colour(COLOUR_STANDBY);
    }
  
  }
  else
  {
    Serial.println("Unable to get data.");
    set_top_pulsing(true);
    set_base_colour(COLOUR_OFF);
    set_top_colour(COLOUR_ERROR);
  }
   
  http.end();

}

void setup()
{

  // Set up the serial in case we want to talk to a human
  Serial.begin(9600);
  Serial.println();

  // We want to be able to control the pixels
  pixels.begin();

  set_base_colour(COLOUR_OFF);

  // Wifi is delicious
  WiFi.begin(WIFI_NETWORK, WIFI_KEY);
  Serial.print("Connecting");
  
  set_top_pulsing(false);
  set_top_colour(COLOUR_WIFI_CONNECTING);

  while (WiFi.status() != WL_CONNECTED)
  {
    update_lights();
    delay(20);
    Serial.print(".");
  }

  int connected_at = millis();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  set_top_colour(COLOUR_WIFI_CONNECTED);

  while (millis() - connected_at < 2500) {
    update_lights();
  }
  
  update_from_server();

}

void loop() {

  current_clock = millis();

  update_lights();

  if (current_clock - last_data_update_clock >= DATA_UPDATE_INTERVAL) {
    update_from_server();
    last_data_update_clock = current_clock;
  }

}
