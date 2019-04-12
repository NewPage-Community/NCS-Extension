#define REQUIRE_EXTENSIONS

#include <sourcemod>
#include <NCS>
#include <cstrike>

public Plugin myinfo = 
{
	name        = "NCS TEST",
	author      = "Gunslinger",
	description = "For test",
	version     = "0.1",
	url         = "https://new-page.xyz"
};

public void NCS_OnReceivedMsg(const char[] router, NCSMsgRev msg)
{
	int j;
	float f;
	char str[512];

	j = msg.GetInt("int")
	NCSMsgRev msgObj = msg.GetObject("NCS");
	msgObj.GetString(str, 512, "string");
	f = msgObj.GetFloat("float");
	for (int i = 0; i < msgObj.GetArraySize("array"); i++)
		j = msgObj.GetArray(i, "array").GetInt();
	msg.GetJson(str, 512);
	
	delete msgObj;
	delete msg;
}

public void OnPluginStart()
{
	RegServerCmd("ncs_test", TestCallback, "");
}

Action TestCallback(int args)
{
	float t1 = GetEngineTime();

	for (int i = 0; i < 10000; i++)
	{
		NCSMsgSend msg = new NCSMsgSend("Test");

		msg.Int(1, "int");
		msg.StartObject("NCS");
		msg.String("Hello", "string");
		msg.Float(2.0, "float");
		msg.StartArray("array");
		msg.Int(1);
		msg.Int(2);
		msg.Int(3);
		msg.EndArray();
		msg.EndObject();

		msg.SendMsg()

		delete msg;
	}
	
	PrintToServer("[NCS TEST] Test end! (%f)", GetEngineTime() - t1);
}