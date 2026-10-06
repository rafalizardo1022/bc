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

#include "EventReceiver.hpp"

#include "InstrumentGUI.hpp"

EventReceiver::EventReceiver(irr::IrrlichtDevice* device, InstrumentGUI* gui)
{
    this->device = device;
    this->gui = gui;
}

bool EventReceiver::OnEvent(const irr::SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
        if (event.KeyInput.Key == irr::KEY_ESCAPE || event.KeyInput.Key == irr::KEY_F4) {
            device->closeDevice();
            return true;
        }
        if (event.KeyInput.Key == irr::KEY_F1) {
            gui->setMode(InstrumentDisplayVessel);
            return true;
        }
        if (event.KeyInput.Key == irr::KEY_F2) {
            gui->setMode(InstrumentDisplayNavigation);
            return true;
        }
        if (event.KeyInput.Key == irr::KEY_TAB) {
            gui->toggleMode();
            return true;
        }
    }

    return false;
}
