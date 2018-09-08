//
// I2C Accelerometer + Gyroscope access library
// Supports the following accels/gyros: LSM9DS1, ADXL345 and MPU6050
//
// Written by Larry Bank - 1/25/2018
// Copyright (c) 2018 BitBank Software, Inc.
// bitbank@pobox.com
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
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "accel.h"

static int file_i2c = -1;
static int iType;

//
// Opens a file system handle to the I2C device
// and initializes the sensor to begin sampling
//
// Returns -1 if error, 0 if success
//
int accelInit(int iChannel, int iAddr, int iAccelType)
{
int i, rc;
char filename[32];
unsigned char ucTemp[16];

	if (iAccelType != TYPE_LSM9DS1 && iAccelType != TYPE_ADXL345 && iAccelType != TYPE_MPU6050 && iAccelType != TYPE_BMI160)
		return -1;
 
	iType = iAccelType;

	sprintf(filename, "/dev/i2c-%d", iChannel);
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open the i2c bus; need root privilege?\n");
		file_i2c = 0;
		return -1;
	}

	if (ioctl(file_i2c, I2C_SLAVE, iAddr) < 0)
	{
		fprintf(stderr, "Failed to acquire bus access or talk to slave\n");
		close(file_i2c);
		file_i2c = -1;
		return -1;
	}

	if (iType == TYPE_LSM9DS1)
	{
                ucTemp[0] = 0x0F; // get ID
                rc = write(file_i2c, ucTemp, 1);
                i = read(file_i2c, ucTemp, 1);
                if (rc < 0 || i != 1 || ucTemp[0] != 0x68)
                {
                        printf("Error, ID doesn't match 0x68; wrong device?\n");
                        close(file_i2c);
                        file_i2c = -1;
                        return -1;
                }
		ucTemp[0] = 0x20; // accelerometer sample rate
		ucTemp[1] = 0x60; // 119hz accel
		write(file_i2c, ucTemp, 2);
		ucTemp[0] = 0x1e; // gyroscope enable
		ucTemp[1] = 0x38; // enable gyro on all axes
		write(file_i2c, ucTemp, 2);
		ucTemp[0] = 0x10; // gyro ctrl_reg1
		ucTemp[1] = 0x28; // data rate + full scale + bw selection
// bits:	ODR_G2 | ODR_G1 | ODR_G0 | FS_G1 | FS_G0 | 0 | BW_G1 | BW_G0
// 0x28 = 14.9hz, 500dps
		write(file_i2c, ucTemp, 2);
	}
	else if (iType == TYPE_ADXL345)
	{
		ucTemp[0] = 0x00; // get ID
		rc = write(file_i2c, ucTemp, 1);
		i = read(file_i2c, ucTemp, 1);
		if (rc < 0 || i != 1 || ucTemp[0] != 0xE5)
		{
			printf("Error, ID doesn't match 0xE5; wrong device?\n");
			close(file_i2c);
			file_i2c = -1;
			return -1;
		}
		ucTemp[0] = 0x2c; // bandwidth/rate mode
		ucTemp[1] = 0x06; // 6.125hz sampling (lowest power)
		rc = write(file_i2c, ucTemp, 2);
		ucTemp[0] = 0x2d; // power control
		ucTemp[1] = 0x08; // set simplest sampling mode (only measure bit)
		rc = write(file_i2c, ucTemp, 2);
		ucTemp[0] = 0x31; // data format
		ucTemp[1] = 0x00; // set +/-2g range and right justified mode
		rc = write(file_i2c, ucTemp, 2);
		if (rc != 2) {};
		}
	else if (iType == TYPE_BMI160)
	{
                ucTemp[0] = 0x00; // get CHIPID
                rc = write(file_i2c, ucTemp, 1);
                i = read(file_i2c, ucTemp, 1);
                if (rc < 0 || i != 1 || ucTemp[0] != 0xD1)
                {
                        printf("Error, ID doesn't match 0xD1; wrong device?\n");
                        close(file_i2c);
                        file_i2c = -1;
                        return -1;
                }
		ucTemp[0] = 0x7e; // send command
		ucTemp[1] = 0x11; // set accelerometer to normal mode
		rc = write(file_i2c, ucTemp, 2);
		usleep(4000); // give it 4ms to occur
		ucTemp[0] = 0x7e; // command
		ucTemp[1] = 0x15; // set gyroscope to normal power mode
		rc = write(file_i2c, ucTemp, 2);
		if (rc != 2) {};
	} // BMI160
	else // MPU6050
	{
		ucTemp[0] = 0x75; // get ID
		rc = write(file_i2c, ucTemp, 1);
		i = read(file_i2c, ucTemp, 1);
		if (rc < 0 || i != 1 || ucTemp[0] != 0x68)
		{
			printf("Error, ID doesn't match 0x68; wrong device?\n");
			printf("Value read = %02x\n", ucTemp[0]);
			close(file_i2c);
			file_i2c = -1;
			return -1;
		}
		// pwr mgmt 1 register
// bits: 7=reset, 6=sleep, 5=cycle, 4=n/a, 3=temp_disable, 2-0=clock select
		ucTemp[0] = 0x6b; // power management 1 register
		ucTemp[1] = 0x00; // disable sleep mode
		rc = write(file_i2c, ucTemp, 2);
		if (rc < 0) {};
	}
	return 0;

} /* accelInit() */

