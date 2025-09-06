
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "./include/parser.h"
#include "./utils/str.h"
#include "./utils/os.h"


static char *read_string(const char **text, const char *tag)
{
    const char *start = *text;
    // while (**text != '[' && **text != '\0') {
    while (strncmp(*text, tag, strlen(tag)) && **text != '\0') {
	(*text)++;
    }
    size_t   length = (size_t)(*text - start);
    char *result = (char*)malloc(length + 1);
    strncpy(result, start, length);
    result[length] = '\0';
    return result;
}

static char *r(const char **text, const char *tag) {
    *text += strlen(tag) - 1;
    
    char *result = read_string(text, tag);
    if (strncmp(*text, tag, (strlen(tag) - 1)) == 0) {
	(*text) += strlen(tag);
    }
    return result;
}

char *read_content(const char **text)
{
    if (**text != '[') return NULL;
    (*text)++;
    
    // not the better code ever... actually, very far from.
    if (strncmp(*text, "header]",  strlen("header]")) == 0) return r(text, "[header]");
    else if (strncmp(*text, "para]", strlen("para]")) == 0) return r(text, "[para]");
    else if (strncmp(*text, "link]", strlen("link]")) == 0) return r(text, "[link]");
}

void jump_space(const char ** text)
{
    while(**text == ' ' || **text == '\n') {
	(*text)++;
    }
    return;
}

RadownValue *parse(const char **text)
{
    jump_space(text);
    
    if (strncmp(*text, "[para]", 6) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	value->type = TYPE_PARA;
	value->value = read_content(text);
	return value;
    }
    else if (strncmp(*text, "[header]", 8) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	value->type = TYPE_HEADER;
	value->value = read_content(text);
	return value;
    }
    else if (strncmp(*text, "[link]", 6) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	value->type = TYPE_LINK;
	value->value = read_content(text);
	return value;
    }
    return NULL;
}

char *lexer(const char *tmp)
{
    RadownValue *main = parse(&tmp);
    char *content = "";
    while (main != NULL) {
	char *_old = content;
	char *_new = "";
	if (main->type == TYPE_HEADER) {
	    const char *tag = "<h1></h1>";
	    size_t length = (strlen(main->value) + strlen(tag) + 1);
	    _new = (char*)malloc(length);
	    
	    snprintf(_new, length, "<h1>%s</h1>", main->value);
	}
	else if (main->type == TYPE_PARA) {
	    const char *tag = "<p></p>";
	    size_t length = (strlen(main->value) + strlen(tag) + 1);
	    _new = (char*)malloc(length);
	    
	    snprintf(_new, length, "<p>%s</p>", main->value);
	}
	else if (main->type == TYPE_LINK) {
	    // TODO: refactor it.
	    const char *tag = "<a href=''></a>";
	    size_t length = ((strlen(main->value) * 2) + strlen(tag) + 1);
	    _new = (char*)malloc(length);
	    
	    snprintf(_new, length, "<a href='%s'>%s</a>", main->value, main->value);
	}
	size_t total_length = (strlen(_old) + strlen(_new) + 1);
	content = (char*)malloc(total_length);
	snprintf(content, total_length, "%s%s", _old, _new);
	main = parse(&tmp);
    }
    printf("\n%s\n\n", content);
    free(main);
    return content;
}

