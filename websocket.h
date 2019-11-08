#ifndef NCS_WEBSOCKET_H
#define NCS_WEBSOCKET_H

//Websocketpp
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> ws_client;

class WebSocket
{
public:
	typedef void (*ErrorHandler)(const char *);
	typedef void(*OpenHandler)();
	typedef void(*FailHandler)(const char *);
	typedef void(*CloseHandler)(const char *);
	typedef void(*MsgHandler)(const char *);

	WebSocket(ErrorHandler hdl1, OpenHandler hdl2, FailHandler hdl3, CloseHandler hdl4, MsgHandler hdl5)
		: m_errorhandler(hdl1)
		, m_openhandler(hdl2)
		, m_failhandler(hdl3)
		, m_closehandler(hdl4)
		, m_msghandler(hdl5)
		, m_connected(false)
	{
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.init_asio();
		m_endpoint.start_perpetual();

		m_endpoint.set_message_handler(websocketpp::lib::bind(&WebSocket::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_open_handler(websocketpp::lib::bind(&WebSocket::on_open, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_close_handler(websocketpp::lib::bind(&WebSocket::on_close, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_fail_handler(websocketpp::lib::bind(&WebSocket::on_fail, this, websocketpp::lib::placeholders::_1));

		m_thread.reset(new websocketpp::lib::thread(&ws_client::run, &m_endpoint));
	}

	void connect(std::string const & uri)
	{
		websocketpp::lib::error_code ec;

		ws_client::connection_ptr con = m_endpoint.get_connection("ws://" + uri, ec);
		if (ec)
		{
			(*m_errorhandler)(ec.message().data());
			return;
		}

		con->append_header("Origin", "http://" + uri);

		m_endpoint.connect(con);
	}

	void close(websocketpp::close::status::value code)
	{
		if (m_connected)
        {
            websocketpp::lib::error_code ec;
            m_endpoint.close(m_hdl, code, "", ec);
            if (ec)
			    (*m_errorhandler)(ec.message().data());
        }
	}

	void send(std::string message)
	{
		if (m_connected)
        {
            websocketpp::lib::error_code ec;
            m_endpoint.send(m_hdl, message, websocketpp::frame::opcode::text);
            if (ec)
                (*m_errorhandler)(ec.message().data());
        }
	}

	void on_open(websocketpp::connection_hdl hdl)
	{
		m_hdl = hdl;
		m_connected = true;
		(*m_openhandler)();
	}

	void on_fail(websocketpp::connection_hdl hdl)
	{
		m_connected = false;
		ws_client::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::stringstream s;
		s << "Error msg: " << con->get_ec().message() << " HTTP response code: " << con->get_response_code() << " HTTP response msg: " << con->get_response_msg();
		(*m_failhandler)(s.str().data());
	}

	void on_close(websocketpp::connection_hdl hdl)
	{
		m_connected = false;
		ws_client::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
			<< websocketpp::close::status::get_string(con->get_remote_close_code())
			<< "), close reason: " << con->get_remote_close_reason();
		(*m_closehandler)(s.str().data());
	}

	void on_message(websocketpp::connection_hdl hdl, ws_client::message_ptr msg)
	{
		(*m_msghandler)(msg->get_payload().data());
	}

	bool IsConnected()
	{
		return m_connected;
	}

	~WebSocket()
	{
		m_connected = false;
		websocketpp::lib::error_code ec;
		m_endpoint.stop_perpetual();
		if (m_connected)
			m_endpoint.close(m_hdl, websocketpp::close::status::going_away, "", ec);
		m_thread->join();

		if (ec)
			(*m_errorhandler)(ec.message().data());
	}
private:
	ws_client m_endpoint;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

	websocketpp::connection_hdl m_hdl;

	ErrorHandler m_errorhandler;
	OpenHandler m_openhandler;
	FailHandler m_failhandler;
	CloseHandler m_closehandler;
	MsgHandler m_msghandler;

	bool m_connected;
};

class WebSocketReconnect : public ITimedEvent
{
public:
	WebSocketReconnect() {}
	ResultType OnTimer(ITimer* pTimer, void* pData);
	void OnTimerEnd(ITimer* pTimer, void* pData) {}
};

void StartWebSocket();
void StopWebSocket();
bool SendQuery(std::string str);
void WebSocketAsyncCheck();

#endif