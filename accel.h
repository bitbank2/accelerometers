
#ifndef __ACCEL__
#define __ACCEL__
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

//
// Open the I2C channel and initialize the sensor to begin sampling
//
// Returns 0 for success, -1 for failure
//
int accelInit(int iChannel, int iAddr, int iType);
//
// Closes the I2C channel
//
void accelShutdown(void);
//
// Read the current accelerometer values
//
int accelReadAValues(int *X, int *Y, int *Z);
//
// Read the current gyroscope values
//
int accelReadGValues(int *X, int *Y, int *Z);

//
// Currently supported devices
//
enum {
      TYPE_ADXL345 = 1,
      TYPE_MPU6050,
      TYPE_LSM9DS1,
      TYPE_BMI160,
      TYPE_LIS3DH,
      TYPE_COUNT
};

#endif // __ACCEL__
