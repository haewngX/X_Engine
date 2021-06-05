#include "Zmq.h"

Zmq::Zmq()
{

}

ZmqClient::ZmqClient()
	:mContext(1)
	, mSocket(mContext, ZMQ_REQ)
{
	mSocket.connect("tcp://localhost:6324");
}

json ZmqClient::RecvMsg(zmq::message_t& msg_rec)
{
	mSocket.recv(msg_rec, zmq::recv_flags::none);
	std::string str = std::string(static_cast<char*>(msg_rec.data()), msg_rec.size());
	json recv_msg = json::parse(str);
	return recv_msg;
}

void ZmqClient::SendMsg(int msg_type, std::string msg_content)
{
	json message;
	message["type"] = msg_type;
	message["content"] = msg_content;
	
	std::string msg_str = message.dump();
	zmq::message_t reply(msg_str.size());
	memcpy(reply.data(), msg_str.c_str(), msg_str.size());
	mSocket.send(reply, zmq::send_flags::none);
}
