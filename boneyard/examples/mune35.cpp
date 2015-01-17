/*
 * Sketch for client / server framework using Redis.  
 * 
 * In the Mu world, the Server is the real-time synthesis engine.  It
 * accepts relatively simple time-stamped commands to initiate sounds.
 * The Client is any application that generates the time-stamped
 * commands.
 *
 * While running, the Server frequently broadcasts its current time
 * to Clients.  A client may use this in any way it sees fit, but the
 * typical use is to queue up a few events in the near future.
 *
 * Implementation:
 *
 * The Server runs in two threads: a reader thread that listens for
 * events from Clients and a processing thread that serves buffers of
 * data to the insatiable DAC.  The processing thread also announces
 * its current time at the start of each new buffer.
 *
 * Inter-process communication:
 *
 *
 * The Server broadcasts its times via the Redis PUBSUB mechanism.
 *
 * Clients push events onto the mu_queue.  
The server pulls events
 * from the mu_queue (blocking if none available)
 * mu_queue via BLPOP
 * where the "score" is the time stamp and the "member" is a JSON
 * formatted string.  
 *
 * A client adds a command via:
 *    ZADD queue timestamp json_string
 *
 * The Server finds the earliest command via 
 *    key = ZRANGE queue 0 0
 *    ZREM #{key}
 *    process key..
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "hiredis.h"

typedef double mu_time;

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
 * ================================================================
 * CLIENT 
 */

/* forward refs */
void run_client();
void client_setup();
mu_time client_get_timestamp();
void client_queue_commands(mu_time timestamp);

/* global state */

redisContext *gClientQueueContext;
redisContext *gClientTimerContext;

void run_client() {
  client_setup();
  while (true) {
    mu_time timestamp = client_get_timestamp();
    client_queue_commands(timestamp);
  }
}

void client_setup() { 
  struct timeval timeout = { 1, 500000 }; // 1.5 seconds
  redisReply *reply;

  gClientQueueContext = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
  if (gClientQueueContext != NULL && gClientQueueContext->err) {
    printf("Error: %s\n", gClientQueueContext->errstr);
    exit(-1);
  }
  gClientTimerContext = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
  if (gClientTimerContext != NULL && gClientTimerContext->err) {
    printf("Error: %s\n", gClientTimerContext->errstr);
    exit(-1);
  }
  reply = (redisReply *)redisCommand(gClientTimerContext, "SUBSCRIBE mu_timer");
  printf("after SUBSCRIBE ");
  print_redis_reply(reply);
  freeReplyObject(reply);
}

mu_time client_get_timestamp() {
  redisReply *reply;

  redisGetReply(gClientTimerContext, (void **)&reply);
  if (reply != NULL) {
    printf("in client_get_timestamp() ");
    print_redis_reply(reply);

    if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements == 3)) {
      const char *text = reply->element[2]->str;
      freeReplyObject(reply);
      return (mu_time)atof(text);
    }
    freeReplyObject(reply);
  }
  return (mu_time)-1.0;
}

void client_queue_commands(mu_time timestamp) {
  char cmd[1024];
  sprintf(cmd, "ZADD mu_queue %f { \"time\" : %f}", timestamp, timestamp);
  redisReply *reply = (redisReply *)redisCommand(gClientQueueContext, cmd);
  printf("in client_queue_commands() ");
  if (reply != NULL) {
    print_redis_reply(reply);
    freeReplyObject(reply);  
  }
}

/* ================================================================
 * ================================================================
 * SERVER
 */

/* forward refs */
void run_server();
void server_setup();
void server_start_timestamp_broadcaster();
const char *server_wait_for_command();
void server_process_command(const char *command);

/* global state */

redisContext *gServerQueueContext;
redisContext *gServerTimerContext;


void run_server() {
  server_setup();
  server_start_timestamp_broadcaster();
  while (true) {
    const char *command = server_wait_for_command();
    server_process_command(command);
  }
}

void server_setup() {
  struct timeval timeout = { 1, 500000 }; // 1.5 seconds
  redisReply *reply;

  gServerQueueContext = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
  if (gServerQueueContext != NULL && gServerQueueContext->err) {
    printf("Error: %s\n", gServerQueueContext->errstr);
    exit(-1);
  }
  gServerTimerContext = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
  if (gServerTimerContext != NULL && gServerTimerContext->err) {
    printf("Error: %s\n", gServerTimerContext->errstr);
    exit(-1);
  }
}

void server_start_timestamp_broadcaster() {
  pthread_t *thread;
  int rc =  pthread_create(&thread, NULL, server_ticker, NULL);
  if (rc != 0) {
    printf("error starting server thread, errno = %d\n", rc);
  }
}

void server_ticker( void ) {
  char cmd[1024];
  while (1) {
    fprintf("."); fflush(stdout);
    sprintf(cmd, "PUBLISH mu_timer %d", time(NULL));
    reply = (redisReply *)redisCommand(gServerTimerContext, cmd);
    printf("after PUBLISH ");
    print_redis_reply(reply);
    freeReplyObject(reply);
    sleep(3);
  }
}

const char *server_wait_for_command() {
  return "";
}

void server_process_command(const char *command) {
}

/* ================================================================
 * ================================================================
 * Main
 */


void usage(int ac, char **av) {
  printf("Usage: %s --server | --client\n", av[0]);
  exit(-1);
}

int main(int ac, char **av) {
  if ((ac == 2) && (strcmp(av[1], "--server") == 0)) {
    run_server();
  } else if ((ac == 2) && (strcmp(av[1], "--client") == 0)) {
    run_client();
  } else {
    usage(ac, av);
  }

  return 0;
}
