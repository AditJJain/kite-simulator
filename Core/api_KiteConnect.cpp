#include <iostream>
#include "Libraries/WebSocket/websocketpp/websocketpp/config/asio_no_tls.hpp"
#include "Libraries/WebSocket/websocketpp/websocketpp/server.hpp"
#include "Libraries/JSON/json.hpp"

using json = nlohmann::json;
using namespace websocketpp;

typedef server<config::asio> server;

class kite_ws_client {
public:
    kite_ws_client(const std::string &access_token, const std::string &api_key)
        : access_token(access_token), api_key(api_key) {
        // Initialize WebSocket client
        ws_server.init_asio();
        ws_server.set_message_handler(std::bind(&kite_ws_client::on_message, this, std::placeholders::_1, std::placeholders::_2));
        ws_server.set_open_handler(std::bind(&kite_ws_client::on_open, this, std::placeholders::_1));
        ws_server.set_close_handler(std::bind(&kite_ws_client::on_close, this, std::placeholders::_1));
    }

    // Connect to WebSocket server
    void connect() {
        // The Kite WebSocket URL (replace with actual URL as needed)
        std::string uri = "wss://ws.kite.trade/";

        // Connect to the WebSocket server
        websocketpp::lib::error_code ec;
        connection_ptr con = ws_server.get_connection(uri, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }

        // Open the WebSocket connection
        ws_server.connect(con);
        ws_server.run();
    }

    // Send subscription request
    void subscribe(const std::vector<int>& tokens) {
        json request;
        request["a"] = "subscribe";
        request["v"] = tokens;

        std::string msg = request.dump();
        ws_server.send(hdl, msg, websocketpp::frame::opcode::text);
    }

private:
    // Called when a message is received
    void on_message(connection_hdl hdl, message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
        // You can process the message here (parse the ticks, etc.)
    }

    // Called when WebSocket is opened
    void on_open(connection_hdl hdl) {
        std::cout << "WebSocket connected" << std::endl;
        this->hdl = hdl;

        // Example: Subscribe to the NIFTY50 index (token 256265)
        subscribe({256265});
    }

    // Called when WebSocket is closed
    void on_close(connection_hdl hdl) {
        std::cout << "WebSocket closed" << std::endl;
    }

    server ws_server;
    connection_hdl hdl;
    std::string access_token;
    std::string api_key;
};

int main() {
    // Replace with your access token and API key
    std::string access_token = "your_access_token";
    std::string api_key = "your_api_key";

    kite_ws_client client(access_token, api_key);
    client.connect();

    return 0;
}