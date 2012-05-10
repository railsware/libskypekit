/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#ifndef __SKYPEKIT_H__
#define __SKYPEKIT_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************
 * Libskypekit API types
 **************************************************/

typedef enum {
  SKYPEKIT_OK = 0,
  SKYPEKIT_ERROR_ALREADY_RUNNING,
  SKYPEKIT_ERROR_ALREADY_INITIALIZED,
  SKYPEKIT_ERROR_NOT_INITIALIZED,
  SKYPEKIT_ERROR_NOT_RUNNING,
  SKYPEKIT_ERROR_KEYFILE_ACCESS,
  SKYPEKIT_ERROR_SKYPENAME_REQUIRED,
  SKYPEKIT_ERROR_PASSWORD_REQUIRED,
  SKYPEKIT_ERROR_ACCOUNT_NOT_FOUND,
  SKYPEKIT_ERROR_CONVERSATION_NOT_FOUND,
  SKYPEKIT_ERROR_LOGIN_FAILURE,
  SKYPEKIT_ERROR_SEND_MESSAGE_FAILURE,
} skypekit_error_t;

typedef enum {
  SKYPEKIT_EVENT_ACCOUNT_STATUS = 1,
  SKYPEKIT_EVENT_CHAT_MESSAGE,
} skypekit_event_t;

typedef enum {
  SKYPEKIT_ACCOUNT_STATUS_LOGGED_OUT = 1,
  SKYPEKIT_ACCOUNT_STATUS_LOGGED_OUT_AND_PWD_SAVED /* the account is logged out, but password is not needed for re-login*/,
  SKYPEKIT_ACCOUNT_STATUS_CONNECTING_TO_P2P        /* connecting to P2P network*/,
  SKYPEKIT_ACCOUNT_STATUS_CONNECTING_TO_SERVER     /* connecting to login server*/,
  SKYPEKIT_ACCOUNT_STATUS_LOGGING_IN               /* waiting for response from server*/,
  SKYPEKIT_ACCOUNT_STATUS_INITIALIZING             /* response OK. initialising account-specific lib structures*/,
  SKYPEKIT_ACCOUNT_STATUS_LOGGED_IN                /* alright, we're good to go!*/,
  SKYPEKIT_ACCOUNT_STATUS_LOGGING_OUT              /* Logout() has been called but not processed yet*/
} skypekit_account_status_t;

typedef enum {
  SKYPEKIT_ACCOUNT_REASON_LOGOUT_CALLED = 1           /* manual logout (or unknown reason from previous session)*/,
  SKYPEKIT_ACCOUNT_REASON_HTTPS_PROXY_AUTH_FAILED     /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_SOCKS_PROXY_AUTH_FAILED     /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_P2P_CONNECT_FAILED          /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_SERVER_CONNECT_FAILED       /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_SERVER_OVERLOADED           /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_DB_IN_USE                   /* sync errors at login/registration*/,
  SKYPEKIT_ACCOUNT_REASON_INVALID_SKYPENAME           /* sync errors at registration*/,
  SKYPEKIT_ACCOUNT_REASON_INVALID_EMAIL               /* sync errors at registration*/,
  SKYPEKIT_ACCOUNT_REASON_UNACCEPTABLE_PASSWORD       /* sync errors at registration*/,
  SKYPEKIT_ACCOUNT_REASON_SKYPENAME_TAKEN             /* sync errors at registration*/,
  SKYPEKIT_ACCOUNT_REASON_REJECTED_AS_UNDERAGE        /* sync errors at registration*/,
  SKYPEKIT_ACCOUNT_REASON_NO_SUCH_IDENTITY            /* sync errors at login*/,
  SKYPEKIT_ACCOUNT_REASON_INCORRECT_PASSWORD          /* sync errors at login*/,
  SKYPEKIT_ACCOUNT_REASON_TOO_MANY_LOGIN_ATTEMPTS     /* sync errors at login*/,
  SKYPEKIT_ACCOUNT_REASON_PASSWORD_HAS_CHANGED        /* async errors (can happen anytime while logged in)*/,
  SKYPEKIT_ACCOUNT_REASON_PERIODIC_UIC_UPDATE_FAILED  /* async errors (can happen anytime while logged in)*/,
  SKYPEKIT_ACCOUNT_REASON_DB_DISK_FULL                /* async errors (can happen anytime while logged in)*/,
  SKYPEKIT_ACCOUNT_REASON_DB_IO_ERROR                 /* async errors (can happen anytime while logged in)*/,
  SKYPEKIT_ACCOUNT_REASON_DB_CORRUPT                  /* async errors (can happen anytime while logged in)*/,
  SKYPEKIT_ACCOUNT_REASON_DB_FAILURE                  /* deprecated (superceded by more detailed DB_* errors)*/,
  SKYPEKIT_ACCOUNT_REASON_INVALID_APP_ID              /* platform sdk*/,
  SKYPEKIT_ACCOUNT_REASON_APP_ID_FAILURE              /* platform sdk*/,
  SKYPEKIT_ACCOUNT_REASON_UNSUPPORTED_VERSION         /* forced upgrade/discontinuation*/
} skypekit_account_reason_t;

