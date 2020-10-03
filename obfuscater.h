#ifndef OBFUSCATER_H_
#define OBFUSCATER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define WORD_SIZE 64

/** @brief file_t is used for working with obfuscater
*/
struct file_t
{
    FILE* file;

    char* content;
    unsigned int size_content;

    bool change_names_f;
    bool remove_spaces_f;
    bool remove_comments_f;
};

/** @brief content_t is used for working with the file's content
 */
struct content_t
{
    char** words;
    size_t count_words;
};

/**
 * There are main functions of an obfuscater
 */

void obfuscate_file(const char* input_file, const char* output_file);
void read_obfuscate_config(struct file_t* fl);

void change_names(struct content_t* content);
void remove_spaces(struct content_t* content);
void remove_comments(struct content_t* content);

/**
 * There are additional functions for clearly working with an ofuscater
 */

struct file_t* open_file(const char* filename);
void free_file(struct file_t* fl);
void create_file_with_content(const char* filename, const struct content_t* content);

struct content_t* create_content(void);

void read_file(struct file_t* fl);
void add_word(char* word, struct content_t* content);
void free_content(struct content_t* content);

struct content_t* devide_content_on_words(char* content, size_t size_content);

/*! @brief Remove words in the interval [start_pos; end_pos]
 */
void remove_words(struct content_t* content, size_t start_pos, size_t end_pos);
char* get_word(char* content, size_t start_pos, size_t end_pos);
void remove_spaces_in_word(char* word);

void replace_word(char* str, const char* old_word, const char* new_word);
void add_memory(struct content_t* content);

bool is_empty(char* str);
bool is_type(char* str, char* type);

struct content_t* get_variables(struct content_t* content);
struct content_t* get_types(struct content_t* content);

void print_content(const struct content_t* content, bool is_detail);

#endif // OBFUSCATER_H_
