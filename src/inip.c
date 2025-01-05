#include "inip.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

enum token {
	TOKEN_STRING,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_EQUAL,
	TOKEN_EOF
};

struct ini_token {
	enum token type;
	const char *start;
	size_t length;
};

static void str_trim(char *str)
{
	char *end;

	// str_trim leading space
	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0) // All spaces?
		return;

	// str_trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;

	// Write new null terminator character
	end[1] = '\0';
}

static int inip_tokenize(const char *buffer, struct ini_token *tokens)
{
	const char *start;
	const char *current;

	current = buffer;
	while (*current != '\0') {
		start = current;
		if (*current == '[') {
			tokens->type = TOKEN_LBRACKET;
			tokens->start = start;
			tokens->length = 1;
			tokens++;
		} else if (*current == ']') {
			tokens->type = TOKEN_RBRACKET;
			tokens->start = start;
			tokens->length = 1;
			tokens++;
		} else if (*current == '=') {
			tokens->type = TOKEN_EQUAL;
			tokens->start = start;
			tokens->length = 1;
			tokens++;
		} else if (*current == ' ' || *current == '\t' ||
			   *current == '\n') {
			// skip whitespace
		} else if (*current == ';') {
			while (*current != '\n' && *current != '\0') {
				current++;
			}
		} else {
			while (*current != '=' && *current != ']' &&
			       *current != '\t' && *current != '\n' &&
			       *current != '\0') {
				current++;
			}
			tokens->type = TOKEN_STRING;
			tokens->start = start;
			tokens->length = current - start;
			tokens++;
			continue;
		}

		current++;
	}

	tokens->type = TOKEN_EOF;
	tokens->start = current;
	tokens->length = 1;
	tokens++;

	return 0;
}

static int inip_build(struct inip *ini, struct ini_token *tokens)
{
	ini->sections = NULL;
	struct inip_section *current_section = NULL;
	struct inip_key *current_key = NULL;

	while (tokens->type != TOKEN_EOF) {
		if (tokens->type == TOKEN_LBRACKET) {
			if ((++tokens)->type == TOKEN_STRING) {
				struct inip_section *new_section =
					malloc(sizeof(struct inip_section));
				new_section->keys = NULL;
				new_section->next = NULL;
				strncpy(new_section->name, tokens->start,
					tokens->length);
				new_section->name[tokens->length] = '\0';
				str_trim(new_section->name);

				if (ini->sections == NULL) {
					ini->sections = new_section;
				} else {
					current_section->next = new_section;
				}
				current_section = new_section;
			} else {
				printf("Error: Expected a section name after '['\n");
				return 1;
			}
			if ((++tokens)->type != TOKEN_RBRACKET) {
				printf("Error: Expected ']' after section name: %s\n",
				       current_section->name);
				return 1;
			}
		} else if (tokens->type == TOKEN_STRING) {
			struct inip_key *new_key =
				malloc(sizeof(struct inip_key));
			strncpy(new_key->name, tokens->start, tokens->length);
			new_key->name[tokens->length] = '\0';
			str_trim(new_key->name);
			new_key->next = NULL;
			if ((++tokens)->type == TOKEN_EQUAL) {
				if ((++tokens)->type == TOKEN_STRING) {
					strncpy(new_key->value, tokens->start,
						tokens->length);
					new_key->value[tokens->length] = '\0';
					str_trim(new_key->value);
					if (current_section == NULL) {
						printf("Error: Key %s has no section\n",
						       new_key->name);
						return 1;
					}
					if (current_section->keys == NULL) {
						current_section->keys = new_key;
					} else {
						current_key->next = new_key;
					}
					current_key = new_key;
				} else {
					printf("Error: Expected a value after '='\n");
					return 1;
				}
			} else {
				printf("Error: Expected '=' after key name\n");
				return 1;
			}
		}
		tokens++;
	}

	return 0;
}

int inip_parse(struct inip *ini, const char *buffer)
{
	if (ini == NULL || buffer == NULL) {
		return 1;
	}

	struct ini_token tokens[1024];
	if (inip_tokenize(buffer, tokens) != 0) {
		return 1;
	}

	return inip_build(ini, tokens);
}

const char *inip_get(struct inip *inip, const char *section, const char *key)
{
	struct inip_section *s = inip->sections;
	while (s != NULL) {
		if (strcmp(s->name, section) == 0) {
			struct inip_key *k = s->keys;
			while (k != NULL) {
				if (strcmp(k->name, key) == 0) {
					return k->value;
				}
				k = k->next;
			}
		}
		s = s->next;
	}
	return NULL;
}

void inip_destroy(struct inip *inip)
{
	if (inip == NULL) {
		return;
	}

	struct inip_section *section = inip->sections;
	while (section != NULL) {
		struct inip_key *key = section->keys;
		while (key != NULL) {
			struct inip_key *next = key->next;
			free(key);
			key = next;
		}
		struct inip_section *next = section->next;
		free(section);
		section = next;
	}
}