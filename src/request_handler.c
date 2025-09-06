

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "./include/request_handler.h"
#include "./include/parser.h"
#include "./utils/str.h"
#include "./utils/os.h"

#define CURRENT_FILE "../moc_project/main.rd" // for test purposes.
#define BUFFER_SIZE 1024

static const char *translate_content()
{
    char *file = load_file(CURRENT_FILE);
    if (!file) {
	fprintf(stderr, "[FAILED] : Main file could not be loaded.\n");
	return NULL;
    }
    const char *content = lexer((char const *)file);
    if (content == NULL) {
	fprintf(stderr, "[FAILED] : Failed to parse main file.\n");
	return NULL;
    }
    return content;
}

static char *render_content() {
    const char *templ  = file_to_char("../index.html");
    const char *result = translate_content();
    
    if (templ == NULL || result == NULL) {
	fprintf(stderr, "[FAILED] : Failed to load content.\n");
	return NULL;
    }
    char *content = strrep(templ, "[content]", result);
    if (content == NULL) {
	fprintf(stderr, "[FAILED] : Failed to inject Radown files content into HTML template.\n");
	return NULL;
    }
    return content;
}

int handle_client(int socket)
{
    char buffer[BUFFER_SIZE] = { 0 };
    read(socket, buffer, BUFFER_SIZE);
    
    char *content = render_content();
    if (content == NULL) {
	fprintf(stderr, "[FAILED] : Content could not be rendered.\n");
	return -1;
    }
    if (content) {
	const char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	write(socket, http_header, strlen(http_header));
	write(socket, content, strlen(content));
    } else {
	const char* not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
	write(socket, not_found_response, strlen(not_found_response));
	free(content);
	close(socket);
	return -1;
    }
    free(content);
    close(socket);
    return 0;
}
