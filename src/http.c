#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include "http.h"

// Checks if given C-string is a valid HTTP request
int http_verify(const char* raw) {
  // Regex will be not be freed
  static regex_t reg;
  static int compiled = 0;

  if(!compiled) {
    if(regcomp(&reg, HTTP_REGEX, REG_EXTENDED | REG_NOSUB) != 0) {
      printf("Failed to compile regex\n");
    } else {
      compiled = 1;
    }
  }

  return regexec(&reg, raw, 0, NULL, 0);
}

// Breaks a C-string into HTTP_Message
// Runtime: O(2N)
HTTP_Message* http_format(char* raw, int length) {
  HTTP_Message* msg;
  char* left = NULL;
  char* right = NULL;

  if((msg = malloc(sizeof(HTTP_Message))) == NULL) {
    return NULL;
  }

  left = raw;
  right = strchr(left, ' ');
  msg->part_a = strndup(left, right - left);

  left = right + 1;
  right = strchr(left, ' ');
  msg->part_b = strndup(left, right - left);

  left = right + 1;
  right = strstr(left, "\r\n");
  msg->part_c = strndup(left, right - left);

  left = right + 2;
  right = strstr(left, "\r\n\r\n");

  if(right == NULL) {
    msg->header = strdup(left);
    msg->body = strdup("\0");
  } else {
    msg->header = strndup(left, right - left + 2);

    left = right + 4;
    right = raw + length;
    msg->body = strndup(left, right - left);
  }

  msg->length = length;

  return msg;
}

// Builds a basic HTTP_Message
// Runtime: O(2N)
HTTP_Message* http_build(const char* a, const char* b, const char* c) {
  HTTP_Message* msg;

  if((msg = malloc(sizeof(HTTP_Message))) == NULL) {
    return NULL;
  }

  msg->part_a = strdup(a);
  msg->part_b = strdup(b);
  msg->part_c = strdup(c);
  msg->header = strdup("\0");
  msg->body = strdup("\0");
  msg->length = strlen(a) + strlen(b) + strlen(c) + 4;

  return msg;
}

// Get header field by name
// Runtime: O(2N)
char* http_header_get(HTTP_Message* msg, const char* field) {
  // Format characters takes 4 bytes
  int length = strlen(field) + 4;
  char* search = calloc(length + 1, 1);
  char* left;
  char* right;
  char* result;

  if(search == NULL) {
    return NULL;
  }

  // Build search string
  strcat(search, "\r\n");
  strcat(search, field);
  strcat(search, ": ");

  // Calculate and copy necessary memory
  left = strstr(msg->header, search);
  right = strstr(left, "\r\n");
  result = strndup(left + length, right - (left + length));

  free(search);

  return result;
}

// Extract pathname without query
// O(N)
char* http_path(HTTP_Message* msg) {
  char* end = strchr(msg->req.path, '?');

  return (end == NULL)
    ? strdup(msg->req.path)
    : strndup(msg->req.path, end - msg->req.path);
}

// Appends a header text to HTTP_Message
// Runtime: O(N + K)
char* http_header_set(HTTP_Message* msg, const char* field, const char* value) {
  // Format characters takes 4 bytes
  int delta = strlen(field) + strlen(value) + 4;
  char* check = realloc(msg->header, strlen(msg->header) + delta + 1);

  // Memory allocation and resize
  if(check == NULL) {
    return NULL;
  } else {
    msg->header = check;
    msg->length += delta;
  }

  // Append field: value
  strcat(msg->header, field);
  strcat(msg->header, ": ");
  strcat(msg->header, value);
  strcat(msg->header, "\r\n");

  return check;
}

// char* http_header_del(HTTP_Message* msg, const char* field);
// char* http_query_get(HTTP_Message* msg, const char* field);
// char* http_query_set(HTTP_Message* msg, const char* field, const char* value);
// char* http_query_del(HTTP_Message* msg, const char* field);

// Removes HTTP_Message body
// Runtime: O(N)
void http_body_detach(HTTP_Message* msg) {
  int length;

  free(msg->body);

  // Format characters takes 7 bytes
  length = 7;
  length += strlen(msg->part_a);
  length += strlen(msg->part_b);
  length += strlen(msg->part_c);
  length += strlen(msg->header);
  msg->body = strdup("\0");
  msg->length = length;
}

// TODO: Replace strncat with memcpy
// Add body from C-string
// Runtime: O(N + 2K)
char* http_attach_text(HTTP_Message* msg, const char* text) {
  long length;
  char buffer[16];

  http_body_detach(msg);
  free(msg->body);

  length = strlen(text);
  msg->body = strndup(text, length);
  msg->length += length;
  snprintf(buffer, 16, "%ld", length);
  http_header_set(msg, "Content-Length", buffer);

  return msg->body;
}

// Add body from file
// Runtime: O(N + 2K)?
char* http_attach_file(HTTP_Message* msg, FILE* f) {
  long length;
  char buffer[16];

  http_body_detach(msg);
  free(msg->body);

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);

  if((msg->body = malloc(length + 1)) == NULL) {
    return NULL;
  }

  msg->body[length] = '\0';
  msg->length += length;

  fread(msg->body, sizeof(char), length, f);
  snprintf(buffer, 16, "%ld", length);
  http_header_set(msg, "Content-Length", buffer);

  return msg->body;
}

// TODO: Replace strncat with memcpy
// Converts HTTP_Message into a C-string
// Runtime: O(3N)
char* http_stringify(HTTP_Message* msg) {
  char* str = calloc(msg->length + 1, 1);

  if(str == NULL) {
    return NULL;
  }

  strcat(str, msg->part_a);
  strcat(str, " ");
  strcat(str, msg->part_b);
  strcat(str, " ");
  strcat(str, msg->part_c);
  strcat(str, "\r\n");
  strcat(str, msg->header);
  strcat(str, "\r\n");
  strncat(str, msg->body, msg->length - strlen(str));

  return str;
}

// Frees memory for HTTP_Message instance
// Runtime: O(1)
void http_destroy(HTTP_Message* msg) {
  free(msg->part_a);
  free(msg->part_b);
  free(msg->part_c);
  free(msg->header);
  free(msg->body);
  free(msg);
}
