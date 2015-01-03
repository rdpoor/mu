/*
 * Test basic redis pub/sub interface with JSON messages.  To test:
 * $ redis-server &
 * $ ./sketch_hiredis03
 * <in another window...>
 * $ redis-cli
 * > publish binky publish binky '[1.234, "target", { "name" : "george", "value" : 3.45 }]'
 * <look at output in hiredis03 window>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis.h"
#include "jansson.h"

void print_indent(int indent) {
  for (int i=0; i<indent; i++) { printf(" "); }
}

/* ================================================================
 * REDIS
 */

const char * const TYPES[] = { 
  "REDIS_REPLY_WTF",
  "REDIS_REPLY_STRING",
  "REDIS_REPLY_ARRAY",
  "REDIS_REPLY_INTEGER",
  "REDIS_REPLY_NIL",
  "REDIS_REPLY_STATUS",
  "REDIS_REPLY_ERROR"
};

void print_redis_reply_aux(redisReply *r, int indent) {
  print_indent(indent);
  if (r == NULL) {
    printf("<null>");
  } else {
    printf("type=%s, ele=%ld, int=%lld, str=%s\n", 
           TYPES[r->type], r->elements, r->integer, r->str);
    for (int i=0; i<r->elements; i++) {
      print_redis_reply_aux(r->element[i], indent+2);
    }
  }
}

/* 
 * Print the details of a redis reply.
 */
void print_redis_reply(redisReply *r) {
  print_redis_reply_aux(r, 0);
}

/* ================================================================
 * JSON 
 */

/* forward refs */
void print_json(json_t *root);
void print_json_aux(json_t *element, int indent);
void print_json_indent(int indent);
const char *json_plural(int count);
void print_json_object(json_t *element, int indent);
void print_json_array(json_t *element, int indent);
void print_json_string(json_t *element, int indent);
void print_json_integer(json_t *element, int indent);
void print_json_real(json_t *element, int indent);
void print_json_true(json_t *element, int indent);
void print_json_false(json_t *element, int indent);
void print_json_null(json_t *element, int indent);

void print_json(json_t *root) {
  print_json_aux(root, 0);
}

void print_json_aux(json_t *element, int indent) {
  switch json_typeof(element) {
    case JSON_OBJECT:
      print_json_object(element, indent);
      break;
    case JSON_ARRAY:
      print_json_array(element, indent);
      break;
    case JSON_STRING:
      print_json_string(element, indent);
      break;
    case JSON_INTEGER:
      print_json_integer(element, indent);
      break;
    case JSON_REAL:
      print_json_real(element, indent);;
      break;
    case JSON_TRUE:
      print_json_true(element, indent);;
      break;
    case JSON_FALSE:
      print_json_false(element, indent);;
      break;
    case JSON_NULL:
      print_json_null(element, indent);;
      break;
    default:
      fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
    }
}

void print_json_indent(int indent) {
  for (int i=0; i<indent; i++) { putchar(' '); }
}

const char *json_plural(int count) {
  return count == 1 ? "" : "s";
}

void print_json_object(json_t *element, int indent) {
  print_json_indent(indent);
  size_t size = json_object_size(element);
  const char *key;
  json_t *value;

  printf("JSON Object of %ld pair%s:\n", size, json_plural(size));
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    print_json_aux(value, indent + 2);
  }

}

void print_json_array(json_t *element, int indent) {
  print_json_indent(indent);
  size_t size = json_array_size(element);

  printf("JSON Array of %ld element%s:\n", size, json_plural(size));
  for (size_t i=0; i<size; i++) {
    print_json_aux(json_array_get(element, i), indent + 2);
  }
}

void print_json_string(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON String: \"%s\"\n", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON Real: %f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON True\n");
}

void print_json_false(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON False\n");
}

void print_json_null(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON Null\n");
}


json_t *load_json(const char *text) {
  json_t *root;
  json_error_t error;

  root = json_loads(text, 0, &error);

  if (root) {
    return root;
  } else {
    fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
    return (json_t *)0;
  }
}

/* ================================================================
 * MAIN
 */

int main(void) {
  redisContext *c;
  redisReply *reply;

  struct timeval timeout = { 1, 500000 }; // 1.5 seconds
  c = redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
  if (c->err) {
    printf("Connection error: %s\n", c->errstr);
    exit(1);
  }

  reply = redisCommand(c,"SUBSCRIBE binky");
  print_redis_reply(reply);
  freeReplyObject(reply);

  while(1) {
    int i = redisGetReply(c, (void **)&reply);
    printf("redisGetReply() => %d, reply = %p\n", i, reply);
    print_redis_reply(reply);
    if (reply != NULL) {
      if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements == 3)) {
        const char *text = reply->element[2]->str;
        printf("raw text = %s\n", text);

        json_t *root = load_json(text);
        if (root) {
          print_json(root);
          json_decref(root);
        }
      }
      freeReplyObject(reply);
    }
  }

  return 0;
}
