#if defined _NewPage_Core_Include_
	#endinput
#endif

#define _NewPage_Core_Include

methodmap NCSMsgRev < Handle
{
    public native NCSMsgRev(const char[] json); //ReadMsg

    public native int GetInt(const char[] key = "");
    public native void GetString(char[] str, int size, const char[] key = "");
    public native float GetFloat(const char[] key = "");
    public native bool GetBool(const char[] key = "");
    public native NCSMsgRev GetArray(int index, const char[] key = "");
    public native int GetArraySize(const char[] key = "");
    public native NCSMsgRev GetObject(const char[] key = "");
    public native int GetJson(char[] str, int size);
};

methodmap NCSMsgSend < Handle
{
    public native NCSMsgSend(const char[] router); //InitMsg
    public native bool SendMsg();

    public native void Int(int i, const char[] key = "");
    public native void String(const char[] str, const char[] key = "");
    public native void Float(float f, const char[] key = "");
	public native void Bool(bool b, char[] key = "");
    public native void StartArray(const char[] key);
    public native void EndArray();
    public native void StartObject(const char[] key = "");
    public native void EndObject();
};

forward void NCS_OnReceivedMsg(const char[] router, NCSMsgRev msg);
forward void NCS_OnConnected();
forward void NCS_OnDisconnected();

public Extension __ext_NCS = 
{
	name = "NCS",
	file = "NCS.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
