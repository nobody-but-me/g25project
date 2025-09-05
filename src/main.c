
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "./include/parser.h"
#include "./utils/str.h"

#define PROJECT_PATH "../moc_project/"


static const char *translate_content()
{
    char *tmp = load_file(PROJECT_PATH"main.rd");
    if (!tmp) {
	printf("[FAILED] : tmp could not be loaded");
	return NULL;
    }
    const char *_tmp = tmp;
    
    const char *content = lexer(_tmp);
    return content;
}

int handle_client(int client)
{
    char buffer[1024];
    read(client, buffer, sizeof(buffer));
    
    // TODO: bad practice: casting away const-ness.
    char *templ = (char *)file_to_char("../index.html");
    char *result = (char *)translate_content();
    
    if (templ == NULL) {
	fprintf(stderr, "[FAILED] : Template could not be loaded.\n");
	close(client);
	return -1;
    }
    const char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char *html;
    
    int new_length = sizeof(char) * (strlen(http_header) + strlen(templ) + 1);
    html = (char *)malloc(new_length);
    
    char *new_templ = (char*)malloc(strlen(templ) + strlen(result) + 1);
    sprintf(new_templ, "%s", templ);
    
    new_templ = strrep(new_templ, "%t", result);
    if (new_templ == NULL) {
	fprintf(stderr, "[FAILED] : Program could not write content into template.\n");
	return -1;
    }
    sprintf(html, "%s%s", http_header, new_templ);
    
    write(client, html, strlen(html));
    close(client);
    
    free(new_templ);
    free(templ);
    free(html);
    return 0;
}

volatile sig_atomic_t stop_server = 0;

void handle_sigint(int sig)
{
    stop_server = 1;
    return;
}

int main(int argv, char *argc[])
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
	printf("[FAILED] : TCP socket could not be created. \n");
	return -1;
    }
    struct sockaddr_in addr = {0};
    
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(4242);
    addr.sin_family = AF_INET;
    
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 10);
    
    printf("[INFO] : Server is running at http://localhost:4242 address.\n");
    signal(SIGINT, handle_sigint);
    while (!stop_server) {
	int client = accept(server, NULL, NULL);
	if (client == -1) {
	    if (errno == EINTR) continue;
	    else { perror("accept"); break; }
	}
	
	if (handle_client(client) == -1) {
	    break;
	}
    }
    close(server);
    printf("[INFO] : Shutting down. Bye.\n");
    return 0;
}
