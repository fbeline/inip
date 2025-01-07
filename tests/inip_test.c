#include "../include/inip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_parse(void)
{
	const char *ini_data = "[section1]\n"
			       "key1 = value 1\n"
			       "key2 = value 2\n"
			       "; foo bar baz \n"
			       "[ section2  ]\n"
			       "key A=valueA\n"
			       "key B=valueB\n";

	struct inip inip = { 0 };

	if (inip_parse(&inip, ini_data) != 0) {
		printf("inip_parse() failed\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int test_stringify(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[ section1  ]\n"
			       "# some comment\n"
			       "key1=value 1 \n"
			       "key2=value 2\n";

	const char *ini_result = "[section1]\n"
				 "key1 = value 1\n"
				 "key2 = value 2\n";

	if (inip_parse(&inip, ini_data) != 0) {
		printf("inip_parse() failed\n");
		return 1;
	}

	char output[512];
	if (inip_stringify(&inip, output) != 0) {
		printf("inip_stringify() failed\n");
		return 1;
	}

	if (strcmp(output, ini_result) != 0) {
		printf("inip_stringify() returned wrong data\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int test_get(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[ section1  ]\n"
			       "key1=value 1\n"
			       "key2=value 2\n";

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

	value = inip_get(&inip, "section1", "key2");
	if (value == NULL) {
		printf("inip_get() returned NULL\n");
		return 1;
	}
	if (strcmp(value, "value 2") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int test_inline_comment(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[ section1  ]\n"
			       "key1=value 1 ; comment 1\n"
			       "key2=value 2 # comment 2\n";

	if (inip_parse(&inip, ini_data) != 0) {
		printf("inline comment failed: inip_parse() failed\n");
		return 1;
	}

	const char *value = inip_get(&inip, "section1", "key1");
	if (value == NULL) {
		printf("inline comment failed: inip_get() returned NULL\n");
		return 1;
	}
	if (strcmp(value, "value 1") != 0) {
		printf("inline comment failed: inip_get() returned wrong value\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int test_no_section_file(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "key1 = value 1\n"
			       "key2 = value 2\n"
			       "[section1]\n"
			       "key3 = value 3\n";

	if (inip_parse(&inip, ini_data) != 0) {
		printf("no section failed: inip_parse() failed\n");
		return 1;
	}

	const char *value = inip_get(&inip, "", "key1");
	if (strcmp(value, "value 1") != 0) {
		printf("no section failed: inip_get() returned wrong value\n");
		return 1;
	}

	char output[512];
	if (inip_stringify(&inip, output) != 0) {
		printf("no section failed: inip_stringify() failed\n");
		return 1;
	}

	if (strcmp(output, ini_data) != 0) {
		printf("%s", output);
		printf("no section failed: inip_stringify() returned wrong data\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int test_key_whitout_value(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[section1]\n"
			       "key1=\n"
			       "key2=value 2\n";

	if (inip_parse(&inip, ini_data) != 2) {
		printf("key whitout value failed: inip_parse() should have failed\n");
		return 1;
	}

	return 0;
}

int test_key_without_name(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[section1]\n"
			       "=value 1\n"
			       "key2=value 2\n";

	if (inip_parse(&inip, ini_data) != 2) {
		printf("key without name failed: inip_parse() should have failed\n");
		return 1;
	}

	return 0;
}

int test_section_without_name(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[]\n"
			       "key1=value 1\n"
			       "key2=value 2\n";

	if (inip_parse(&inip, ini_data) != 2) {
		printf("section without name failed: inip_parse() should have failed\n");
		return 1;
	}

	return 0;
}

int test_wrong_section_definition(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[section1\n"
			       "key1=value 1\n"
			       "key2=value 2\n";

	if (inip_parse(&inip, ini_data) != 2) {
		printf("wrong section definition failed: inip_parse() should have failed\n");
		return 1;
	}

	const char *ini_data2 = "section1]\n"
				"key1=value 1\n"
				"key2=value 2\n";

	if (inip_parse(&inip, ini_data2) != 2) {
		printf("wrong section definition failed: inip_parse() should have failed\n");
		return 1;
	}

	const char *ini_data3 = "section1\n"
				"key1=value 1\n"
				"key2=value 2\n";

	if (inip_parse(&inip, ini_data3) != 2) {
		printf("wrong section definition failed: inip_parse() should have failed\n");
		return 1;
	}

	return 0;
}

int test_set_key(void)
{
	struct inip inip = { 0 };
	const char *ini_data = "[section1]\n"
			       "key1=value 1\n"
			       "key2=value 2\n";

	if (inip_parse(&inip, ini_data) != 0) {
		printf("inip_parse() failed\n");
		return 1;
	}

	inip_set(&inip, "section1", "key1", "new value");

	// update key
	const char *value = inip_get(&inip, "section1", "key1");
	if (strcmp(value, "new value") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	// create key
	inip_set(&inip, "section1", "key3", "Value 3");
	const char *key3_value = inip_get(&inip, "section1", "key3");
	if (strcmp(key3_value, "Value 3") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	// create section & key
	inip_set(&inip, "new_section", "a", "value a");
	const char *new_key_value = inip_get(&inip, "new_section", "a");
	if (strcmp(new_key_value, "value a") != 0) {
		printf("inip_get() returned wrong value\n");
		return 1;
	}

	inip_destroy(&inip);

	return 0;
}

int main(void)
{
	int result = 0;
	result += test_parse();
	result += test_get();
	result += test_inline_comment();
	result += test_stringify();
	result += test_no_section_file();
	result += test_key_whitout_value();
	result += test_section_without_name();
	result += test_wrong_section_definition();
	result += test_key_without_name();
	result += test_set_key();

	if (result == 0) {
		printf("All tests passed\n");
	} else {
		printf("Some tests failed\n");
	}

	return 0;
}
