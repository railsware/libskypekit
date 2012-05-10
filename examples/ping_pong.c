#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "skypekit.h"

static struct SkypekitSkype* skype;

void terminate() {
  printf("Terminating skype ...\n");

  skypekit_skype_stop(skype);
  skypekit_skype_free(skype);

  exit(0);
}

void check_error(skypekit_error_t error) {
  if (error != SKYPEKIT_OK) {
    printf("Error: %d\n", error);
    exit(0);
  }
}

int main(int argc, char** argv) {
  struct SkypekitEvent* event;
  struct SkypekitAccountStatusData* account_status_data;
  struct SkypekitChatMessageData* chat_message_data;

  const char* keyfile;
  const char* host;
  int port;
  const char* skypename;
  const char* password;

  if (argc < 6)
  {
    printf("usage: %s <keyfile> <host> <port> <skypename> <password>\n", argv[0]);
    return 1;
  };

  keyfile = argv[1];
  host = argv[2];
  port = atoi(argv[3]);
  skypename = argv[4];
  password = argv[5];

  skype = skypekit_skype_new();

  signal(SIGINT, terminate);

  printf("Initializing skype ...\n");
  skypekit_skype_init(skype, keyfile, host, port, NULL);

  printf("Start skype ...\n");
  check_error(skypekit_skype_start(skype));

  printf("Log in to skype ...\n");
  check_error(skypekit_skype_login(skype, skypename, password));

  printf("Start loop ...\n");

  while(1) {
    event = skypekit_get_event(skype);

    if (!event) {
      sleep(5);
      continue;
    }

    switch(event->type) {
      case SKYPEKIT_EVENT_ACCOUNT_STATUS:
        account_status_data = (struct SkypekitAccountStatusData*) event->data;

        if ( account_status_data->status == SKYPEKIT_ACCOUNT_STATUS_LOGGED_IN) {
          printf("Congrats! We are Logged in!\n");
        }

        if ( account_status_data->status == SKYPEKIT_ACCOUNT_STATUS_LOGGED_OUT && 
            account_status_data->reason != 0) {
          printf("Login error: %d\n", account_status_data->reason);
          terminate();
        }

        break;

      case  SKYPEKIT_EVENT_CHAT_MESSAGE:
        chat_message_data = (struct SkypekitChatMessageData*) event->data;

        printf("Message received %p\n", chat_message_data);
        printf("convo_id=%s\n", chat_message_data->convo_id);
        printf("convo_guid=%s\n", chat_message_data->convo_guid);
        printf("author=%s\n", chat_message_data->author);
        printf("author_displayname=%s\n", chat_message_data->author_displayname);
        printf("timestamp=%d\n", chat_message_data->timestamp);
        printf("body_xml=%s\n", chat_message_data->body_xml);

        if (strcmp(chat_message_data->body_xml,"ping") == 0) {
          skypekit_chat_send_message(skype, chat_message_data->convo_id, "pong", 0);
        }

        break;

      default:
        printf("Unknown event type: %d\n", event->type);
        break;
    }

    skypekit_event_free(event);
  }

  return 0;
}
