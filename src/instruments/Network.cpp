/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2026

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License version 2 as
     published by the Free Software Foundation

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY Or FITNESS For A PARTICULAR PURPOSE.  See the
     GNU General Public License For more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "Network.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "../InstrumentNetworkCodec.hpp"

Network::Network(int port)
{
    server = 0;
    receivedData = false;

    if (enet_initialize() != 0)
    {
        std::cout << "An error occurred while initializing ENet.\n";
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;

    int tries = 0;
    while (server == NULL && tries < 10) {
        address.port = port;
        server = enet_host_create(&address,
            32,
            0,
            0,
            0);

        if (server == NULL) {
            tries++;
            port++;
        }
    }

    if (server == NULL)
    {
        std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Instrument display listening on UDP port " << server->address.port << std::endl;
    }
}

Network::~Network()
{
    enet_host_destroy(server);
    enet_deinitialize();
}

bool Network::update(InstrumentData& instrumentData)
{
    bool shutdownRequested = false;

    while (enet_host_service(server, &event, 10) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                shutdownRequested = receiveMessage(instrumentData) || shutdownRequested;
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                event.peer->data = NULL;
                break;
            default:
                break;
        }
    }

    return shutdownRequested;
}

int Network::getPort() const
{
    if (server) {
        return server->address.port;
    }
    return 0;
}

bool Network::hasReceivedData() const
{
    return receivedData;
}

bool Network::receiveMessage(InstrumentData& instrumentData)
{
    char tempString[8192];
    snprintf(tempString, 8192, "%s", event.packet->data);
    std::string receivedString(tempString);

    if (receivedString.length() >= 2 && receivedString.substr(0, 2) == "SD") {
        return true;
    }

    InstrumentData parsed;
    if (InstrumentNetworkCodec::deserialize(receivedString, parsed)) {
        instrumentData = parsed;
        receivedData = true;
    }

    return false;
}
