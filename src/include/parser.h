
#ifndef PARSER_H
#define PARSER_H

typedef enum {
    TYPE_HEADER,
    TYPE_PARA
} RadownType;

typedef struct {
    RadownType type;
    char *value;
} RadownValue;

const char *file_to_char(const char *file_path);
char *load_file(const char *file_path); // TODO: Perhaps somehow merge this function with file_to_string() in main.c?

char *read_header(const char **text);
char *read_para(const char **text);
void jump_space(const char **text);

RadownValue *parse(const char **text);
char *lexer(const char *tmp);


#endif//PARSER_H
