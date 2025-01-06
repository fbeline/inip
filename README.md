# INIP
[.ini](https://en.wikipedia.org/wiki/INI_file) file parser written in C

The project implements the standard .ini file format. Plus some **Non-standard features:**

- inline comments with `;` or `#`
- key-value pairs without sections

## Usage

```c
struct inip ini = {0};
const char *ini_data = "[section]\nkey=value\n";

if (inip_parse(&inip, ini_data) != 0) {
	printf("inip_parse() failed\n");
	return 1;
}

// get value by section and key
const char *value = inip_get(&ini, "section", "key");

// set value (create if not exists)
if (inip_set(&ini, "section", "key", "new_value") != 0) {
    printf("inip_set() failed\n");
    return 1;
}

// stringify structure
char output[512];
inip_stringify(&ini, output);

inip_destroy(&ini);
```
