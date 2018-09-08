//
// Accelerometer sample app
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <accel.h>

int main(int argc, char *argv[])
{
int i;
int X, Y, Z, GX, GY, GZ;


//	i = accelInit(1, 0x53, TYPE_ADXL345);
//	i = accelInit(1, 0x68, TYPE_MPU6050);
//	i = accelInit(1, 0x6a, TYPE_LSM9DS1);
	i = accelInit(1, 0x69, TYPE_BMI160);
	if (i != 0)
	{
		return -1; // problem - quit
	}

	printf("Accelerometer device successfully opened.\n");
	usleep(1000000); // wait for data to settle for first read

	for (i=0; i<12000; i++) // read values twice a second for 1 minute
	{
		accelReadAValues(&X, &Y, &Z);
		accelReadGValues(&GX, &GY, &GZ);
		printf("G:%06d,%06d,%06d  A:%06d,%06d,%06d\n", GX,GY,GZ,X,Y,Z);
	//	usleep(100000);
	}
	accelShutdown();

return 0;
} /* main() */
