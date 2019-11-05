#include "extension.h"

#include "native.h"
#include "websocket.h"

// Handle
HandleType_t g_MsgRevType = 0;
HandleType_t g_MsgSendType = 0;

MsgRevHandler g_MsgRevTypeHandler;
MsgSendHandler g_MsgSendTypeHandler;

void NativeLoad()
{
	sharesys->AddNatives(myself, NCSNatives);

	g_MsgRevType = g_pHandleSys->CreateType("NCSMsgRev", &g_MsgRevTypeHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	g_MsgSendType = g_pHandleSys->CreateType("NCSMsgSend", &g_MsgSendTypeHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
}

void NativeUnload()
{
	g_pHandleSys->RemoveType(g_MsgRevType, myself->GetIdentity());
	g_pHandleSys->RemoveType(g_MsgSendType, myself->GetIdentity());
}

Handle_t GetMsgHandle(MsgRevJson* obj)
{
	return g_pHandleSys->CreateHandle(g_MsgRevType, obj, NULL, myself->GetIdentity(), NULL);
}

void FreeHandle(Handle_t hdl)
{
	g_pHandleSys->FreeHandle(hdl, new HandleSecurity(NULL, myself->GetIdentity()));
}

MsgRevJson* MsgRevJson::GetFromHandle(cell_t hdl, IPluginContext* pContext)
{
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	MsgRevJson* obj;
	HandleError err = g_pHandleSys->ReadHandle(static_cast<Handle_t>(hdl), g_MsgRevType, &sec, (void**)& obj);
	if (err != HandleError_None)
	{
		pContext->ThrowNativeError("Invalid <Object> (error %d)", err);
		return nullptr;
	}

	return obj;
}

MsgSendJson* MsgSendJson::GetFromHandle(cell_t hdl, IPluginContext* pContext)
{
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	MsgSendJson* obj;
	HandleError err = g_pHandleSys->ReadHandle(static_cast<Handle_t>(hdl), g_MsgSendType, &sec, (void**)& obj);
	if (err != HandleError_None)
	{
		pContext->ThrowNativeError("Invalid <Object> (error %d)", err);
		return nullptr;
	}

	return obj;
}

// NCSMsgRev(char[] json)
static cell_t Msg_Read(IPluginContext* pContext, const cell_t* params)
{
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	char* json;
	pContext->LocalToString(params[1], &json);

	rapidjson::Document document;
	if (document.Parse(json).HasParseError() || !document.IsObject())
		return BAD_HANDLE;

	Handle_t hndl = g_pHandleSys->CreateHandleEx(g_MsgRevType, new MsgRevJson(document, document.GetAllocator()), &sec, NULL, NULL);

	if (hndl == BAD_HANDLE)
		pContext->ThrowNativeError("Could not create <Object> handle.");

	return hndl;
}

// GetInt(char[] key = "")
static cell_t Json_GetInt(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	// Return
	int i = -1;
	obj->GetInt(i, key);

	return i;
}

// GetString(char[] str, int size, char[] key = "")
static cell_t Json_GetString(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == NULL)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[4], &key);

	// Return
	std::string str;
	obj->GetString(str, key);
	pContext->StringToLocalUTF8(params[2], params[3], str.data(), NULL);
	return str.length();

	return -1;
}

// GetFloat(char[] key = "")
static cell_t Json_GetFloat(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	// Return
	double d = -1.0;
	obj->GetDouble(d, key);

	return sp_ftoc((float)d);
}

// GetArray(int index, char[] key = "")
static cell_t Json_GetArray(IPluginContext * pContext, const cell_t * params)
{
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());

	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[3], &key);

	// Return
	MsgRevJson * arr = obj->GetArray(key, (int)params[2]);
	if (arr == nullptr)
		return pContext->ThrowNativeError("<Object> is not array or not exist.");

	Handle_t hndlObject = g_pHandleSys->CreateHandleEx(g_MsgRevType, arr, &sec, NULL, NULL);

	if (hndlObject == BAD_HANDLE)
		pContext->ThrowNativeError("Could not create <Object> handle.");

	return hndlObject;
}

// GetArraySize(char[] key = "")
static cell_t Json_GetArraySize(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	// Return
	int i = -1;
	obj->GetArraySize(i, key);

	return i;
}

// GetObject(char[] key = "")
static cell_t Json_GetObject(IPluginContext * pContext, const cell_t * params)
{
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	// Return
	MsgRevJson * obj1 = obj->GetJObject(key);
	if (obj1 == nullptr)
		return pContext->ThrowNativeError("<Object> is not object or not exist.");

	Handle_t hndlObject = g_pHandleSys->CreateHandleEx(g_MsgRevType, obj1, &sec, NULL, NULL);

	if (hndlObject == BAD_HANDLE)
		pContext->ThrowNativeError("Could not create <Object> handle.");

	return hndlObject;
}

