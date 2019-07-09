# ActiveCooler

Simple script to actuate cooling fans based on temperature sensor readout

## Hardware
Device: SparkFun Pro Micro 5V/16MHz
Temperature Sensor: DS18B20
Fan: SanAce 92W XF-P05801, DC 24V/1.0A

## Libraries used:
https://github.com/milesburton/Arduino-Temperature-Control-Library
https://github.com/PaulStoffregen/OneWire

## Notes
1. Make sure your Arduino IDE is setup for uploading code to the SparkFun Pro Micro Board:
..* https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/all
..* https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro16MHzv1.pdf
2. The code simply uses the linear mapping function map() to map temperatures to motor speeds
..* Motor speeds range from 0-100% duty cycle (Listed as 0-255)
..* Exposed parameters are defined at the top of the code


