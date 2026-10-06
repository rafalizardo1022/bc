/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2014 James Packer

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

#include <iostream>

#include "GUI.hpp"
#include "ControllerModel.hpp"
#include "Network.hpp"
#include "../Utilities.hpp"

////using namespace irr;

    WeatherJoystickSetup::WeatherJoystickSetup()
    {
        const irr::u32 unmappedJoystick = 0xFFFFFFFF;
        const irr::u32 unmappedButton = 32;

        enabled = false;

        joystickNoIncreaseWeather = unmappedJoystick;
        joystickButtonIncreaseWeather = unmappedButton;
        joystickNoDecreaseWeather = unmappedJoystick;
        joystickButtonDecreaseWeather = unmappedButton;

        joystickNoIncreaseRain = unmappedJoystick;
        joystickButtonIncreaseRain = unmappedButton;
        joystickNoDecreaseRain = unmappedJoystick;
        joystickButtonDecreaseRain = unmappedButton;

        joystickNoIncreaseVisibility = unmappedJoystick;
        joystickButtonIncreaseVisibility = unmappedButton;
        joystickNoDecreaseVisibility = unmappedJoystick;
        joystickButtonDecreaseVisibility = unmappedButton;

        joystickNoIncreaseWindDirection = unmappedJoystick;
        joystickButtonIncreaseWindDirection = unmappedButton;
        joystickNoDecreaseWindDirection = unmappedJoystick;
        joystickButtonDecreaseWindDirection = unmappedButton;

        joystickNoIncreaseWindSpeed = unmappedJoystick;
        joystickButtonIncreaseWindSpeed = unmappedButton;
        joystickNoDecreaseWindSpeed = unmappedJoystick;
        joystickButtonDecreaseWindSpeed = unmappedButton;

        joystickNoIncreaseStreamDirection = unmappedJoystick;
        joystickButtonIncreaseStreamDirection = unmappedButton;
        joystickNoDecreaseStreamDirection = unmappedJoystick;
        joystickButtonDecreaseStreamDirection = unmappedButton;

        joystickNoIncreaseStreamSpeed = unmappedJoystick;
        joystickButtonIncreaseStreamSpeed = unmappedButton;
        joystickNoDecreaseStreamSpeed = unmappedJoystick;
        joystickButtonDecreaseStreamSpeed = unmappedButton;

        joystickNoToggleStreamOverride = unmappedJoystick;
        joystickButtonToggleStreamOverride = unmappedButton;

        weatherStep = 5;
        rainStep = 5;
        visibilityStep = 1;
        windDirectionStep = 5;
        windSpeedStep = 1;
        streamDirectionStep = 5;
        streamSpeedStep = 1;
    }

    EventReceiver::EventReceiver(irr::IrrlichtDevice* device, ControllerModel* model, GUIMain* gui, Network* network, WeatherJoystickSetup weatherJoystickSetup) //Constructor
	{
		this->device = device; //Link to the irrlicht device
		this->model = model; //Link to the model
		this->gui = gui; //Link to GUI
        this->network = network; //Link to the network
        this->weatherJoystickSetup = weatherJoystickSetup;
        lastShownJoystickStatus = device->getTimer()->getRealTime() - 5000;

        if (this->weatherJoystickSetup.enabled) {
            device->activateJoysticks(joystickInfo);

            std::string joystickInfoMessage = "Number of controller joysticks detected: ";
            joystickInfoMessage.append(std::string(irr::core::stringc(joystickInfo.size()).c_str()));
            device->getLogger()->log(joystickInfoMessage.c_str());

            for (irr::u32 i = 0; i < joystickInfo.size(); i++) {
                joystickInfoMessage = "Controller joystick number: ";
                joystickInfoMessage.append(irr::core::stringc(i).c_str());
                joystickInfoMessage.append(", Name: ");
                joystickInfoMessage.append(std::string(joystickInfo[i].Name.c_str()));
                device->getLogger()->log(joystickInfoMessage.c_str());
            }
        }
    }

    bool EventReceiver::OnEvent(const irr::SEvent& event)
	{

        if (event.EventType == irr::EET_GUI_EVENT)
		{
			irr::s32 id = event.GUIEvent.Caller->getID();


            if (event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED) {

                if (id == GUIMain::GUI_ID_ZOOMIN_BUTTON) {
                    model->increaseZoom();
                }

                if (id == GUIMain::GUI_ID_ZOOMOUT_BUTTON) {
                    model->decreaseZoom();
                }

                if (id == GUIMain::GUI_ID_CHANGE_BUTTON || id == GUIMain::GUI_ID_CHANGE_COURSESPEED_BUTTON) {
                    //Get data from gui

                    irr::f32 legCourse = gui->getEditBoxCourse();
                    irr::f32 legSpeed = gui->getEditBoxSpeed();
                    irr::f32 legDistance = gui->getEditBoxDistance();

                    if (id == GUIMain::GUI_ID_CHANGE_COURSESPEED_BUTTON) {
                        legDistance = -1; //Flag to change course and speed, but not distance
                    }

                    int ship = gui->getSelectedShip();
                    int leg = gui->getSelectedLeg();

                    std::string messageToSend = "MCCL,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(leg));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legCourse));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legSpeed));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legDistance));
                    messageToSend.append("#");

                    network->setStringToSend(messageToSend);

                }
                if (id == GUIMain::GUI_ID_DELETELEG_BUTTON) {
                    int ship = gui->getSelectedShip();
                    int leg = gui->getSelectedLeg();

                    std::string messageToSend = "MCDL,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(leg));
                    messageToSend.append("#");

                    network->setStringToSend(messageToSend);

                }

                if (id == GUIMain::GUI_ID_CLEARLEG_BUTTON) {
                    model->clearSelectedLeg();
                    gui->updateEditBoxes();
                }

                if (id == GUIMain::GUI_ID_CHANGELEGTOCENTRE_BUTTON) {
                    int ship = gui->getSelectedShip();
                    int leg = gui->getSelectedLeg();
                    irr::s32 commandLeg = 0;
                    irr::f32 legCourse = 0;
                    irr::f32 legSpeed = 0;
                    irr::f32 legDistance = 0;

                    if (model->calculateChangeLegToScreenCentre(ship, leg, gui->getEditBoxSpeed(), commandLeg, legCourse, legSpeed, legDistance)) {
                        std::string messageToSend = "MCCL,";
                        messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(commandLeg));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legCourse));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legSpeed));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legDistance));
                        messageToSend.append("#");
                        network->setStringToSend(messageToSend);
                    }
                }

                if (id == GUIMain::GUI_ID_ADDLEG_BUTTON) {

                    irr::f32 legCourse = gui->getEditBoxCourse();
                    irr::f32 legSpeed = gui->getEditBoxSpeed();
                    irr::f32 legDistance = gui->getEditBoxDistance();

                    int ship = gui->getSelectedShip();
                    int leg = gui->getSelectedLeg();

                    std::string messageToSend = "MCAL,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(leg));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legCourse));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legSpeed));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(legDistance));
                    messageToSend.append("#");

                    //std::cout << messageToSend << std::endl;

                    network->setStringToSend(messageToSend);

                }
                if (id == GUIMain::GUI_ID_ADDLEGTOCENTRE_BUTTON) {

                    int ship = gui->getSelectedShip();
                    irr::s32 commandAfterLeg = 0;
                    irr::f32 legCourse = 0;
                    irr::f32 legSpeed = 0;
                    irr::f32 legDistance = 0;

                    if (model->calculateAddLegToScreenCentre(ship, gui->getEditBoxSpeed(), commandAfterLeg, legCourse, legSpeed, legDistance)) {
                        std::string messageToSend = "MCAL,";
                        messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(commandAfterLeg));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legCourse));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legSpeed));
                        messageToSend.append(",");
                        messageToSend.append(Utilities::lexical_cast<std::string>(legDistance));
                        messageToSend.append("#");
                        network->setStringToSend(messageToSend);
                    }
                }
                if (id == GUIMain::GUI_ID_MOVESHIP_BUTTON) {

                    int ship = gui->getSelectedShip();
                    irr::core::vector2df screenCentrePos = gui->getScreenCentrePosition(); //Check screen centre

                    std::string messageToSend = "MCRS,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(screenCentrePos.X));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(screenCentrePos.Y));
                    messageToSend.append("#");
                    network->setStringToSend(messageToSend);

                    //If moving own ship, reset offset, so the map doesn't jump
                    if (ship==0) {
                        model->resetOffset();
                    }
                    //std::cout << messageToSend << std::endl;
                }

                if (id == GUIMain::GUI_ID_SETMMSI_BUTTON) {

                    int ship = gui->getSelectedShip();
                    int mmsi = gui->getEditBoxMMSI();
                    
                    std::string messageToSend = "MCMM,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(mmsi));
                    messageToSend.append("#");
                    network->setStringToSend(messageToSend);
                    //std::cout << messageToSend << std::endl;
                }

                if (id == GUIMain::GUI_ID_RELEASEMOB_BUTTON) {
                    std::string messageToSend = "MCMO,1#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_RETRIEVEMOB_BUTTON) {
                    std::string messageToSend = "MCMO,-1#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_RUDDERPUMP_1_WORKING_BUTTON) {
                    std::string messageToSend = "MCRW,1,1#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_RUDDERPUMP_1_FAILED_BUTTON) {
                    std::string messageToSend = "MCRW,1,0#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_RUDDERPUMP_2_WORKING_BUTTON) {
                    std::string messageToSend = "MCRW,2,1#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_RUDDERPUMP_2_FAILED_BUTTON) {
                    std::string messageToSend = "MCRW,2,0#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_FOLLOWUP_WORKING_BUTTON) {
                    std::string messageToSend = "MCRF,1#";
                    network->setStringToSend(messageToSend);
                }

                if (id == GUIMain::GUI_ID_FOLLOWUP_FAILED_BUTTON) {
                    std::string messageToSend = "MCRF,0#";
                    network->setStringToSend(messageToSend);
                }

            }

            if (event.GUIEvent.EventType==irr::gui::EGET_COMBO_BOX_CHANGED || event.GUIEvent.EventType==irr::gui::EGET_LISTBOX_CHANGED) {
                if (id==GUIMain::GUI_ID_SHIP_COMBOBOX) {
                    model->updateSelectedShip( ((irr::gui::IGUIComboBox*)event.GUIEvent.Caller)->getSelected());
                    gui->updateEditBoxes();
                }

                if (id==GUIMain::GUI_ID_LEG_LISTBOX) {
                    model->updateSelectedLeg( ((irr::gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected());
                    gui->updateEditBoxes();
                }
            }

            if (event.GUIEvent.EventType==irr::gui::EGET_SCROLL_BAR_CHANGED ||
                event.GUIEvent.EventType==irr::gui::EGET_CHECKBOX_CHANGED) {
                if (id == GUIMain::GUI_ID_WEATHER_SCROLLBAR || 
                    id == GUIMain::GUI_ID_RAIN_SCROLLBAR || 
                    id == GUIMain::GUI_ID_VISIBILITY_SCROLLBAR ||
                    id == GUIMain::GUI_ID_WINDDIRECTION_SCROLL_BAR ||
                    id == GUIMain::GUI_ID_WINDSPEED_SCROLL_BAR ||
                    id == GUIMain::GUI_ID_STREAMDIRECTION_SCROLL_BAR ||
                    id == GUIMain::GUI_ID_STREAMSPEED_SCROLL_BAR ||
                    id == GUIMain::GUI_ID_STREAMOVERRIDE_BOX) {
                    //Weather
                    //9 elements in 'Set weather' command: SW,weather,rain,vis,windDirection,windSpeed,streamDirection,streamSpeed,streamOverride
                    irr::f32 weather=gui->getWeather();
                    irr::f32 rain=gui->getRain();
                    irr::f32 visibility=gui->getVisibility();
                    irr::f32 windDirection=gui->getWindDirection();
                    irr::f32 windSpeed=gui->getWindSpeed();
                    irr::f32 streamDirection=gui->getStreamDirection();
                    irr::f32 streamSpeed=gui->getStreamSpeed();
                    bool streamOverride=gui->getStreamOverride();

                    std::string messageToSend = "MCSW,";
                    messageToSend.append(Utilities::lexical_cast<std::string>(weather));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(rain));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(visibility));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(windDirection));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(windSpeed));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(streamDirection));
                    messageToSend.append(",");
                    messageToSend.append(Utilities::lexical_cast<std::string>(streamSpeed));
                    messageToSend.append(",");
                    if (streamOverride) {
                        messageToSend.append("1");
                    } else {
                        messageToSend.append("0");
                    }
                    messageToSend.append("#");
                    network->setStringToSend(messageToSend);
                }
            }

            if (event.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
                if (id == GUIMain::GUI_ID_SART_CHECKBOX) {
                    int ship = gui->getSelectedShip();
                    if (ship > 0) {
                        // Other ship
                        std::string messageToSend = "MCSR,";
                        messageToSend.append(Utilities::lexical_cast<std::string>(ship));
                        messageToSend.append(",");
                        if (((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked()) {
                            messageToSend.append("1");
                        } else {
                            messageToSend.append("0");
                        }
                        messageToSend.append("#");
                        network->setStringToSend(messageToSend);
                    }
                }
            }
            
            /*
            if (event.GUIEvent.EventType==irr::gui::EGDT_WINDOW_CLOSE) {
                if (id==GUIMain::GUI_ID_WINDOW) {
                    return true; //Absorb event : TODO: Should this trigger program close?
                }

            }
            */


        }

        //From keyboard
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown)
		{

            if (event.KeyInput.Shift) {
                //Shift down

            } else if (event.KeyInput.Control) {
                //Ctrl down


            } else {
                //Shift and Ctrl not down
                if (event.KeyInput.Key == irr::KEY_KEY_9) {
                    std::string messageToSend = "MCCT,3600#";
                    network->setStringToSend(messageToSend);
                }

                if (event.KeyInput.Key == irr::KEY_KEY_8) {
                    std::string messageToSend = "MCCT,-3600#";
                    network->setStringToSend(messageToSend);
                }

            }
		} //end of key down event

        if (event.EventType == irr::EET_JOYSTICK_INPUT_EVENT && weatherJoystickSetup.enabled)
        {
            const irr::u32 thisJoystick = event.JoystickEvent.Joystick;

            if (device->getTimer()->getRealTime() - lastShownJoystickStatus > 5000) {
                std::string joystickInfoMessage = "Controller joystick status (";
                joystickInfoMessage.append(irr::core::stringc(thisJoystick).c_str());
                joystickInfoMessage.append(") buttons: ");
                joystickInfoMessage.append(irr::core::stringc(event.JoystickEvent.ButtonStates).c_str());
                joystickInfoMessage.append(" POV: ");
                joystickInfoMessage.append(irr::core::stringc(event.JoystickEvent.POV).c_str());
                joystickInfoMessage.append(" axes: ");

                for (irr::u8 thisAxis = 0; thisAxis < event.JoystickEvent.NUMBER_OF_AXES; thisAxis++) {
                    joystickInfoMessage.append(irr::core::stringc(event.JoystickEvent.Axis[thisAxis]).c_str());
                    joystickInfoMessage.append(" ");
                }

                device->getLogger()->log(joystickInfoMessage.c_str());
                lastShownJoystickStatus = device->getTimer()->getRealTime();
            }

            while (joystickPreviousButtonStates.size() <= thisJoystick) {
                joystickPreviousButtonStates.push_back(0);
            }

            const irr::u32 thisButtonState = event.JoystickEvent.ButtonStates;
            const irr::u32 previousButtonState = joystickPreviousButtonStates.at(thisJoystick);
            bool weatherChanged = false;

            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseWeather, weatherJoystickSetup.joystickButtonIncreaseWeather, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWeather(weatherJoystickSetup.weatherStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseWeather, weatherJoystickSetup.joystickButtonDecreaseWeather, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWeather(-weatherJoystickSetup.weatherStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseRain, weatherJoystickSetup.joystickButtonIncreaseRain, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustRain(weatherJoystickSetup.rainStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseRain, weatherJoystickSetup.joystickButtonDecreaseRain, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustRain(-weatherJoystickSetup.rainStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseVisibility, weatherJoystickSetup.joystickButtonIncreaseVisibility, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustVisibility(weatherJoystickSetup.visibilityStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseVisibility, weatherJoystickSetup.joystickButtonDecreaseVisibility, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustVisibility(-weatherJoystickSetup.visibilityStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseWindDirection, weatherJoystickSetup.joystickButtonIncreaseWindDirection, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWindDirection(weatherJoystickSetup.windDirectionStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseWindDirection, weatherJoystickSetup.joystickButtonDecreaseWindDirection, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWindDirection(-weatherJoystickSetup.windDirectionStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseWindSpeed, weatherJoystickSetup.joystickButtonIncreaseWindSpeed, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWindSpeed(weatherJoystickSetup.windSpeedStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseWindSpeed, weatherJoystickSetup.joystickButtonDecreaseWindSpeed, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustWindSpeed(-weatherJoystickSetup.windSpeedStep);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseStreamDirection, weatherJoystickSetup.joystickButtonIncreaseStreamDirection, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustStreamDirection(weatherJoystickSetup.streamDirectionStep);
                gui->setStreamOverride(true);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseStreamDirection, weatherJoystickSetup.joystickButtonDecreaseStreamDirection, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustStreamDirection(-weatherJoystickSetup.streamDirectionStep);
                gui->setStreamOverride(true);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoIncreaseStreamSpeed, weatherJoystickSetup.joystickButtonIncreaseStreamSpeed, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustStreamSpeed(weatherJoystickSetup.streamSpeedStep);
                gui->setStreamOverride(true);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoDecreaseStreamSpeed, weatherJoystickSetup.joystickButtonDecreaseStreamSpeed, thisJoystick, thisButtonState, previousButtonState)) {
                gui->adjustStreamSpeed(-weatherJoystickSetup.streamSpeedStep);
                gui->setStreamOverride(true);
                weatherChanged = true;
            }
            if (WasButtonPressed(weatherJoystickSetup.joystickNoToggleStreamOverride, weatherJoystickSetup.joystickButtonToggleStreamOverride, thisJoystick, thisButtonState, previousButtonState)) {
                gui->setStreamOverride(!gui->getStreamOverride());
                weatherChanged = true;
            }

            if (weatherChanged) {
                sendWeatherCommand();
            }

            joystickPreviousButtonStates.at(thisJoystick) = thisButtonState;
        }

		//From mouse
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {

            if (event.MouseInput.Event == irr::EMIE_MOUSE_WHEEL ) {
                irr::gui::IGUIElement* overElement = device->getGUIEnvironment()->getRootGUIElement()->getElementFromPoint(device->getCursorControl()->getPosition());
                if ( (overElement == 0 || overElement == device->getGUIEnvironment()->getRootGUIElement()) ) {
                    if (event.MouseInput.Wheel > 0) {
                        model->increaseZoom();
                    } else if (event.MouseInput.Wheel < 0) {
                        model->decreaseZoom();
                    }
                    return true;
                }
            }

            if (event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN ) {

                //Check if we're over a gui element, and if so ignore the click
                irr::gui::IGUIElement* overElement = device->getGUIEnvironment()->getRootGUIElement()->getElementFromPoint(device->getCursorControl()->getPosition());
                if ( (overElement == 0 || overElement == device->getGUIEnvironment()->getRootGUIElement()) ) {
                    model->setMouseDown(true);
                }
            }

            if (event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP ) {
                model->setMouseDown(false);
            }

            if (event.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN ) {
                irr::core::position2d<irr::s32> cursorPosition = device->getCursorControl()->getPosition();
                irr::gui::IGUIElement* overElement = device->getGUIEnvironment()->getRootGUIElement()->getElementFromPoint(cursorPosition);
                if ( (overElement == 0 || overElement == device->getGUIEnvironment()->getRootGUIElement()) ) {
                    model->centreMapAtScreenPoint(cursorPosition);
                    return true;
                }
            }

		} //end of mouse event


        return false;

    }

    bool EventReceiver::IsButtonPressed(irr::u32 button, irr::u32 buttonBitmap) const
    {
        if (button >= 32) {
            return false;
        }

        return (buttonBitmap & (irr::u32(1) << button)) ? true : false;
    }

    bool EventReceiver::WasButtonPressed(irr::u32 expectedJoystick, irr::u32 expectedButton, irr::u32 actualJoystick, irr::u32 thisButtonState, irr::u32 previousButtonState) const
    {
        if (expectedJoystick != actualJoystick || expectedButton >= 32) {
            return false;
        }

        return IsButtonPressed(expectedButton, thisButtonState) &&
               !IsButtonPressed(expectedButton, previousButtonState);
    }

    void EventReceiver::sendWeatherCommand()
    {
        std::string messageToSend = "MCSW,";
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getWeather()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getRain()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getVisibility()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getWindDirection()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getWindSpeed()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getStreamDirection()));
        messageToSend.append(",");
        messageToSend.append(Utilities::lexical_cast<std::string>(gui->getStreamSpeed()));
        messageToSend.append(",");
        messageToSend.append(gui->getStreamOverride() ? "1" : "0");
        messageToSend.append("#");
        network->setStringToSend(messageToSend);
    }
