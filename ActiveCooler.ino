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

// Macro to converts from duty (0..100) to PWM (0..255)
#define DUTY2PWM(x)  ((255*(x))/100)

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
    
    /*
        TCCR4A Configuration
        Bits 7,6 for OCW4A + OCR4A connection: 0,0
        Bits 5,4 OCW4B + OC4B connection: 0,0
        Bit 3,2 FOC Match: 0,0
        Bit 1 PWM4A for OCR4A: 0
        Bit 0 PWM4B for OCR4B: 0  
    */
    TCCR4A = 0;

    /*
        TCCR4B Configuration
        Bit 7: PWM Inversion Mode, 0
        Bit 6: Prescaler Reset, 0
        Bit 5,4: Dead Time Prescaler, 0
        Bit 3..0: Prescaling source for Timer 4: 0,1,0,0
    */
    TCCR4B = 4; // PCK/8 (asyc) or CK/8 (sync)



    /*
        TCCR4C Configuration
        Bit 7,6,5,4: Comparator output modes: 0
        Bit 3,2: Comparator D Output mode
        Bit 1: FOC4D, 0
        Bit 0: OCR4D Pwm mode, 1
     */
    TCCR4C = 00000001;

    /*
        TCCR4D Configuration
        Bit 1:0, WGM41/40 Waveform Generation Mode
        00 => Fast PWM Mode, TOP with OCR4C
     */
    TCCR4D = 0;


    // Counter Configuration
    /*
    15.3.1 Counter Initialization for Asynchronous Mode
    To change Timer/Counter4 to the asynchronous mode follow the procedure below:
    1. Enable PLL.
    2. Wait 100µs for PLL to stabilize.
    3. Poll the PLOCK bit until it is set.
    4. Configure the PLLTM1:0 bits in the PLLFRQ register to enable the asynchronous mode (different from
    0:0 value).
    */
    // Use 96MHz / 2 = 48MHz
    //PLLFRQ=(PLLFRQ&0xCF)|0x30;
    PPLFRQ = 
    // Terminal count for Timer 4 PWM
    OCR4C=255; //OCR4C holds the timer's TOP value
    DDRD|=1<<7; // Set output mode D7, 128

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
    OCR4D = pwm; // Set PWM
    //DDRD|=1<<7; // Set output mode D7, 128
    //TCCR4C|=0x09; // Activate channel d, bitwise OR






}
