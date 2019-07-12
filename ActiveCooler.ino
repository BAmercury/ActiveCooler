/* 
    Simple Active Cooler
    Bhautik (Brian) Amin
    brianamin@dpiuav.com

    Device: SparkFun Pro Micro 5V/16MHz (Atmega32U4)
    Temperature Sensor: DS18B20
    Fan: SanAce 92W XF-P05801, DC 24V/1.0A
*/
#include <DallasTemperature.h>
#include <OneWire.h>

// Pins
#define PIN_TEMP_ONEWIRE 7
#define PIN_MOTOR_OUTPUT 9
#define PIN_DEBUG 2

// Temperature Ranges (fahrenheit)
#define TEMP_MIN 70
#define TEMP_MAX 105
// Motor Speed Ranges
#define MOTOR_MIN 20 // (In terms of duty cycle) At TEMP_MIN this speed will engage
#define MOTOR_MAX 100 // (In terms of duty cycle) At TEMP_MAX this speed will engage

#define PWM_TOP 639 // TOP Value for PWM

unsigned long SAMPLING_PERIOD = 30; // Period in Seconds
unsigned long previous_ms = 0; 

// Configure onewire object
OneWire oneWire(PIN_TEMP_ONEWIRE);

// Pass onewire object reference to Temperature library
DallasTemperature sensors(&oneWire);

int pwm = 0;
int temperature = 0;

// Convert given duty cycle to valid pwm value
long DUTY2PWM(int duty)
{
    if (duty == 100)
    {
        return PWM_TOP;
    }
    else
    {
        long pwm = ( (PWM_TOP * duty) / 100);
        return pwm;
    }

}

void setup(void)
{
    Serial.begin(115200);
    // Start the temperature library
    sensors.begin();
    pinMode(PIN_MOTOR_OUTPUT, OUTPUT);
    pinMode(PIN_DEBUG, OUTPUT);
    // Using Timer 1 for PWM (OC1A, PB5, Pin 9 on Sparkfun Board)
    // Set Timer 1 for 25kHz frequency
    // 25kHz = (16 Mhz / PRESCALER) / (TOP + 1)
    // Prescaler = 1, TOP = 639
    TCCR1A = (_BV(COM1A1) |_BV(WGM11) ); // Noninverting PWM signal
    TCCR1B = (_BV(WGM13) | _BV(WGM12) | _BV(CS10) ); // Prescaler 1
    TCCR1C = 0;
    ICR1 = PWM_TOP; // TOP value
    DDRB |= _BV(DDB5); // Enable output




}

void loop(void)
{

    static unsigned long current_ms = millis();
    if (current_ms - previous_ms >= SAMPLING_PERIOD)
    {

        /*
            sensors.requestTemperatures() issues a global command
            to all connected sensors on the bus
        */ 
        digitalWrite(PIN_DEBUG, HIGH); // Set Debug pin how, use scope to measure sensor update rate
        sensors.requestTemperatures();
        temperature = sensors.getTempFByIndex(0);

        // Refer to DallasTemperature.h for error codes
        // If below negative -196.6 yield an error
        // If the sensor wire is disconnected or if sensor is removed from ground
        if (temperature != DEVICE_DISCONNECTED_F)
        {
            Serial.print("Temperatuare (F): ");
            Serial.print(temperature);
            Serial.print(", ");
            // Map the temperature to fan pwm value
            pwm = map(temperature, TEMP_MIN, TEMP_MAX, DUTY2PWM(MOTOR_MIN), DUTY2PWM(MOTOR_MAX));
            Serial.print("PWM: ");
            // Apply limits (0-639)
            if (pwm > PWM_TOP)
            {
                pwm = PWM_TOP;
            }
            Serial.println(pwm);
        }
        else
        {
            Serial.println("ERROR");
            pwm = 0;
        }
        previous_ms = millis();
        digitalWrite(PIN_DEBUG, LOW);
    }

    // Pin 9 on Sparkfun Pro Micro is connected to Timer 1
    OCR1A = pwm; // Set PWM
}
