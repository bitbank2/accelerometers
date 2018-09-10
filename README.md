## Accelerometers

A simple C library to iniitalize and read the data from 4 popular
accelerometer chips (ADXL345, MPU6050, BMI160 and LSM9DS1). This library uses the I2C
kernel driver to access the sensors (/dev/i2c*) and therefore needs to run with
root privilege. A sample program is provided to demonstrate its use.<br>
<br>
The I2C addresses can vary depending on the way each chip is wired. The demo
program uses the common addresses used by breakout boards. <br>
<br>
To build the C library, simply type 'make' in the terminal. To build the sample
app, type 'make -f make_sample'. This will build the 'test' executable to show
that the library is working.<br>
<br>
Copyright (c) 2018 by Larry Bank<br>
Project started 1/25/2018<br>
bitbank@pobox.com<br>

