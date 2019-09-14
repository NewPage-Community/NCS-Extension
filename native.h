#ifndef NCS_NATIVE_H
#define NCS_NATIVE_H

#include "rapidjson.h"

void NativeLoad();
void NativeUnload();
Handle_t GetMsgHandle(MsgRevJson* obj);
void FreeHandle(Handle_t hdl);

#endif