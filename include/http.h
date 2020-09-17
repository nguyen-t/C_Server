#ifndef HTTP_H
#define HTTP_H

// String used to idenfity HTTP requests
#define HTTP_REGEX "^(OPTIONS|GET|HEAD|POST|PUT|DELETE|TRACE|CONNECT) \\/.* HTTP\\/(1\\.1|2\\.0)\r\n" \
                   "([a-zA-Z]+(-[a-zA-Z]+)*: (!|#|\\$|%|&|'|\\*|\\+|-|\\.|\\^|`|\\||~|\\w)+\r\n)*(\r\n.*)*"

typedef struct {
  union {
    struct {
      char* part_a;
      char* part_b;
      char* part_c;
    };
    struct {
      char* protocol;
      char* path;
      char* version;
    } req;
    struct {
      char* version;
      char* code;
      char* message;
    } res;
  };
  char* header;
  char* body;
  int length;
} HTTP_Message;

int http_verify(const char*);
HTTP_Message* http_format(char*, int);
HTTP_Message* http_build(const char*, const char*, const char*);
char* http_path(HTTP_Message*);
char* http_header_get(HTTP_Message*, const char*);
char* http_header_set(HTTP_Message*, const char*, const char*);

/****************** TODO: Implement these *********************/
char* http_header_del(HTTP_Message*, const char*);
char* http_query_get(HTTP_Message*, const char*);
char* http_query_set(HTTP_Message*, const char*, const char *);
char* http_query_del(HTTP_Message*, const char*);
/*************************************************************/

void http_body_detach(HTTP_Message*);
char* http_attach_text(HTTP_Message*, const char*);
char* http_attach_file(HTTP_Message*, FILE*);
char* http_stringify(HTTP_Message*);
void http_destroy(HTTP_Message*);

#endif
