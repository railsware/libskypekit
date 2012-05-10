/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#include "skypekit.h"
#include "skypekit_internal.h"

/***************************************************
 * Libskypekit API main methods
 **************************************************/

/**
 * Сreate new skype.
 *
 * @return pointer to skype data
 * @see SkypekitSkype_free
 */
SkypekitSkype* skypekit_skype_new() {
  SkypekitSkype* skype;

  skype = new SkypekitSkype();

  skypekit_D("Skype is created\n");
  return skype;
}

/**
 * Init skype
 * @param sk_skype an skype instance
 * @param host a runtime IP host
 * @param port a runtime IP port
 * @param keyfile a file that contains keypair for runtime authentication
 * @param logfile a file for debug output
 */
skypekit_error_t skypekit_skype_init(struct SkypekitSkype* skype,
    const char* keyfile,
    const char *host, 
    int port,
    const char* logfile)
{
  skypekit_D("Skype initializing ...\n");

  if(skype->initialized) {
    return SKYPEKIT_ERROR_ALREADY_INITIALIZED;
  }

  if(skype->running) {
    return SKYPEKIT_ERROR_ALREADY_RUNNING;
  }

  if(!skype->ReadCertificate(keyfile)) {
    return SKYPEKIT_ERROR_KEYFILE_ACCESS;
  }

  skype->init(skype->certbuff, host, port, logfile);

  skype->initialized = true;

  skypekit_D("Skype is initialized\n");
  return SKYPEKIT_OK;
}

/**
 * Start skype
 */
skypekit_error_t skypekit_skype_start(struct SkypekitSkype* skype) {
  skypekit_D("Skype starting ...\n");

  if(!skype->initialized)
    return SKYPEKIT_ERROR_NOT_INITIALIZED;

  if(skype->running)
    return SKYPEKIT_ERROR_ALREADY_RUNNING;

  skypekit_D("Skype start()...\n");
  skype->start();

  skype->running = true;

  skypekit_D("Skype started");
  return SKYPEKIT_OK;
}

/**
 * Log in to skype account
 */
skypekit_error_t skypekit_skype_login(struct SkypekitSkype* skype,
    const char* username,
    const char* password)
{
  skypekit_D("Skype logging in ...\n");

  if (!username)
    return SKYPEKIT_ERROR_SKYPENAME_REQUIRED;

  if (!password)
    return SKYPEKIT_ERROR_PASSWORD_REQUIRED;

  if (!skype->GetAccount(username, skype->current_account))
    return SKYPEKIT_ERROR_ACCOUNT_NOT_FOUND;

  skype->current_username = username;

  if (!skype->current_account->LoginWithPassword(password, false, false))
    return SKYPEKIT_ERROR_LOGIN_FAILURE;

  skypekit_D("Skype logging finish\n");
  // now callback will notify us when login successful
  return SKYPEKIT_OK;
}

/**
 * Log out skype account
 */
skypekit_error_t skypekit_skype_logout(struct SkypekitSkype* skype)
{
  skypekit_D("Skype logging out ...\n");

  if (!skype->current_account.present())
    return SKYPEKIT_ERROR_ACCOUNT_NOT_FOUND;

  skype->current_account->Logout(false);

  skypekit_D("Skype is logged out\n");

  // now callback will notify us when logout finished
  return SKYPEKIT_OK;
}

/**
 * Stop skype
 */
skypekit_error_t skypekit_skype_stop(struct SkypekitSkype* skype) {
  if(!skype->running)
    return SKYPEKIT_ERROR_NOT_RUNNING;

  skypekit_D("Skype stopping ...\n");

  skype->stop();
  skype->running = false;

  skypekit_D("Skype stopped\n");
  return SKYPEKIT_OK;
}


/**
 * Free skype object
 */
void skypekit_skype_free(struct SkypekitSkype* skype) {
  if (skype) {
    skypekit_D("Delete skype object %p\n", skype);
    delete skype;
    skype = NULL;
  }
}

/***************************************************
 * Libskypekit API event methods
 **************************************************/

/*
 * Count unprocessed events
 *
 * @return events count
 */
int skypekit_count_events(struct SkypekitSkype* skype) {
  return skype->queue->count();
}

/**
 * Get unprocessed event
 * @return SkypekitEvent object or NULL
 */
struct SkypekitEvent* skypekit_get_event(struct SkypekitSkype* skype) {
  skypekit_D("get event...\n");
  return skype->queue->pop();
}

/**
 * Clear unprocessed events
 */
void skypekit_clear_events(struct SkypekitSkype* skype) {
  skypekit_D("Clear events queue\n");
  skype->queue->clear();
}

/**
 * Free event allocated memory
 */

void skypekit_event_free(struct SkypekitEvent* event) {
  skypekit_D("Free event memory: %p\n", event);
  SKYPEKIT_FREE_EVENT(event)
}

/***************************************************
 * Libskypekit API chat methods
 **************************************************/

/**
 * Send chat message 
 * @param skype a skype object
 * @param conversation_id a conversation identity
 * @param text an message text
 * @param is_xml is XML or not
 */

skypekit_error_t skypekit_chat_send_message(struct SkypekitSkype* skype,
    const char* conversation_id,
    const char* text,
    int is_xml)
{
  SkypekitConversation::Ref conversation;
  MessageRef message; // ignored btw, we don't need the Message object later

  skypekit_D("Sending message...\n");

  if (!skype->GetConversationByIdentity(conversation_id, conversation)) {
    return SKYPEKIT_ERROR_CONVERSATION_NOT_FOUND;
  }

  if (!conversation->PostText(text, message, is_xml)) {
    return SKYPEKIT_ERROR_SEND_MESSAGE_FAILURE;
  }

  return SKYPEKIT_OK;
}
