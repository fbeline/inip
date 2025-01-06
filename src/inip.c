#include "inip.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define NTOKEN 256

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

static int is_inline_comment(const char *current)
{
	return *current == ' ' &&
	       (*(current + 1) == ';' || *(current + 1) == '#');
}

static struct ini_token *inip_tokenize(const char *buffer)
{
	struct ini_token *tokens;
	size_t capacity = NTOKEN;
	size_t idx = 0;

	const char *start;
	const char *current;

	if ((tokens = malloc(capacity * sizeof(struct ini_token))) == NULL) {
		return NULL;
	}

	current = buffer;
	while (*current != '\0') {
		start = current;

		if (idx >= capacity) {
			capacity *= 2;
			if ((tokens = realloc(
				     tokens,
				     capacity * sizeof(struct ini_token))) ==
			    NULL) {
				return NULL;
			}
		}

		if (*current == '[') {
			tokens[idx].type = TOKEN_LBRACKET;
			tokens[idx].start = start;
			tokens[idx].length = 1;
			idx++;
		} else if (*current == ']') {
			tokens[idx].type = TOKEN_RBRACKET;
			tokens[idx].start = start;
			tokens[idx].length = 1;
			idx++;
		} else if (*current == '=') {
			tokens[idx].type = TOKEN_EQUAL;
			tokens[idx].start = start;
			tokens[idx].length = 1;
			idx++;
		} else if (*current == ' ' || *current == '\t' ||
			   *current == '\n') {
			// skip whitespace
		} else if (*current == ';' || *current == '#') {
			while (*current != '\n' && *current != '\0') {
				current++;
			}
		} else {
			while (*current != '=' && *current != ']' &&
			       *current != '\t' && *current != '\n' &&
			       *current != '\0' &&
			       !is_inline_comment(current)) {
				current++;
			}
			tokens[idx].type = TOKEN_STRING;
			tokens[idx].start = start;
			tokens[idx].length = current - start;
			idx++;
			continue;
		}

		current++;
	}

	tokens[idx].type = TOKEN_EOF;

	return tokens;
}

static struct inip_section *create_section(struct ini_token *token)
{
	struct inip_section *section = malloc(sizeof(struct inip_section));
	if (section == NULL) {
		return NULL;
	}

	section->keys = NULL;
	section->next = NULL;

	strncpy(section->name, token->start, token->length);
	section->name[token->length] = '\0';
	str_trim(section->name);

	return section;
}

static struct inip_key *create_key(struct ini_token *token)
{
	struct inip_key *key = malloc(sizeof(struct inip_key));
	if (key == NULL) {
		return NULL;
	}

	key->next = NULL;

	strncpy(key->name, token->start, token->length);
	key->name[token->length] = '\0';
	str_trim(key->name);

	return key;
}

static void inip_key_set(struct inip_key *key, struct ini_token *token)
{
	strncpy(key->value, token->start, token->length);
	key->value[token->length] = '\0';
	str_trim(key->value);
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
					create_section(tokens);
				if (new_section == NULL)
					return 1;

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
			struct inip_key *new_key = create_key(tokens);

			if ((++tokens)->type == TOKEN_EQUAL) {
				if ((++tokens)->type == TOKEN_STRING) {
					inip_key_set(new_key, tokens);

					if (current_section == NULL) {
						free(new_key);
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
					free(new_key);
					printf("Error: Expected a value after '='\n");
					return 1;
				}
			} else {
				free(new_key);
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
	struct ini_token *tokens;
	if (ini == NULL || buffer == NULL) {
		return 1;
	}

	if ((tokens = inip_tokenize(buffer)) == NULL) {
		return 1;
	}

	int result = inip_build(ini, tokens);
	free(tokens);

	return result;
}

int inip_stringify(struct inip *inip, char *buffer)
{
	if (inip == NULL || buffer == NULL) {
		return 1;
	}
	buffer[0] = '\0';

	struct inip_section *s = inip->sections;
	while (s != NULL) {
		strcat(buffer, "[");
		strcat(buffer, s->name);
		strcat(buffer, "]\n");
		struct inip_key *k = s->keys;
		while (k != NULL) {
			strcat(buffer, k->name);
			strcat(buffer, " = ");
			strcat(buffer, k->value);
			strcat(buffer, "\n");
			k = k->next;
		}
		s = s->next;
	}

	return 0;
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
