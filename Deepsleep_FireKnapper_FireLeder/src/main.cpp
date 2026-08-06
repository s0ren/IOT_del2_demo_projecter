#include <Arduino.h>
#include "OneButton.h"
#include "driver/rtc_io.h"

const int BuildinledPin = 2; // GPIO2 is often connected to the onboard LED on ESP32 boards
const int ButtonPin = 32;    // GPIO 32 is often used for a button on ESP32 boards
const int LedPin1 = 4; // GPIO 04 for an external LED
const int LedPin2 = 5; // GPIO 05 for another external LED


int ledState1 = LOW; 
int ledState2 = LOW; 
int lastButtonState = LOW; // Previous state of the button
int debounceDelay = 50; // Debounce time in milliseconds
int lastTime = 0; // Last time the button state changed

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds) */

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex
#define USE_EXT0_WAKEUP          1               // 1 = EXT0 wakeup, 0 = EXT1 wakeup
#define WAKEUP_GPIO              GPIO_NUM_32     // Only RTC IO are allowed - ESP32 Pin example
RTC_DATA_ATTR int bootCount = 0;

OneButton button;

void Click(); // function prototype for button click event

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);  //Take some time to open up the Serial Monitor

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  switch (esp_sleep_get_wakeup_cause() )
  {
    case ESP_SLEEP_WAKEUP_EXT0:
    case ESP_SLEEP_WAKEUP_EXT1:  Click(); break;
    case ESP_SLEEP_WAKEUP_TIMER: {
      // Serial.writeln("Woke up from timer or external button press");
      ledState2 = HIGH;
      digitalWrite(LedPin2, ledState2);
    };
    break;
  }

#if USE_EXT0_WAKEUP
  esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO, 1);  //1 = High, 0 = Low
  // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
  // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
  // No need to keep that power domain explicitly, unlike EXT1.
  rtc_gpio_pullup_dis(WAKEUP_GPIO);
  rtc_gpio_pulldown_en(WAKEUP_GPIO);

  gpio_hold_dis(GPIO_NUM_5);

#else  // EXT1 WAKEUP
  //If you were to use ext1, you would use it like
  esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(WAKEUP_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);
  /*
    If there are no external pull-up/downs, tie wakeup pins to inactive level with internal pull-up/downs via RTC IO
         during deepsleep. However, RTC IO relies on the RTC_PERIPH power domain. Keeping this power domain on will
         increase some power consumption. However, if we turn off the RTC_PERIPH domain or if certain chips lack the RTC_PERIPH
         domain, we will use the HOLD feature to maintain the pull-up and pull-down on the pins during sleep.
  */
  rtc_gpio_pulldown_en(WAKEUP_GPIO);  // GPIO33 is tie to GND in order to wake up in HIGH
  rtc_gpio_pullup_dis(WAKEUP_GPIO);   // Disable PULL_UP in order to allow it to wakeup on HIGH
#endif

  // put your setup code here, to run once:
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);
  digitalWrite(LedPin1, ledState1); // Ensure LED is off at start
  digitalWrite(LedPin2, ledState2); // Ensure LED is off at start
  delay(1000); // Wait for a second

  pinMode(ButtonPin, INPUT);

  // setup OneButton
  button.setup(ButtonPin, INPUT, false);

  // link the click function to be called on a doubleclick event.
  button.attachClick(Click);
}

void Click() {
  Serial.println("Button clicked!");
  // ledState1 = !ledState1; // Toggle LED state
  // digitalWrite(LedPin1, ledState1); // Update LED based on ledState
  // ledState2 = !ledState2; // Toggle LED state
  // digitalWrite(LedPin2, ledState2); // Update LED based on ledState
  ledState1 = HIGH;
  digitalWrite(LedPin1, ledState1); // Update LED based on ledState

  lastTime = millis(); // Update lastTime to current time
}

void loop() {
  

  if (millis() - lastTime < 10000) {
   
    button.tick(); // check the button state and trigger events
  }
  else {
    // Serial.println("Entering deep sleep for 30 seconds...");
    Serial.println("Entering deep sleep until button press...");
    delay(1000); // Give time to print message
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.flush(); // Ensure all serial data is sent before sleeping
    digitalWrite(LedPin1, LOW); // Turn off LED1 before sleeping
    digitalWrite(LedPin2, LOW); // Turn off LED2 before sleeping
    gpio_hold_dis(GPIO_NUM_5);
    esp_deep_sleep_start();
  }

}

