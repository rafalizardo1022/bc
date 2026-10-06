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

#ifndef __INSTRUMENT_NETWORK_HPP_INCLUDED__
#define __INSTRUMENT_NETWORK_HPP_INCLUDED__

#include <enet/enet.h>

#include "../InstrumentData.hpp"

class Network
{
public:
    Network(int port);
    ~Network();

    bool update(InstrumentData& instrumentData);
    int getPort() const;
    bool hasReceivedData() const;

private:
    ENetHost* server;
    ENetEvent event;
    bool receivedData;

    bool receiveMessage(InstrumentData& instrumentData);
};

#endif
