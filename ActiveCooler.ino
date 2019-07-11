/* 
    Simple Active Cooler
    Bhautik (Brian) Amin
    brianamin@dpiuav.com
*/
#include <DallasTemperature.h>
#include <OneWire.h>

// Pins
#define PIN_TEMP_ONEWIRE 7


// Temperature Ranges (fahrenheit)
#define TEMP_MIN 80
#define TEMP_MAX 105
// Motor Speed Ranges
#define MOTOR_MIN 20 // (In terms of duty cycle) At TEMP_MIN this speed will engage
#define MOTOR_MAX 100 // (In terms of duty cycle) At TEMP_MAX this speed will engage

// Macro to converts from duty (0..100) to PWM (0..79)
#define DUTY2PWM(x)  ((639*(x))/100)

// Configure onewire object
OneWire oneWire(PIN_TEMP_ONEWIRE);

// Pass onewire object reference to Temperature library
DallasTemperature sensors(&oneWire);

int pwm = 0;

void setup(void)
{
    Serial.begin(115200);

    // Start the temperature library
    sensors.begin();
    pinMode(9, OUTPUT);
    // Using Timer 1 for PWM (OC1A, PB5, Pin 9)
    // Set Timer 1 for 25kHz frequency
    TCCR1A = (_BV(COM1A1) |_BV(WGM11) ); // Noninverting PWM signal
    TCCR1B = (_BV(WGM13) | _BV(WGM12) | _BV(CS10) ); // Prescaler 1
    // 25kHz = (16 Mhz / 1) / (639 + 1)
    TCCR1C = 0;
    ICR1 = 639; // TOP value



}

void loop(void)
{


    /*
        sensors.requestTemperatures() issues a global command
        to all connected sensors on the bus
     */ 
    sensors.requestTemperatures();

    float temp = sensors.getTempFByIndex(0);
    // Check if reading was successful

    if (temp != DEVICE_DISCONNECTED_C) // Refer to DallasTemperature.h for error codes
    {
        Serial.print("Temperatuare (F): ");
        Serial.print(temp);
        Serial.print(", ");
        // Map the temperature to fan pwm value
        pwm = map(temp, TEMP_MIN, TEMP_MAX, DUTY2PWM(MOTOR_MIN), DUTY2PWM(MOTOR_MAX));
        Serial.print("PWM: ");
        Serial.println(pwm);
    }
    else
    {
        Serial.println("ERROR");
        pwm = 0;
    }


    // Pin 6 on Sparkfun Pro Micro is connected to Timer4
    OCR1A = pwm; // Set PWM
    DDRB |= _BV(DDB5);






}
