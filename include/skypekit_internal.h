/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#ifndef __SKYPEKIT_INTERNAL_H__
#define __SKYPEKIT_INTERNAL_H__

#include "skype-embedded_2.h"

/**
 * Debug macro
 */ 
#ifdef DEBUG
#define skypekit_D(format, ...) fprintf (stderr, "[LIBSKYPEKIT %s:%d] " format, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define skypekit_D(format, ...)
#endif

/**
 * Macro to allocate memory for struct with payload
 * @note it also nullify allocated memory
 */
#define SKYPEKIT_ALLOCATE_STRUCT_WITH_PAYLOAD(var_name, struct_name, payload_size) \
  var_name = (struct struct_name*)malloc(sizeof(struct struct_name) + payload_size); \
  memset(var_name, 0, sizeof(struct struct_name) + payload_size); 

/**
 * Macro to allocate memory for struct.
 * @note it also nullify allocated memory
 */
#define SKYPEKIT_ALLOCATE_STRUCT(var_name, struct_name) \
  SKYPEKIT_ALLOCATE_STRUCT_WITH_PAYLOAD(var_name, struct_name, 0)

/**
 * Macro to get string property from object and count payload
 */
#define SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(object, method, string, count) \
  object->method(string); \
  count += strlen(string) + 1;

/**
 * Macro for adding value to payload
 * @note value should be C-string (sequence of characters terminated by '\0'
 */
#define SKYPEKIT_ADD_PAYLOAD(struct_ptr, iterator_ptr, attr_name, source) \
  struct_ptr->attr_name = iterator_ptr; \
  if(source) { \
    strcpy(iterator_ptr, source); \
    payload += strlen(source) + 1; \
  } else { \
    payload += 1; \
  }

/**
 * Macro for de-allocating event memory
 */
#define SKYPEKIT_FREE_EVENT(event) \
  if(event->data) { free(event->data); } \
  free(event); 

/***************************************************
 * SkypeKit C++ API
 **************************************************/

class SkypekitSkype;
class SkypekitAccount;
class SkypekitConversation;
class SkypekitQueue;

using namespace Sid;

/**
 * @see http://developer.skype.com/skypekit/reference/cpp/class_account.html
 */
class SkypekitAccount : public Account {
  public:

    typedef DRef< SkypekitAccount, Account> Ref;
    
    typedef DRefs<SkypekitAccount, Account> Refs;
    
    SkypekitAccount(unsigned int oid, SERootObject* root) : Account(oid, root) {}

    void OnChange(int prop);
    
    SkypekitAccountStatusData* CreateAccountStatusData(int status);
};

/**
 * @see http://developer.skype.com/skypekit/reference/cpp/class_conversation.html
 */
class SkypekitConversation : public Conversation {
  public:

    typedef DRef< SkypekitConversation, Conversation> Ref;
    
    typedef DRefs<SkypekitConversation, Conversation> Refs;
    
    SkypekitConversation(unsigned int oid, SERootObject* root) : Conversation(oid, root) {}

    void OnMessage(const MessageRef & message);
    
    SkypekitChatMessageData* CreateChatMessageData(const MessageRef& message);
};

/**
 * @see http://developer.skype.com/skypekit/reference/cpp/class_skype.html
 */
class SkypekitSkype : public Skype {

  public:
    SkypekitSkype();

    ~SkypekitSkype();

    SkypekitAccount::Ref current_account;

    char *certbuff;

    const char* current_username;

    bool initialized;

    bool running;

    Account* newAccount(int oid) { return new SkypekitAccount(oid, this); }
    
    Conversation* newConversation(int oid) { return new SkypekitConversation(oid, this); }

    bool ReadCertificate(const char* filename);

    void OnConversationListChange(
        const ConversationRef &conversation,
        const Conversation::LIST_TYPE &type,
        const bool &added);

    void OnContactOnlineAppearance(const ContactRef &contact);

    void OnContactGoneOffline(const ContactRef &contact);

    void FetchConversationList();

    bool GetParticularConversationByIdx(unsigned int index, SkypekitConversation::Ref& retConv);

    unsigned int GetConversationsInboxSize() { return conversations.size(); }

    SkypekitQueue* queue;

  protected:
    
    SkypekitConversation::Refs conversations;
};


/**
 * SkypekitQueue implements queue for SkypeKit events
 * @note it's not SDK class
 */
class SkypekitQueue {

  SkypekitSkype* skype;
  
  Sid::Mutex* mutex;
  
  struct SkypekitEvent* head_event;
  
  struct SkypekitEvent* tail_event;
  
  int size;

  public:

    SkypekitQueue(SkypekitSkype* skype);
  
    ~SkypekitQueue();

    void push(SkypekitAccountStatusData* data);
  
    void push(SkypekitChatMessageData* data);
  
    void push(SkypekitEvent* event);

    struct SkypekitEvent* pop();

    int count();

    void clear();
};

#endif // __SKYPEKIT_INTERNAL_H__
