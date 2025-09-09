
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

void read_content(const char **text, RadownValue *value)
{
    if (**text != '[') return;
    (*text)++;
    
    if (strncmp(*text, "header]", 7) == 0) {
	value->value = r(text, "[header]");
	value->type = TYPE_HEADER;
	return;
    }
    else if (strncmp(*text, "para]", 5) == 0) {
	value->value = r(text, "[para]");
	value->type = TYPE_PARA;
	return;
    }
    else if (strncmp(*text, "image]", 6) == 0) {
	value->value = r(text, "[image]");
	value->type = TYPE_IMAGE;
	return;
    }
    else if (strncmp(*text, "link=", 5) == 0) {
	*text += 5;
	const char *start = *text;
	while (strncmp(*text, "]", strlen("]")) && **text != '\0') {
	    (*text)++;
	}
	size_t link_length = (size_t)(*text - start);
	char *link = (char*)malloc(link_length + 1);
	strncpy(link, start, link_length);
	link[link_length] = '\0';
	(*text)++;
	
	char *v = read_string(text, "[link]");
	if (strncmp(*text, "[link]", strlen("[link]")) == 0) {
	    (*text) += strlen("[link]");
	}
	char *result = (char*)malloc(link_length + strlen(v) + 2);
	snprintf(result, (link_length + strlen(v) + 2), "%s|%s", link, v);
	
	value->type = TYPE_LINK;
	value->value = result;
	return;
    }
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
    
    if (strncmp(*text, "[", 1) == 0) {
	RadownValue *value = malloc(sizeof(RadownValue));
	read_content(text, value);
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
	else if (main->type == TYPE_IMAGE) {
	    const char *tag = "<div class='center'><img src=''/></div>";
	    size_t length = (strlen(main->value) + strlen(tag) + 1);
	    _new = (char*)malloc(length);
	    
	    snprintf(_new, length, "<div class='center'><img src='%s'/></div>", main->value);
	}
	else if (main->type == TYPE_LINK) {
	    const char *tag = "<div class='center'><a href='' target='_blank'></a></div>";
	    
	    const char *link  = strtok(main->value, "|");
	    const char *value = strtok(NULL, "|");
	    
	    size_t length = (strlen(link) + strlen(value) + strlen(tag) + 1);
	    _new = (char*)malloc(length);
	    
	    snprintf(_new, length, "<a href='%s' target='_blank'>%s</a>", link, value);
	}
	size_t total_length = (strlen(_old) + strlen(_new) + 1);
	content = (char*)malloc(total_length);
	snprintf(content, total_length, "%s%s", _old, _new);
	main = parse(&tmp);
    }
    // printf("\n%s\n\n", content);
    free(main);
    return content;
}

