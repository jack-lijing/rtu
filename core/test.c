#include <stdio.h>
#include <time.h>

int main()
{
	time_t t;
	struct tm *p;
	time(&t);
	p = gmtime(&t);
	printf("Year:%d\n",1900+p->tm_year);
	printf("Day:%d\n",p->tm_mday);
	printf("Hour:%d\n",p->tm_hour);
	printf("Hour:%d\n",p->tm_hour);
	printf("Min:%d\n",p->tm_min);
	printf("Sec:%d\n",p->tm_sec);
	return 0;
}
