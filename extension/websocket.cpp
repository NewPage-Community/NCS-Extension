#include "extension.h"

#include "websocket.h"
#include "rapidjson.h"
#include "native.h"

// Async times per tick
#define ASYNCTIMES 50

// WebSocket
WebSocket* g_websocket;

// Send Queue
std::queue<std::string> g_MsgSendQueue;
// Rev Queue
std::queue<std::string> g_MsgRevQueue;

// Connected forward
bool g_ConnectedCall = false;
// Disconnected forward
bool g_DisconnectedCall = false;

// Forward
IForward* g_OnReceivedMsg = nullptr;
IForward* g_OnConnected = nullptr;
IForward* g_OnDisconnected = nullptr;

void WS_Open()
{
	g_pSM->LogMessage(myself, "WebSocket server connected!");
	g_ConnectedCall = true;
}

void WS_Fail(const char* error)
{
	g_pSM->LogError(myself, "WebSocket connection faild! Error: %s", error);

	// Reconnect after 5s
	timersys->CreateTimer(new WebSocketReconnect(), 5.0, nullptr, NULL);
}

void WS_Close(const char* reason)
{
	g_pSM->LogMessage(myself, "WebSocket connection closed! Reason: %s", reason);

	// Reconnect after 5s
	timersys->CreateTimer(new WebSocketReconnect(), 5.0, nullptr, NULL);

	g_DisconnectedCall = true;
}

void WS_Msg(const char* msg)
{
	g_MsgRevQueue.push(msg);
}

void WS_Error(const char* error)
{
	g_pSM->LogError(myself, "Websocket Errro: %s", error);
}

void StartWebSocket()
{
	// Create WebSocket
	g_websocket = new WebSocket(WS_Error, WS_Open, WS_Fail, WS_Close, WS_Msg);
	g_websocket->connect("127.0.0.1:1234");

	// Add Forward
	g_OnReceivedMsg = forwards->CreateForward("NCS_OnReceivedMsg", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	g_OnConnected = forwards->CreateForward("NCS_OnConnected", ET_Ignore, 0, NULL);
	g_OnDisconnected = forwards->CreateForward("NCS_OnDisconnected", ET_Ignore, 0, NULL);
}

void StopWebSocket()
{
	delete& g_websocket; //delete websocket connection

	forwards->ReleaseForward(g_OnReceivedMsg);
	forwards->ReleaseForward(g_OnConnected);
	forwards->ReleaseForward(g_OnDisconnected);
}

void WebSocketAsyncCheck()
{
	std::string msg;

	if (g_ConnectedCall)
	{
		g_ConnectedCall = false;
		g_OnConnected->Execute(nullptr);
	}

	if (g_DisconnectedCall)
	{
		g_DisconnectedCall = false;
		g_OnDisconnected->Execute(nullptr);
	}
	

	for (int i = 0; i < ASYNCTIMES && !g_MsgRevQueue.empty(); i++)
	{
		msg = g_MsgRevQueue.front();

		if (msg.empty())
			continue;

		rapidjson::Document document;
		if (document.Parse(msg.c_str()).HasParseError() || !document.HasMember("Router") || !document.HasMember("Msg"))
			continue;
		if (!document["Router"].IsString())
			continue;

		Handle_t hdl = GetMsgHandle(new MsgRevJson(document["Msg"]));

		// Call Forward with json handle
		g_OnReceivedMsg->PushString(document["Router"].GetString());
		g_OnReceivedMsg->PushCell(static_cast<cell_t>(hdl));
		g_OnReceivedMsg->Execute(nullptr);

		g_MsgRevQueue.pop();
		FreeHandle(hdl); // Free handle when called forward
	}

	for (int i = 0; i < ASYNCTIMES && !g_MsgSendQueue.empty() && g_websocket->IsConnected(); i++)
	{
		g_websocket->send(g_MsgSendQueue.front());
		g_MsgSendQueue.pop();
	}
}

bool SendQuery(std::string str)
{
	if (g_websocket->IsConnected())
	{
		g_MsgSendQueue.push(str);
		return true;
	}
		
	return false;
}

ResultType WebSocketReconnect::OnTimer(ITimer* pTimer, void* pData)
{
	g_pSM->LogMessage(myself, "Reconnect WebSocket server ...");

	// Create WebSocket
	g_websocket = new WebSocket(WS_Error, WS_Open, WS_Fail, WS_Close, WS_Msg);
	g_websocket->connect("127.0.0.1:1234");

	return Pl_Stop;
}