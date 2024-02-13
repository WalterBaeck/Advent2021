#include <stdio.h>

int main()
{
	int i;
	long long num=1;
	for (i=0;i<64; i++)
	{
		printf("%2d: %20lld\n", i, num);
		num <<= 1;
	}

	return 0;
}
