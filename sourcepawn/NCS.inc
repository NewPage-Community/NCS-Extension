#if defined _NCS_Include_
	#endinput
#endif

#define _NCS_Include

#pragma newdecls required

/* ----- NCS Ext APIs ----- */

// NCS's Message (Read JSON) NEED DELETE AFTER USED!!!
methodmap NCSMsgRev < Handle
{
	public native NCSMsgRev(const char[] json); // Read Message
	// return NCSMsgRev

	// Data
	public native int GetInt(const char[] key = "");
	public native int GetString(char[] str, int size, const char[] key = ""); //Return: str length (invaild = -1)
	public native int GetArraySize(const char[] key = "");
	public native int GetJson(char[] str, int size);

	public native float GetFloat(const char[] key = "");

	public native bool GetBool(const char[] key = "");

	public native NCSMsgRev GetArray(int index, const char[] key = "");
	public native NCSMsgRev GetObject(const char[] key = "");
};

// NCS's Message (Write JSON) NEED DELETE AFTER SEND!!!
methodmap NCSMsgSend < Handle
{
	public native NCSMsgSend(const char[] router); // Init Message to send
	// return NCSMsgSend

	public native bool SendMsg(); // Send Message to backend

	// Import Data
	public native void Int(int i, const char[] key = "");
	public native void String(const char[] str, const char[] key = "");
	public native void Float(float f, const char[] key = "");
	public native void Bool(bool b, char[] key = "");
	public native void StartArray(const char[] key);
	public native void EndArray();
	public native void StartObject(const char[] key = "");
	public native void EndObject();
};

// This forward do not need to delete NCSMsgRev handle(Auto Free)
forward void NCS_OnReceivedMsg(const char[] router, NCSMsgRev msg);

// Call when backend is connected
forward void NCS_OnConnected();

// Call when backend is disconnected
forward void NCS_OnDisconnected();

// SourceMOD shared plugin settings
public Extension __ext_NCS = 
{
	name = "NCS",
	file = "NCS.ext",
	autoload = 1,
	required = 1,
};