#include "obfuscater.h"

/**
 * There are main functions of an obfuscater
 */

void obfuscate_file(const char* input_file, const char* output_file)
{
    struct file_t* fl = open_file(input_file);

    read_obfuscate_config(fl);
    read_file(fl);

    struct content_t* content = devide_content_on_words(fl->content, fl->size_content);

    if (!content) return;

    if (fl->remove_comments_f)
        remove_comments(content);

    if (fl->remove_spaces_f)
        remove_spaces(content);

    if (fl->change_names_f)
        change_names(content);

    create_file_with_content("test_ob_1.c", content);
    free_content(content);
    free_file(fl);
}

void read_obfuscate_config(struct file_t* fl)
{
    FILE* file_config = fopen("ob_config.txt", "r");

    if (!file_config) {
        fl->change_names_f = true;
        fl->remove_comments_f = true;
        fl->remove_spaces_f = true;
    } else {
        const size_t MAX_STR = 11;
        char str[MAX_STR];

        while (fgets(str, MAX_STR, file_config) != NULL) {
            if (strstr(str, "names=") != NULL)
                fl->change_names_f = strstr(str, "0") ? true : false;
            else if (strstr(str, "spaces=") != NULL)
                fl->remove_spaces_f = strstr(str, "0") ? true : false;
            else if (strstr(str, "comments=") != NULL)
                fl->remove_comments_f = strstr(str, "0") ? true : false;
        }

        fclose(file_config);
    }
}

void change_names(struct content_t* content)
{
    struct content_t* variables = get_variables(content);

    for (size_t i = 0; i < variables->count_words; i++) {
        char* name_var = (char*) calloc(10, 10 * sizeof(char));
        sprintf(name_var, "temp_%zu", i);

        for (size_t j = 0; j < content->count_words; j++)
            replace_word(content->words[j], variables->words[i], name_var);

        free(name_var);
    }

    free_content(variables);
}

void remove_spaces(struct content_t* content)
{
    for (size_t i = 0; i < content->count_words; i++) {
        if (strstr(content->words[i], "#include")) {
            i++;
            continue;
        }

        remove_spaces_in_word(content->words[i]);
    }
}

void remove_comments(struct content_t* content)
{
    char* end_comments[] = { "*/", "\n" };

    bool found_comment = false;
    bool is_long_comment = false;
    bool is_repeat = true;

    size_t start_pos = 0;
    size_t end_pos = 0;

    while (is_repeat) {
        for (size_t i = 0; i < content->count_words; i++) {
            if (!found_comment) {
                if (strstr(content->words[i], "/*") != NULL) {
                    start_pos = i;
                    found_comment = true;
                    is_long_comment = true;
                } else if (strstr(content->words[i], "//") != NULL) {
                    start_pos = i;
                    found_comment = true;
                    is_long_comment = false;
                }

                is_repeat = false;
            }

            if (found_comment) {
                if (content->words[i][0] != '\n') {
                    if (strstr(content->words[i],
                               is_long_comment ? end_comments[0] : end_comments[1]) != NULL) {
                        end_pos = i;
                        found_comment = false;
                        is_repeat = true;

                        remove_words(content, start_pos, end_pos);
                        break;
                    }
                }
            }
        }
    }

    if (found_comment) {
        end_pos = content->count_words - 1;
        remove_words(content, start_pos, end_pos);
    }
}

/**
 * There are additional functions for clearly working with an ofuscater
 */

struct file_t* open_file(const char* filename)
{
    struct file_t* fl = (struct file_t*) malloc(sizeof(struct file_t));
    fl->file = fopen(filename, "r");

    if (!fl->file) {
        free(fl);
        return NULL;
    }

    fl->size_content = 0;
    fl->content = NULL;

    return fl;
}

void free_file(struct file_t* fl)
{
    if (fl) {
        if (fl->content)
            free(fl->content);
        free(fl);
    }
}

void create_file_with_content(const char* filename,
                              const struct content_t* content)
{
    FILE* output = fopen(filename, "w");

    if (output) {
        for (size_t i = 0; i < content->count_words; i++)
            fprintf(output, "%s ", content->words[i]);

        fclose(output);
    }
}

struct content_t* create_content()
{
    struct content_t* content = (struct content_t*) malloc(sizeof(struct content_t));

    content->count_words = 0;
    content->words = (char**) calloc(0, 0 * sizeof(char*));

    return content;
}

void read_file(struct file_t* fl)
{
    fseek(fl->file, 0, SEEK_END);
    fl->size_content = ftell(fl->file);

    rewind(fl->file);

    fl->content = (char*) calloc(fl->size_content, fl->size_content * sizeof(char));
    fread(fl->content, 1, fl->size_content, fl->file);

    fclose(fl->file);
}

void add_word(char* word, struct content_t* content)
{
    add_memory(content);
    content->words[content->count_words - 1] = word;
}

void free_content(struct content_t* content)
{
    if (content) {
        for (size_t i = 0; i < content->count_words; i++)
            if (content->words[i]) free(content->words[i]);
        free(content->words);
        free(content);
    }
}

