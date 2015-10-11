#include <stdio.h>
#include <string.h>

#include "MySQLCXX.h"
#include "Test.h"

#define kMaxTests 5

int main(int argc, char* argv[])
{
	unsigned int Count = 0;
	Connection Con;
	Con.Connect("localhost", "root", "root", NULL, 3306);

	if (Con.IsConnected())
	{
		Count++;
		if (!Test1_Preperation(&Con))
		{
			printf("[%d/%d] Test 1: Preperation - FAILED\n", Count, kMaxTests);
			return 1;
		}

		printf("[%d/%d] Test 1: Preperation - SUCCESS\n", Count, kMaxTests);


		Count++;
		if (!Test2_QueryWithoutOutput(&Con))
		{
			printf("[%d/%d] Test 2: Query Without Output - FAILED\n", Count, kMaxTests);
			return 1;
		}

		printf("[%d/%d] Test 2: Query Without Output - SUCCESS\n", Count, kMaxTests);


		Count++;
		if (!Test3_QueryWithOutput(&Con))
		{
			printf("[%d/%d] Test 3: Query With Output - FAILED\n", Count, kMaxTests);
			return 1;
		}

		printf("[%d/%d] Test 3: Query With Output - SUCCESS\n", Count, kMaxTests);


		Count++;
		if (!Test4_PreparedStatement(&Con))
		{
			printf("[%d/%d] Test 4: Prepared Statement - FAILED\n", Count, kMaxTests);
			return 1;
		}

		printf("[%d/%d] Test 3: Prepared Statement - SUCCESS\n", Count, kMaxTests);


		Count++;
		if (!Test5_Copy(&Con))
		{
			printf("[%d/%d] Test 5: Copy- FAILED\n", Count, kMaxTests);
			return 1;
		}

		printf("[%d/%d] Test 5: Copy - SUCCESS\n", Count, kMaxTests);

	}

	return 0;
} 