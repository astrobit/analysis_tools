#include <stdio.h>

int main(void)
{
	double dX = 0.0;
	double dY = 1.0 / dX;

	printf("Y = %e\n",dY);
	if (dY < 0.0)
		printf("Y < 0\n");
	if (dY > 0.0)
		printf("Y > 0\n");
	if (dY == 0.0)
		printf("Y == 0\n");
}