void accelShutdown(void)
{
	if (file_i2c >= 0)
	{
		close(file_i2c);
		file_i2c = -1;
	}
}
//
// Read the gyroscope register values
//
int accelReadGValues(int *X, int *Y, int *Z)
{
unsigned char ucTemp[8];
int i,rc, x, y, z;

	if (file_i2c < 0) // not initialized
		return -1;

	if (X == NULL && Y == NULL && Z == NULL)
		return -1; // nothing to do

	if (iType == TYPE_ADXL345)
		return -1; // ADXL345 doesn't have a gyroscope
	if (iType == TYPE_LSM9DS1)
		ucTemp[0] = 0x18;
	else if (iType == TYPE_BMI160)
		ucTemp[0] = 0xc;
	else // MPU6050
		ucTemp[0] = 0x43;
	rc = write(file_i2c, ucTemp, 1); // write address of register to read
	i = read(file_i2c, ucTemp, 6);
	if (rc < 0 || i != 6)
	{
		return -1; // something went wrong
	}
	if (iType == TYPE_LSM9DS1 || iType == TYPE_BMI160)
	{
		x = (ucTemp[1] << 8) + ucTemp[0];
		y = (ucTemp[3] << 8) + ucTemp[2];
		z = (ucTemp[5] << 8) + ucTemp[4];
	}
	else // MPU6050
	{
		x = (ucTemp[0] << 8) + ucTemp[1];
		y = (ucTemp[2] << 8) + ucTemp[3];
		z = (ucTemp[4] << 8) + ucTemp[5];
	}
	if (x > 32767) x -= 65536; // two's compliment
	if (y > 32767) y -= 65536;
	if (z > 32767) z -= 65536;
	if (X != NULL) *X = x;
	if (Y != NULL) *Y = y;
	if (Z != NULL) *Z = z;

	return 0;

} /* accelReadGValues() */

//
// Read the accelerator register values
//
int accelReadAValues(int *X, int *Y, int *Z)
{
unsigned char ucTemp[8];
int i,rc, x, y, z;

	if (file_i2c < 0) // not initialized
		return -1;

	if (X == NULL && Y == NULL && Z == NULL)
		return -1; // nothing to do

	if (iType == TYPE_LSM9DS1)
		ucTemp[0] = 0x28;
	else if (iType == TYPE_ADXL345)
		ucTemp[0] = 0x32; // start of axis data
	else if (iType == TYPE_BMI160)
		ucTemp[0] = 0x12;
	else // MPU6050
		ucTemp[0] = 0x3b;
	rc = write(file_i2c, ucTemp, 1); // write address of register to read
	i = read(file_i2c, ucTemp, 6);
	if (rc < 0 || i != 6)
	{
		return -1; // something went wrong
	}
	if (iType == TYPE_LSM9DS1 || iType == TYPE_ADXL345 || iType == TYPE_BMI160)
	{
		x = (ucTemp[1] << 8) + ucTemp[0];
		y = (ucTemp[3] << 8) + ucTemp[2];
		z = (ucTemp[5] << 8) + ucTemp[4];
	}
	else
	{
                x = (ucTemp[0] << 8) + ucTemp[1];
                y = (ucTemp[2] << 8) + ucTemp[3];
                z = (ucTemp[4] << 8) + ucTemp[5];
	}
	if (x > 32767) x -= 65536; // two's compliment
	if (y > 32767) y -= 65536;
	if (z > 32767) z -= 65536;
	if (X != NULL) *X = x;
	if (Y != NULL) *Y = y;
	if (Z != NULL) *Z = z;

	return 0;

} /* accelReadAValues() */