// GetJson(char[] str, int size)
static cell_t Json_GetJson(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Return
	std::string str = obj->GetJson();
	if (str.empty())
		return pContext->ThrowNativeError("Invalid <Object>.");

	pContext->StringToLocalUTF8(params[2], params[3], str.c_str(), NULL);
	return str.length();
}

// GetBool(char[] key = "")
static cell_t Json_GetBool(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgRevJson* obj = MsgRevJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	// Return
	bool b = false;
	obj->GetBool(b, key);

	return b;
}

// NCSMsgSend(char[] Router)
static cell_t Msg_Init(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	char* key;
	pContext->LocalToString(params[1], &key);

	if (!strcmp(key, ""))
		return pContext->ThrowNativeError("Router can not be empty.");

	Handle_t hndl = g_pHandleSys->CreateHandle(g_MsgSendType, new MsgSendJson(key), pContext->GetIdentity(), myself->GetIdentity(), NULL);

	if (hndl == BAD_HANDLE)
		pContext->ThrowNativeError("Could not create <Object> handle.");

	return hndl;
}

// SendMsg()
static cell_t Msg_Send(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	return SendQuery(obj->GetMsg());
}

// Int(int i, char[] key = "")
static cell_t Msg_Int(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[3], &key);

	if (!strcmp(key, ""))
		obj->Int(params[2]);
	else
		obj->Int(key, params[2]);

	return 1;
}

// String(char[] str, char[] key = "")
static cell_t Msg_String(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key, *str;
	pContext->LocalToString(params[2], &str);
	pContext->LocalToString(params[3], &key);

	if (!strcmp(key, ""))
		obj->String(str);
	else
		obj->String(key, str);

	return 1;
}

// Float(float f, char[] key = "")
static cell_t Msg_Float(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[3], &key);

	if (!strcmp(key, ""))
		obj->Double(sp_ctof(params[2]));
	else
		obj->Double(key, sp_ctof(params[2]));

	return 1;
}

// StartArray(char[] key)
static cell_t Msg_StartArray(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	if (!strcmp(key, ""))
		return pContext->ThrowNativeError("Array key can not be empty.");

	obj->StartArray(key);

	return 1;
}

// EndArray()
static cell_t Msg_EndArray(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	obj->EndArray();

	return 1;
}

// StartObject(char[] key = "")
static cell_t Msg_StartObject(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[2], &key);

	if (!strcmp(key, ""))
		obj->StartObject();
	else
		obj->StartObject(key);

	return 1;
}

// EndObject()
static cell_t Msg_EndObject(IPluginContext * pContext, const cell_t * params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	obj->EndObject();

	return 1;
}

// Bool(bool b, char[] key = "")
static cell_t Msg_Bool(IPluginContext* pContext, const cell_t* params)
{
	// Param 1
	MsgSendJson* obj = MsgSendJson::GetFromHandle(params[1], pContext);

	if (obj == nullptr)
		return BAD_HANDLE;

	// Param 2
	char* key;
	pContext->LocalToString(params[3], &key);

	if (!strcmp(key, ""))
		obj->Bool(params[2]);
	else
		obj->Bool(key, params[2]);

	return 1;
}

const sp_nativeinfo_t NCSNatives[] = {
	{"NCSMsgRev.NCSMsgRev", Msg_Read},
	{"NCSMsgRev.GetInt", Json_GetInt},
	{"NCSMsgRev.GetString", Json_GetString},
	{"NCSMsgRev.GetFloat", Json_GetFloat},
	{"NCSMsgRev.GetArray", Json_GetArray},
	{"NCSMsgRev.GetArraySize", Json_GetArraySize},
	{"NCSMsgRev.GetObject", Json_GetObject},
	{"NCSMsgRev.GetJson", Json_GetJson},
	{"NCSMsgRev.GetBool", Json_GetBool},
	{"NCSMsgSend.NCSMsgSend", Msg_Init},
	{"NCSMsgSend.SendMsg", Msg_Send},
	{"NCSMsgSend.Int", Msg_Int},
	{"NCSMsgSend.String", Msg_String},
	{"NCSMsgSend.Float", Msg_Float},
	{"NCSMsgSend.Bool", Msg_Bool},
	{"NCSMsgSend.StartArray", Msg_StartArray},
	{"NCSMsgSend.EndArray", Msg_EndArray},
	{"NCSMsgSend.StartObject", Msg_StartObject},
	{"NCSMsgSend.EndObject", Msg_EndObject},
	{NULL, NULL}
};