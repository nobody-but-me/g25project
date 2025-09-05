
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
    char buffer[1024] = {0};
    if (read(client, buffer, sizeof(buffer)) < 0) {
	fprintf(stderr, "[FAILED] : Failed to read buffer from client. \n");
	return -1;
    }
    const char *templ  = file_to_char("../index.html");
    const char *result = translate_content();
    
    if (templ == NULL || result == NULL) {
	fprintf(stderr, "[FAILED] : Template could not be loaded.\n");
	const char *err = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error";
	write(client, err, strlen(err));
	close(client);
	return -1;
    }
    const char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char *html = strrep(templ, "%t", result);
    if (html == NULL) {
	fprintf(stderr, "[FAILED] : Failed to inject Radown files content into HTML script.\n");
	close(client);
	return -1;
    }
    
    size_t new_length = (strlen(http_header) + strlen(html) + 1);
    char *final_html = (char *)malloc(new_length);
    if (!final_html) {
	fprintf(stderr, "[FAILED] : Failed to allocate memory for response");
	close(client);
	free(html);
	return -1;
    }
    snprintf(final_html, new_length, "%s%s", http_header, html);
    write(client, final_html, strlen(final_html));
    
    free(final_html);
    close(client);
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
    
    // -- stole code for test purposes.
    int opt = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { perror("setsockopt(SO_REUSEADDR) failed"); close(server); return -1; }
    // --
    
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
