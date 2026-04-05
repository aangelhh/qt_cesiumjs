#include "../include/Websocketserver.h"
#include "../ClientPitchWebSocket.h"


using nlohmann::json;
using namespace SDL_RTI;


Websocketserver::Websocketserver(ClientPitchWebSocket* clientPitch) 
    : m_ClientPitch(clientPitch), statusConnection(false)
{
    poolOfThread = std::make_shared<ThreadPool>(8);
    active_hdl = nullptr;
}

Websocketserver::~Websocketserver() {
    if (serverThread.joinable()) {
        serverThread.join(); // Espera a que el hilo termine antes de destruir el objeto
    }
}

void Websocketserver::enqueueSend(const nlohmann::json& msg) {

    if(statusConnection)
    {
        poolOfThread->enqueue([this, msg]() {
            std::lock_guard<std::mutex> lock(send_mutex);
            try {
                print_server.send(*(active_hdl.get()), msg.dump(), websocketpp::frame::opcode::text);
            } catch (const websocketpp::exception& e) {
                std::cerr << "Error al enviar: " << e.what() << std::endl;
            }
        });
    }
    else
    {
        std::cout << "No conexión disponible" << std::endl;
    }
}

void Websocketserver::enqueueSend(const Envelope& envelope) {

    if (statusConnection && active_hdl)
    {
        poolOfThread->enqueue([this, envelope]() {
            std::lock_guard<std::mutex> lock(send_mutex);
            try {
                
                std::string buffer;
                envelope.SerializeToString(&buffer);

                print_server.send(*(active_hdl.get()), buffer, websocketpp::frame::opcode::binary);
            } catch (const websocketpp::exception& e) {
                std::cerr << "Error al enviar Protobuf: " << e.what() << std::endl;
            }
        });
    }
    else
    {
        std::cout << "No conexión disponible para enviar Protobuf" << std::endl;
    }
}

void Websocketserver::on_open(server* s, connection_hdl hdl) {
    std::cout << "Nueva conexión establecida" << std::endl;
    statusConnection = true;
    active_hdl = std::make_unique<connection_hdl>(hdl);
}

void Websocketserver::on_message(server* s, connection_hdl hdl, server::message_ptr msg) 
{
    std::cout << "Mensaje recibido (binario, tamaño " << msg->get_payload().size() << ")" << std::endl;

    try 
    {
        Envelope envelope;
        if (!envelope.ParseFromString(msg->get_payload())) {
            std::cerr << "Error: no se pudo parsear el mensaje Protobuf." << std::endl;
            return;
        }

        // Procesar según el tipo de payload recibido
        if (envelope.has_create_entity_magic_action()) {
            const auto& action = envelope.create_entity_magic_action();

            std::cout << "has_create_entity_magic_action"<< std::endl;
            std::cout << action.DebugString() << std::endl;

            if (m_ClientPitch) {
                 m_ClientPitch->handleMagicAction(action);
            }
        }
        else if (envelope.has_remove_entity_magic_action()) {
            const auto& action = envelope.remove_entity_magic_action();

            std::cout << "has_remove_entity_magic_action"<< std::endl;
            std::cout << action.DebugString() << std::endl;

            if (m_ClientPitch) {
                 m_ClientPitch->handleRemoveEntityMagicAction(action);
            }
        }
        else if (envelope.has_destroy_entity()) {
            const auto& action = envelope.destroy_entity();

            std::cout << "has_destroy_entity"<< std::endl;
            std::cout << action.DebugString() << std::endl;

            if (m_ClientPitch) {
                 m_ClientPitch->handleDestroyEntityMagicAction(action);
            }
        }
        else if (envelope.has_magic_move_entity()) {
            const auto& action = envelope.magic_move_entity();

            std::cout << "has_magic_move_entity"<< std::endl;
            std::cout << action.DebugString() << std::endl;

            if (m_ClientPitch) {
                 m_ClientPitch->handleMagicMoveAction(action);
            }
        }
        else if (envelope.has_base_entity()) {
            const auto& base = envelope.base_entity();
            // Procesar base_entity si lo necesitas
        }
        else if (envelope.has_federate()) {
            const auto& federate = envelope.federate();
            // Procesar federate si lo necesitas
        }
        else if (envelope.has_interaction()) {
            const auto& interaction = envelope.interaction();

            switch (interaction.type()) {
            case InteractionProto::PLAY:
                // Es el comando PLAY (valor 1)
                std::cout << "Comando de Interacción Recibido: PLAY" << std::endl;
                m_ClientPitch->handlePlayResume();
                break;

            case InteractionProto::PAUSE:
                // Es el comando PAUSE (valor 2)
                std::cout << "Comando de Interacción Recibido: PAUSE" << std::endl;
                m_ClientPitch->handlePauseFreeze();
                break;

            case InteractionProto::STOP:
                // Es el comando STOP (valor 3)
                std::cout << "Comando de Interacción Recibido: STOP" << std::endl;
                m_ClientPitch->handleStop();
                break;

            case InteractionProto::UNKNOWN:
            default:
                // Si el valor es 0 o un valor desconocido
                std::cerr << "Comando de Interacción Recibido: DESCONOCIDO o UNKNOWN" << std::endl;
                break;
            }
        }
        else if (envelope.has_requesthla()) {
        const auto& action = envelope.requesthla();

            std::cout << "Hla Proto received"<< std::endl;
            std::cout << action.DebugString() << std::endl;

            if(action.connect())
            {
                m_ClientPitch->initConnect(action);
            }
            else
            {
                m_ClientPitch->finishConnect();
            }
            
        }
        else {
            std::cerr << "Envelope recibido sin payload reconocido." << std::endl;
        }
    } catch (const websocketpp::exception& e) {
        std::cerr << "Error al enviar mensaje: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Excepción general al enviar mensaje: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Excepción desconocida al enviar mensaje." << std::endl;
    }
}

void Websocketserver::on_close(server* s, connection_hdl hdl) {
    std::cout << "Conexión cerrada" << std::endl;
    statusConnection = false;
    active_hdl = nullptr;
    m_ClientPitch->finishConnect();
}

void Websocketserver::initServer() {

    print_server.set_open_handler(
    [this](connection_hdl hdl) {
        this->on_open(&this->print_server, hdl);
    }
    );

    print_server.set_close_handler(
        [this](connection_hdl hdl) {
            this->on_close(&this->print_server, hdl);
        }
    );

    print_server.set_message_handler(
        [this](connection_hdl hdl, server::message_ptr msg) {
            this->on_message(&this->print_server, hdl, msg);
        }
    );


    std::cout << "Configurando el servidor para escuchar en 0.0.0.0:9002" << std::endl;
    print_server.init_asio();
    print_server.set_reuse_addr(true);
    print_server.listen(9002);
    print_server.start_accept();

    try {
       startInBackground();
    } catch (const websocketpp::exception& e) {
        std::cerr << "Excepción del servidor: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Excepción general: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Excepción desconocida." << std::endl;
    }
}

void Websocketserver::startInBackground() {
    serverThread = std::thread([this]() {
        print_server.run();
    });
}

