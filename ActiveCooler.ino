/* 
    Simple Active Cooler
    Bhautik (Brian) Amin
    brianamin@dpiuav.com
*/
#include <DallasTemperature.h>
#include <OneWire.h>

// Pins
#define PIN_TEMP_ONEWIRE 7
#define PIN_MOTOR_OUTPUT 9

// Temperature Ranges (fahrenheit)
#define TEMP_MIN 70
#define TEMP_MAX 105
// Motor Speed Ranges
#define MOTOR_MIN 20 // (In terms of duty cycle) At TEMP_MIN this speed will engage
#define MOTOR_MAX 100 // (In terms of duty cycle) At TEMP_MAX this speed will engage

#define PWM_TOP 639 // TOP Value for PWM
#define INTERRUPT_TOP 199 // TOP Value for Data Sampling ISR

// Configure onewire object
OneWire oneWire(PIN_TEMP_ONEWIRE);

// Pass onewire object reference to Temperature library
DallasTemperature sensors(&oneWire);

volatile int pwm = 0;

volatile int temperature = 0;

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

ISR(TIMER3_CAPT_vect)
{
    /*
        sensors.requestTemperatures() issues a global command
        to all connected sensors on the bus
     */ 
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

}

void setup(void)
{
    Serial.begin(115200);
    // Start the temperature library
    sensors.begin();
    pinMode(PIN_MOTOR_OUTPUT, OUTPUT);
    // Using Timer 1 for PWM (OC1A, PB5, Pin 9 on Sparkfun Board)
    // Set Timer 1 for 25kHz frequency
    // 25kHz = (16 Mhz / PRESCALER) / (TOP + 1)
    // Prescaler = 1, TOP = 639
    TCCR1A = (_BV(COM1A1) |_BV(WGM11) ); // Noninverting PWM signal
    TCCR1B = (_BV(WGM13) | _BV(WGM12) | _BV(CS10) ); // Prescaler 1
    TCCR1C = 0;
    ICR1 = PWM_TOP; // TOP value
    DDRB |= _BV(DDB5); // Enable output

    // Timer 3 Setup for sampling temperature sensor
    // Sampling Rate: 40Khz
    TCCR3A = 0;
    TCCR3B = (_BV(WGM33) | _BV(WGM32) | _BV(CS30)); // Prescaler 1, Mode 12 WGM
    TCCR3C = 0;
    ICR3 = INTERRUPT_TOP; // TOP value
    sei(); // Global Interrupt Enable
    TIFR3 = (_BV(ICF3)); // Enable Input Capture Flag
    TIMSK3 = (_BV(ICIE3)); // Enable Input Capture Interrupt

}

void loop(void)
{
    // Pin 9 on Sparkfun Pro Micro is connected to Timer 1
    cli(); // Disable Interrupts
    int output = pwm; // Copy PWM value
    sei(); // Enable Interrupts
    OCR1A = output; // Set PWM
}
