/*
 * Test basic redis pub/sub interface.  To test:
 * $ redis-server &
 * $ ./sketch_hiredis02
 * <in another window...>
 * $ redis-cli
 * > publish binky woof
 * <look at output in hiredis02 window>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis.h"

const char * const TYPES[] = { 
  "REDIS_REPLY_WTF",
  "REDIS_REPLY_STRING",
  "REDIS_REPLY_ARRAY",
  "REDIS_REPLY_INTEGER",
  "REDIS_REPLY_NIL",
  "REDIS_REPLY_STATUS",
  "REDIS_REPLY_ERROR"
};

void print_indent(int indent) {
  for (int i=0; i<indent; i++) { printf(" "); }
}

void print_reply_aux(redisReply *r, int indent) {
  print_indent(indent);
  if (r == NULL) {
    printf("<null>");
  } else {
    printf("type=%s, ele=%ld, int=%lld, str=%s\n", 
           TYPES[r->type], r->elements, r->integer, r->str);
    for (int i=0; i<r->elements; i++) {
      print_reply_aux(r->element[i], indent+2);
    }
  }
}

/* 
 * Print the details of a redis reply.
 */
void print_reply(redisReply *r) {
  print_reply_aux(r, 0);
}

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
  print_reply(reply);
  freeReplyObject(reply);

  while(1) {
    int i = redisGetReply(c, (void **)&reply);
    printf("redisGetReply() => %d, reply = %p\n", i, reply);
    print_reply(reply);
    if (reply != NULL) freeReplyObject(reply);
  }

  return 0;
}