struct content_t* devide_content_on_words(char* content, size_t size_content)
{
    struct content_t* r_content = create_content();

    size_t i = 0;

    size_t start_pos = 0;
    size_t end_pos = 0;

    bool is_end_word = false;

    while (i < size_content) {
        if (!is_end_word) {
            if (content[i] == ' ' || content[i] == '\n') {
                end_pos = i;
                is_end_word = true;
            }
        }

        if (is_end_word) {
            char* word = get_word(content, start_pos, end_pos);

            if (!is_empty(word))
                add_word(word, r_content);
            else
                free(word);

            start_pos = end_pos;
            is_end_word = false;
        }

        i++;
    }

    if (!is_end_word) {
        end_pos = size_content - 1;
        char* word = get_word(content, start_pos, end_pos);

        if (!is_empty(word))
            add_word(word, r_content);
        else
            free(word);
    }

    return r_content;
}

void remove_words(struct content_t* content, size_t start_pos, size_t end_pos)
{
    size_t new_size = content->count_words - (end_pos - start_pos) - 1;
    size_t _i = 0;

    // char** save_words = (char**) malloc(new_size * sizeof(char*));
    char** realloc_words = (char**) realloc(content->words, new_size * sizeof(*content->words));

    if (realloc_words) {
        for (size_t i = end_pos; i >= start_pos; i--)
            for (size_t j = i; j < content->count_words - (end_pos - i) - 1; j++)
                strcpy(realloc_words[j], content->words[j + 1]);

        content->count_words = new_size;
        content->words = realloc_words;
    }
}

char* get_word(char* content, size_t start_pos, size_t end_pos)
{
    char* word = (char*) calloc(WORD_SIZE, WORD_SIZE * sizeof(char));
    size_t _i = 0;
    for (size_t i = start_pos; i <= end_pos; i++, _i++)
        word[_i] = content[i];

    return word;
}

void remove_spaces_in_word(char* word)
{
    replace_word(word, " ", "");
    replace_word(word, "\n", "");
    replace_word(word, "\t", "");
}

void replace_word(char* str, const char* old_word, const char* new_word)
{
    char* pos;
    char* temp = (char*) malloc((strlen(str) + 1) * sizeof(char));

    int index = 0;
    int owlen;

    owlen = strlen(old_word);

    /*
     * Repeat till all occurrences are replaced.
     */
    while ((pos = strstr(str, old_word)) != NULL) {
        // Bakup current line
        strcpy(temp, str);

        // Index of current found word
        index = pos - str;

        // Terminate str after word found index
        str[index] = '\0';

        // Concatenate str with new word
        strcat(str, new_word);

        // Concatenate str with remaining words after
        // oldword found index.
        strcat(str, temp + index + owlen);
    }

    free(temp);
}

void add_memory(struct content_t* content)
{
    char** new_words = (char**) realloc(content->words,
                                        (content->count_words + 1) * sizeof(*content->words));
    if (new_words) {
        content->words = new_words;
        content->count_words++;
    }
}

bool is_empty(char* str)
{
    for (size_t i = 0; i < strlen(str); i++)
        if (!isspace(str[i]) && str[i] != '\n') return false;
    return true;
}

bool is_type(char* str, char* type)
{
    if (strstr(str, type) != NULL) {
        size_t _i = 0;
        for (size_t i = 0; i < strlen(str); i++) {
            if (!ispunct(str[i])) {
                if (str[i] != type[_i]) return false;
                _i++;
            }
        }
    } else
        return false;
    return true;
}

struct content_t* get_variables(struct content_t* content)
{
    struct content_t* variables = create_content();
    struct content_t* types = get_types(content);

    for (size_t i = 0; i < content->count_words; i++) {
        for (size_t j = 0; j < types->count_words; j++) {
            if (is_type(content->words[i], types->words[j])) {
                if (strcmp(content->words[i + 1], "main") != 0) {
                    char* word = (char*) calloc(WORD_SIZE, WORD_SIZE * sizeof(char));
                    strcpy(word, content->words[i + 1]);
                    add_word(word, variables);
                }
                break;
            }
        }
    }

    free_content(types);

    return variables;
}

struct content_t* get_types(struct content_t* content)
{
    char* default_types[5] = { "int", "void", "unsigned", "short", "long" };

    struct content_t* types = create_content();

    for (size_t i = 0; i < content->count_words; i++) {
        if (strstr(content->words[i], "typedef") != NULL)
            add_word(content->words[i + 2], types);
        else if (strstr(content->words[i], "struct") != NULL)
            add_word(content->words[i + 1], types);
    }

    for (size_t i = 0; i < 5; i++) {
        char* word = (char*) calloc(strlen(default_types[i]),
                                    strlen(default_types[i]) * sizeof(char));
        strncpy(word, default_types[i], strlen(default_types[i]));
        add_word(word, types);
    }

    return types;
}

void print_content(const struct content_t* content, bool is_detail)
{
    for (size_t i = 0; i < content->count_words; i++)
        if (is_detail)
            printf("%zu: %s\n", i, content->words[i]);
        else
            printf("%s ", content->words[i]);
}
