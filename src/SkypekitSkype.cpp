/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#include "skypekit.h"
#include "skypekit_internal.h"

SkypekitSkype::SkypekitSkype() {
  this->queue = new SkypekitQueue(this);
  this->current_username = NULL;
  this->initialized = false;
  this->running = false;
  this->certbuff = NULL;
}

SkypekitSkype::~SkypekitSkype() {
  if(certbuff) {
    free(certbuff);
  }
  skypekit_D("Delete queue object %p\n", queue);
  delete queue;
}

bool SkypekitSkype::ReadCertificate(const char* filename) {
  FILE* fd;
  long int size;
  bool retval;

  retval = false;

  if ( (fd = fopen(filename, "r")) ) {
    // find out file size
    fseek(fd, 0L, SEEK_END);
    size = ftell(fd);
    rewind(fd);

    // allocate memory for buffer
    this->certbuff = (char *)malloc(size + 1);

    if (this->certbuff != NULL) {
      fread(this->certbuff, size, 1, fd);
      this->certbuff[size] = '\0';
      retval = true;
    }

    //close file
    fclose(fd);
  }

  return retval;
}

void SkypekitSkype::OnConversationListChange(
  const ConversationRef &conversation,
  const Conversation::LIST_TYPE &type,
  const bool &added)
{
  // if conversation was added to inbox and was not in there already - append
  if (type == Conversation::INBOX_CONVERSATIONS && added
      && !conversations.contains(conversation)) {
    conversations.append(conversation);
    fetch(conversations);
  }
}

void SkypekitSkype::OnContactOnlineAppearance(const ContactRef &contact) {
  skypekit_D("TODO SkypekitSkype::OnContactOnlineAppearance\n");
}
void SkypekitSkype::OnContactGoneOffline(const ContactRef &contact) {
  skypekit_D("TODO SkypekitSkype::OnContactOnlineAppearance\n");
}


void SkypekitSkype::FetchConversationList() {
  skypekit_D("START Fetching conversation list\n");
  GetConversationList(conversations, Conversation::INBOX_CONVERSATIONS);
  fetch(conversations);
  skypekit_D("DONE fetching conversation list\n");
}

