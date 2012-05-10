/*
 *  Copyright (c) 2012 Railsware (www.railsware.com)
 */
#include "skypekit.h"
#include "skypekit_internal.h"

SkypekitAccountStatusData* SkypekitAccount::CreateAccountStatusData(int status)
{
  SkypekitAccountStatusData* data;
  SEString skypename;
  Account::LOGOUTREASON reason;
  int payload_size;
  char* payload;

  skypekit_D("SkypekitAccount::CreateAccountStatusData ...\n");

  GetPropSkypename(skypename);

  payload_size = strlen(skypename) + 2;

  skypekit_D("SkypekitAccount::CreateAccountStatusData allocate memory(%d)\n", payload_size);
  SKYPEKIT_ALLOCATE_STRUCT_WITH_PAYLOAD(data, SkypekitAccountStatusData, payload_size)

  payload = data->payload;
  SKYPEKIT_ADD_PAYLOAD(data, payload, skypename, skypename)

  data->status = status;
  if (status == Account::LOGGED_OUT) {
    GetPropLogoutreason(reason);
    data->reason = reason;
  }

  skypekit_D("SkypekitAccount::CreateAccountStatusData OK\n");
  return data;
}

void SkypekitAccount::OnChange(int prop) {
  SkypekitSkype *skype;
  Account::STATUS status;
  SkypekitAccountStatusData* data;

  skypekit_D("SkypekitAccount::OnChange...\n");

  skype = (SkypekitSkype*) this->root;

  switch (prop) {
    case Account::P_STATUS:
      GetPropStatus(status);
      skypekit_D("Account status: %s\n", (const char*)tostring(status));

      if (status == Account::LOGGED_IN) {
        // so that we'll receive messages on that conversations on start
        skype->FetchConversationList();
      }

      data = this->CreateAccountStatusData(status);
      skype->queue->push(data);

      break;

    default:
      // unhandled property change
      skypekit_D("UNSUPPORTED PROPERTY: %d\n", prop);
      break;
  }
}