/***************************************************
 * Libskypekit API structs
 **************************************************/

/*
 * Skype data (implementation hidden)
 */
struct SkypekitSkype;

/**
 * Event
 *
 */
struct SkypekitEvent {
  // event type
  skypekit_event_t type;

  // timestamp of event creation
  time_t timestamp;

  // event data
  void* data; 

  // next event
  struct SkypekitEvent* next;
};

/**
 * Account status event data
 */
struct SkypekitAccountStatusData {
  // account status type
  int status;

  // account status message (optional)
  int reason;

  // account skypename
  char *skypename;

  // internal payload
  char payload[];
};

/**
 * Text chat message data
 */
struct SkypekitChatMessageData {
  // contact identity in case of dialogs, chat name in case of conferences
  char* convo_id;

  // The GUID is a "global ID" - these values are shared accross Skype client instances and accross all the participants of the conversation
  char* convo_guid;

  // Identity of the sender. The same as SKYPENAME property of the Contact
  char* author;

  // Displayname of the sender at the time of posting, 
  char* author_displayname; 

  // Message body
  char* body_xml;

  // UNIX timestamp (sent time, adjusted for local clock)
  unsigned int timestamp;

  // internal storage
  char payload[];
};


/***************************************************
 * Libskypekit API methods
 **************************************************/

/*
 * Allocate new skype object.
 *
 * @return new SkypekitSkype object
 * @note Your are responsible to free resource.
 *
 * @see skypekit_skype_free()
 * */
struct SkypekitSkype* skypekit_skype_new();

/*
 * Init skype.
 *
 * @param skype a skype object
 * @param keyfile a path to cerification file
 * @param host runtime host
 * @param port runtime port
 * @param logfile streamlogfile
 */
skypekit_error_t skypekit_skype_init(struct SkypekitSkype* skype,
    const char* keyfile,
    const char *host,
    int port,
    const char* logfile);

/**
 * Start skype.
 *
 * @param skype a skype object
 */
skypekit_error_t skypekit_skype_start(struct SkypekitSkype* skype);

/*
 * Login in to skype with specified account credentials.
 *
 * @param skype a skype object
 */
skypekit_error_t skypekit_skype_login(struct SkypekitSkype* skype,
    const char* username,
    const char* password);

/**
 * Log out from skype.
 *
 * @param skype a skype object
 */
skypekit_error_t skypekit_skype_logout(struct SkypekitSkype* skype);

/**
 * Count events into queue.
 *
 * @param skype a skype object
 */
int skypekit_count_events(struct SkypekitSkype* skype);

/*
 * Pop events from queue.
 * 
 * @param skype a skype object
 *
 * @return SkypekitEvent object
 * @note your are responsible to free resources allocated to event;
 * @see skypekit_event_free()
 */
struct SkypekitEvent* skypekit_get_event(struct SkypekitSkype* skype);

/*
 * Clear events queue
 * @param skype a skype object
 */
void skypekit_clear_events(struct SkypekitSkype* skype);

/**
 * Free memory allocated for event.
 * @param event a SkypekitEvent object
 */
void skypekit_event_free(struct SkypekitEvent* event);

/**
 * Send chat message.
 *
 * @param skype a skype object
 * @param conversation_id a Conversation Id
 * @param text a message text
 * @param is_xml is a message XML formatted
 */
skypekit_error_t skypekit_chat_send_message(struct SkypekitSkype* skype,
    const char* conversation_id,
    const char* text,
    int is_xml);

/**
 * Stop skype.
 *
 * @param skype a skype object
 */
skypekit_error_t skypekit_skype_stop(struct SkypekitSkype* skype);

/**
 * Free memory allocated for SkypekitSkype object.
 *
 * @param skype a skype object
 */
void skypekit_skype_free(struct SkypekitSkype* skype);

#ifdef __cplusplus
} // extern C
#endif

#endif // ifndef __SKYPEKIT_H__
