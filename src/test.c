#include "inip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_inip_parse(void)
{
	const char *ini_data = "[section1]\n"
			       "key1 = value 1\n"
			       "key2 = value 2\n"
			       "; foo bar baz \n"
			       "[ section2  ]\n"
			       "key A=valueA\n"
			       "key B=valueB\n";

	struct inip inip = {0};

	if (inip_parse(&inip, ini_data) != 0) {
		printf("inip_parse() failed\n");
		return 1;
	}

	const char *value = inip_get(&inip, "section1", "key1");
	if (value == NULL) {
		printf("inip_get() returned NULL\n");
		return 1;
	}
	if (strcmp(value, "value 1") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	value = inip_get(&inip, "section2", "key B");
	if (value == NULL) {
		printf("inip_get() returned NULL\n");
		return 1;
	}
	if (strcmp(value, "valueB") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int main(void)
{
	int result = 0;
	result += test_inip_parse();

	if (result == 0) {
		printf("All tests passed\n");
	} else {
		printf("Some tests failed\n");
	}

	return 0;
}
