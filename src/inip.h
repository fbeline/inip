/*
 * inip.h
 *
 * This header file defines structures and functions for parsing and handling INI files.
 *
 * The INI file format is a simple, informal standard for configuration files for software
 * applications. INI files are simple text files with a basic structure composed of sections,
 * properties, and values.
 *
 *
 * Author: Felipe Beline Baravieira
 * Date: 2025-01-05
 * License: MIT
 */

#ifndef INIP_H
#define INIP_H

#define INI_MAX_SIZE 255

struct inip_key {
	char name[INI_MAX_SIZE];
	char value[INI_MAX_SIZE];
	struct inip_key *next;
};

struct inip_section {
	char name[INI_MAX_SIZE];
	struct inip_key *keys;
	struct inip_section *next;
};

struct inip {
	struct inip_section *sections;
};

/**
 * Parses the given ini buffer and populates the provided inip structure.
 *
 * @param ini A pointer to the inip structure to be populated.
 * @param buffer A string containing the INI data to be parsed.
 * @return An integer indicating the success or failure of the parsing operation. 0 on success, 1 generic failure, 2 parsing failure.
 */
int inip_parse(struct inip *ini, const char *buffer);

/**
 * Retrieves the value associated with the given section and key from the ini file.
 *
 * @param inip A pointer to the inip structure.
 * @param section The name of the section to search for.
 * @param key The name of the key to search for within the section.
 * @return A string containing the value associated with the specified section and key, or NULL if not found.
 */
const char *inip_get(struct inip *inip, const char *section, const char *key);

/**
 * Sets the value associated with the given section and key in the ini file. If the section or key does not exist, they will be created.
 *
 * @param inip A pointer to the inip structure.
 * @param section The name of the section to set the value in.
 * @param key The name of the key to set the value for within the section.
 * @param value The value to set for the specified section and key.
 * @return An integer indicating the success or failure of the set operation. 0 on success, 1 on failure.
 */
int inip_set(struct inip *inip, const char *section, const char *key, const char *value);

/**
 * Converts the inip structure back into an INI formatted string.
 *
 * @param inip A pointer to the inip structure to be converted.
 * @param buffer A string buffer to hold the resulting INI formatted string.
 * @return An integer indicating the success or failure of the conversion operation. 0 on success, 1 on failure.
 */
int inip_stringify(struct inip *inip, char *buffer);

/**
 * Frees all memory associated with the provided inip structure.
 *
 * @param inip A pointer to the inip structure to be destroyed.
 */
void inip_destroy(struct inip *inip);

#endif
