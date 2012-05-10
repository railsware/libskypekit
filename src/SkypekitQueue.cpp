/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#include "skypekit.h"
#include "skypekit_internal.h"

/**
 * Queue constructor
 */
SkypekitQueue::SkypekitQueue(SkypekitSkype* skype) {
  skypekit_D("SkypekitQueue new...\n");

  this->skype = skype;
  this->head_event = NULL;
  this->tail_event = NULL;
  this->size = 0;
  this->mutex = new Sid::Mutex();
  skypekit_D("Created mutex object %p\n", this->mutex);
}

/**
 * Queue destructor
 */
SkypekitQueue::~SkypekitQueue() {
  this->clear();
  skypekit_D("Delete mutex object %p\n", this->mutex);
  delete this->mutex;
}


/**
 * Push SkypekitAccountStatusData event into queue
 */
void SkypekitQueue::push(SkypekitAccountStatusData* data) {
  SkypekitEvent* event;

  SKYPEKIT_ALLOCATE_STRUCT(event, SkypekitEvent)

  event->type = SKYPEKIT_EVENT_ACCOUNT_STATUS;
  event->timestamp = time(NULL);
  event->data = data;

  push(event);
}

/**
 * Push SkypekitChatMessageData event into queue
 */
void SkypekitQueue::push(SkypekitChatMessageData* data) {
  SkypekitEvent* event;

  SKYPEKIT_ALLOCATE_STRUCT(event, SkypekitEvent)

  event->type = SKYPEKIT_EVENT_CHAT_MESSAGE;
  event->timestamp = time(NULL);
  event->data = data;

  push(event);
}

/**
 * Push event into queue
 */
void SkypekitQueue::push(struct SkypekitEvent* event) {
  skypekit_D("SkypekitQueue::push mutex locking\n");
  mutex->lock();
  skypekit_D("SkypekitQueue::push mutex locked\n");

  if (!head_event) {
    tail_event = head_event = event;
  } else {
    tail_event = tail_event->next = event;
  }

  ++size;

  skypekit_D("SkypekitQueue::push mutex unlocking\n");
  mutex->unlock();
  skypekit_D("SkypekitQueue::push mutex unlocked\n");
};


/**
 * Pop event
 */
struct SkypekitEvent* SkypekitQueue::pop() {
  SkypekitEvent* event;

  skypekit_D("SkypekitQueue::pop mutex locking\n");
  mutex->lock();
  skypekit_D("SkypekitQueue::pop mutex locked\n");

  event = head_event;

  if (event) {
    head_event = event->next;
    --size;
  }

  skypekit_D("SkypekitQueue::pop mutex unlocking\n");
  mutex->unlock();
  skypekit_D("SkypekitQueue::pop mutex unlocked\n");

  return event;
}

/**
 * Count events in queue
 */
int SkypekitQueue::count() {
  return size;
}

/**
 * Clear queue
 */
void SkypekitQueue::clear() {
  SkypekitEvent* event;

  skypekit_D("SkypekitQueue::clear mutex locking\n");
  mutex->lock();
  skypekit_D("SkypekitQueue::clear mutex locked\n");

  while ((event = this->head_event)) {
    this->head_event = event->next;
    SKYPEKIT_FREE_EVENT(event)
  }

  this->head_event = NULL;
  this->tail_event = NULL;
  this->size = 0;

  skypekit_D("SkypekitQueue::clear mutex unlocking\n");
  mutex->unlock();
  skypekit_D("SkypekitQueue::clear mutex unlocked\n");
}
