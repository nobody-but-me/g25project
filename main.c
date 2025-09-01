
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

const char *file_to_char(const char *file_path) {
    long buffer_size = 0;
    char *buffer;
    FILE *file;
    
    file = fopen(file_path, "r");
    if (file == NULL) {
	fprintf(stderr, "[FAILED] : File could not be loaded. \n");
	fclose(file);
	return NULL;
    }
    if (fseek(file, 0, SEEK_END) < 0) goto BLANK;
    buffer_size = ftell(file);
    buffer = (char *)malloc(buffer_size + 1);
    if (buffer == NULL) {
	fprintf(stderr, "[FAILED] : Buffer could not be read. \n");
	return NULL;
    }
    
    if (fseek(file, 0, SEEK_SET) < 0) goto BLANK;
    fread(buffer, 1, buffer_size, file);
    if (ferror(file)) {
	fprintf(stderr, "[FAILED] : File could not be read. \n");
	return NULL;
    }
    fclose(file);
    
#if VERBOSE == true
    printf("[INFO] : File have been loaded sucessfully. \n");
#endif
    return buffer;
    
BLANK:
    fprintf(stderr, "[INFO] : File is blank. \n");
    fclose(file);
    
    buffer = (char *)malloc(sizeof(char));
    free(buffer);
    return NULL;
}

int handle_client(int client) {
    char buffer[1024];
    read(client, buffer, sizeof(buffer));
    
    const char *templ = file_to_char("../template.html");
    if (templ == NULL) {
	fprintf(stderr, "[FAILED] : Template could not be loaded.\n");
	close(client);
	return -1;
    }
    const char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char *html;
    
    int new_length = sizeof(char) * (strlen(http_header) + strlen(templ));
    html = (char *)malloc(new_length);
    sprintf(html, "%s%s", http_header, templ);
    write(client, html, strlen(html));
    close(client);
    return 0;
}

int main(int argv, char *argc[]) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(4242);
    addr.sin_family = AF_INET;
    
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 10);
    
    printf("[INFO] : Server is running at http://localhost:4242 address.\n");
    while (true) {
	int client = accept(server, NULL, NULL);
	if (handle_client(client) == -1) {
	    break;
	}
    }
    return 0;
}
