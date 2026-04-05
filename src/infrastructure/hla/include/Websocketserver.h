
#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "ThreadPool.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <atomic>
#include <cmath>

#include "../../protobuf/c++/entity.pb.h"

using websocketpp::connection_hdl;
using server = websocketpp::server<websocketpp::config::asio>;

namespace SDL_RTI {

    class ClientPitchWebSocket;
    
    class Websocketserver
    {
        public:

        Websocketserver(ClientPitchWebSocket* clientPitch);
        ~Websocketserver();
        void enqueueSend(const nlohmann::json& msg);
        void enqueueSend(const Envelope& envelope);
        std::atomic<bool> statusConnection;

        void on_open(server* s, connection_hdl hdl);
        void on_message(server* s, connection_hdl hdl, server::message_ptr msg);
        void on_close(server* s, connection_hdl hdl);

        void initServer(void);
        void startInBackground();

        private:

        ClientPitchWebSocket* m_ClientPitch;

        std::mutex mtx;
        std::condition_variable cv;

        std::mutex send_mutex;

        server print_server;

        std::thread serverThread;

        std::shared_ptr<SDL_RTI::ThreadPool> poolOfThread;

        std::unique_ptr<connection_hdl> active_hdl;
    };
}
#endif // WEBSOCKETSERVER_H
