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
int X, Y, Z;


	i = accelInit(1, 0x53, TYPE_ADXL345);
//	i = accelInit(1, 0x69, TYPE_MPU6050);
//	i = accelInit(1, 0x6a, TYPE_LSM9DS1);
	if (i != 0)
	{
		return -1; // problem - quit
	}

	printf("Accelerometer device successfully opened.\n");
	usleep(1000000); // wait for data to settle for first read

	for (i=0; i<120; i++) // read values twice a second for 1 minute
	{
		accelReadAValues(&X, &Y, &Z);
		printf("Accel X/Y/Z = %d,%d,%d\n", X, Y, Z);
		accelReadGValues(&X, &Y, &Z);
		printf("Gyro X/Y/Z = %d,%d,%d\n", X, Y, Z);
		usleep(500000);
	}
	accelShutdown();

return 0;
} /* main() */
