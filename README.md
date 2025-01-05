# INIP
INIP is a simple .ini file parser written in C with no external dependencies. It is designed to be simple and easy to use.

## Usage

```c
struct inip ini = {0};
const char *ini_data = "[section]\nkey=value\n";

if (inip_parse(&inip, ini_data) != 0) {
	printf("inip_parse() failed\n");
	return 1;
}

const char *value = inip_get(&ini, "section", "key");

inip_destroy(&ini);
```
