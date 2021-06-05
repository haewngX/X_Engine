#pragma once
#include "zmq.hpp"
#include "json.hpp"

using json = nlohmann::json;

enum MessageType
{
	Start,
	Obs_REQ,
	Action_REP,
	SingleGameOver,
	Finish,
	ReqError
};

class Zmq
{
public:
	Zmq();
private:
	zmq::context_t mContext;
	zmq::socket_t mSocket;
};
class ZmqClient : public Zmq
{
public:
	ZmqClient();
	json RecvMsg(zmq::message_t& msg_rec);
	void SendMsg(int msg_type, std::string msg_content);
private:
	zmq::context_t mContext;
	zmq::socket_t mSocket;
};
