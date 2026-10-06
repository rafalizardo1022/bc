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

#ifndef __INSTRUMENTGUI_HPP_INCLUDED__
#define __INSTRUMENTGUI_HPP_INCLUDED__

#include <string>

#include "irrlicht.h"

#include "../InstrumentData.hpp"

enum InstrumentDisplayMode
{
    InstrumentDisplayVessel = 0,
    InstrumentDisplayNavigation = 1
};

class InstrumentGUI
{
public:
    InstrumentGUI(irr::IrrlichtDevice* device, InstrumentDisplayMode mode, int listenPort);

    void setMode(InstrumentDisplayMode mode);
    void toggleMode();
    InstrumentDisplayMode getMode() const;
    void draw(const InstrumentData& data, bool connected);

private:
    irr::IrrlichtDevice* device;
    irr::video::IVideoDriver* driver;
    irr::gui::IGUIEnvironment* guienv;
    InstrumentDisplayMode mode;
    int listenPort;

    irr::video::SColor backgroundColour;
    irr::video::SColor panelColour;
    irr::video::SColor borderColour;
    irr::video::SColor textColour;
    irr::video::SColor mutedTextColour;
    irr::video::SColor accentColour;
    irr::video::SColor warningColour;
    irr::video::SColor dangerColour;

    void drawHeader(const InstrumentData& data, bool connected);
    void drawWaiting();
    void drawVesselDisplay(const InstrumentData& data);
    void drawNavigationDisplay(const InstrumentData& data);

    void drawPanel(const irr::core::rect<irr::s32>& rect, const std::string& title);
    void drawText(const std::string& text, const irr::core::rect<irr::s32>& rect, irr::video::SColor colour, bool centre = false, bool verticalCentre = false);
    void drawValue(const irr::core::rect<irr::s32>& rect, const std::string& label, const std::string& value, const std::string& unit, irr::video::SColor valueColour);
    void drawBar(const irr::core::rect<irr::s32>& rect, float value, float minimum, float maximum, irr::video::SColor fillColour);
    void drawRudder(const irr::core::rect<irr::s32>& rect, float rudderDeg, float wheelDeg);
    void drawCompass(const irr::core::rect<irr::s32>& rect, float headingDeg, float cogDeg);

    std::string formatFloat(float value, int precision) const;
    std::string formatSignedFloat(float value, int precision) const;
    std::string formatPercent(float value) const;
    std::string formatTime(uint64_t timestamp) const;
    std::string formatLatLong(float value, bool latitude) const;
    irr::core::rect<irr::s32> inset(const irr::core::rect<irr::s32>& rect, irr::s32 amount) const;
};

#endif
