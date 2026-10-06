/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2015 James Packer

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

#ifndef __EVENTRECEIVER_HPP_INCLUDED__
#define __EVENTRECEIVER_HPP_INCLUDED__

#include "irrlicht.h"
#include <vector>

//forward declarations
class GUIMain;
class ControllerModel;
class Network;

class WeatherJoystickSetup {
public:
    WeatherJoystickSetup();

    bool enabled;

    irr::u32 joystickNoIncreaseWeather;
    irr::u32 joystickButtonIncreaseWeather;
    irr::u32 joystickNoDecreaseWeather;
    irr::u32 joystickButtonDecreaseWeather;

    irr::u32 joystickNoIncreaseRain;
    irr::u32 joystickButtonIncreaseRain;
    irr::u32 joystickNoDecreaseRain;
    irr::u32 joystickButtonDecreaseRain;

    irr::u32 joystickNoIncreaseVisibility;
    irr::u32 joystickButtonIncreaseVisibility;
    irr::u32 joystickNoDecreaseVisibility;
    irr::u32 joystickButtonDecreaseVisibility;

    irr::u32 joystickNoIncreaseWindDirection;
    irr::u32 joystickButtonIncreaseWindDirection;
    irr::u32 joystickNoDecreaseWindDirection;
    irr::u32 joystickButtonDecreaseWindDirection;

    irr::u32 joystickNoIncreaseWindSpeed;
    irr::u32 joystickButtonIncreaseWindSpeed;
    irr::u32 joystickNoDecreaseWindSpeed;
    irr::u32 joystickButtonDecreaseWindSpeed;

    irr::u32 joystickNoIncreaseStreamDirection;
    irr::u32 joystickButtonIncreaseStreamDirection;
    irr::u32 joystickNoDecreaseStreamDirection;
    irr::u32 joystickButtonDecreaseStreamDirection;

    irr::u32 joystickNoIncreaseStreamSpeed;
    irr::u32 joystickButtonIncreaseStreamSpeed;
    irr::u32 joystickNoDecreaseStreamSpeed;
    irr::u32 joystickButtonDecreaseStreamSpeed;

    irr::u32 joystickNoToggleStreamOverride;
    irr::u32 joystickButtonToggleStreamOverride;

    irr::s32 weatherStep;
    irr::s32 rainStep;
    irr::s32 visibilityStep;
    irr::s32 windDirectionStep;
    irr::s32 windSpeedStep;
    irr::s32 streamDirectionStep;
    irr::s32 streamSpeedStep;
};

class EventReceiver : public irr::IEventReceiver
{
public:

    EventReceiver(irr::IrrlichtDevice* device, ControllerModel* model, GUIMain* gui, Network* network, WeatherJoystickSetup weatherJoystickSetup);
    bool OnEvent(const irr::SEvent& event);

    bool isMouseDown() const;

private:

    bool IsButtonPressed(irr::u32 button, irr::u32 buttonBitmap) const;
    bool WasButtonPressed(irr::u32 expectedJoystick, irr::u32 expectedButton, irr::u32 actualJoystick, irr::u32 thisButtonState, irr::u32 previousButtonState) const;
    void sendWeatherCommand();

    irr::IrrlichtDevice* device;
    ControllerModel* model;
    GUIMain* gui;
    Network* network;
    WeatherJoystickSetup weatherJoystickSetup;
    irr::core::array<irr::SJoystickInfo> joystickInfo;
    std::vector<irr::u32> joystickPreviousButtonStates;
    irr::u32 lastShownJoystickStatus;

};

#endif
