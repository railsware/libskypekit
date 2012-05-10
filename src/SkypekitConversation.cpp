/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#include "skypekit.h"
#include "skypekit_internal.h"

SkypekitChatMessageData* SkypekitConversation::CreateChatMessageData(const MessageRef& message) {
  SkypekitChatMessageData* data;

  SEString convo_id;
  SEString convo_guid;
  SEString author;
  SEString author_displayname;
  SEString body_xml;

  int payload_size;
  char* payload;

  skypekit_D("SkypekitAccount::CreateChatMessageData get props...\n");

  payload_size = 0;

  SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(this, GetPropIdentity, convo_id, payload_size)
  SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(message, GetPropConvoGuid, convo_guid, payload_size)
  SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(message, GetPropAuthor, author, payload_size)
  SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(message, GetPropAuthorDisplayname, author_displayname, payload_size)
  SKYPEKIT_GET_PROP_AND_COUNT_PAYLOAD(message, GetPropBodyXml, body_xml, payload_size)
  
  skypekit_D("SkypekitAccount::CreateChatMessageData allocate (%d)...\n", payload_size);
  SKYPEKIT_ALLOCATE_STRUCT_WITH_PAYLOAD(data, SkypekitChatMessageData, payload_size)
  skypekit_D("SkypekitAccount::CreateChatMessageData allocated\n");

  payload = data->payload;

  SKYPEKIT_ADD_PAYLOAD(data, payload, convo_id, convo_id)
  SKYPEKIT_ADD_PAYLOAD(data, payload, convo_guid, convo_guid)
  SKYPEKIT_ADD_PAYLOAD(data, payload, author, author)
  SKYPEKIT_ADD_PAYLOAD(data, payload, author_displayname, author_displayname)
  SKYPEKIT_ADD_PAYLOAD(data, payload, body_xml, body_xml)

  message->GetPropTimestamp(data->timestamp);

  return data;
}

void SkypekitConversation::OnMessage(const MessageRef& message) {
  SkypekitSkype *skype;
  Message::TYPE messageType;
  SkypekitChatMessageData* data;

  skypekit_D("SkypekitAccount::OnMessage...\n");

  skype = (SkypekitSkype*) this->root;

  message->GetPropType(messageType);

  if (messageType == Message::POSTED_TEXT) {
    data = this->CreateChatMessageData(message);
    skype->queue->push(data);
  } else {
    skypekit_D("SkypekitAccount::OnMessage UNSUPPORTED message type %d\n", messageType);
  }
}
