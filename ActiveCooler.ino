/* Simple Active Cooler
   Bhautik (Brian) Amin
   brianamin@dpiuav.com



*/
#include <DallasTemperature.h>
#include <OneWire.h>


#define PIN_TEMP_ONEWIRE 7


// Temperature Ranges (celsius)
#define TEMP_MIN 80
#define TEMP_MAX 105 // F
// Motor Speed Ranges
#define MOTOR_MIN 20 // (In terms of duty cycle) At TEMP_MIN this speed will engage
#define MOTOR_MAX 100 // (In terms of duty cycle) At TEMP_MAX this speed will engage



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


    // Set timer 4 for 25KHz PWM frequency
    TCCR4A = 0;
    TCCR4B = 4;
    TCCR4C = 0;
    TCCR4D = 0;
    // PLL Configuration
    // Use 96MHz / 2 = 48MHz
    PLLFRQ=(PLLFRQ&0xCF)|0x30;
    // Terminal count for Timer 4 PWM
    OCR4C=255;
}

void loop(void)
{


    /*
        sensors.requestTemperatures() issues a global command
        to all connected sensors on the bus
     */ 
    sensors.requestTemperatures();

    float temp = sensors.getTempCByIndex(0);
    // Check if reading was successful

    if (temp != DEVICE_DISCONNECTED_C)
    {
        Serial.print(temp);
        Serial.print(",");
        // Map the temperature to fan pwm value
        pwm = map(temp, TEMP_MIN, TEMP_MAX, 0, 255);
        Serial.println(pwm);
    }
    else
    {
        Serial.println("ERROR");
        pwm = 0;
    }




    // Pin 6 on Sparkfun Pro Micro is connected to Timer4
    OCR4D = pwm; // Set PWM
    DDRD|=1<<7; // Set output mode D7
    TCCR4C|=0x09; // Activate channel d






}
