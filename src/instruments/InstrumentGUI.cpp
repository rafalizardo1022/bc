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

#include "InstrumentGUI.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "../Constants.hpp"
#include "../Utilities.hpp"

InstrumentGUI::InstrumentGUI(irr::IrrlichtDevice* device, InstrumentDisplayMode mode, int listenPort)
{
    this->device = device;
    driver = device->getVideoDriver();
    guienv = device->getGUIEnvironment();
    this->mode = mode;
    this->listenPort = listenPort;

    backgroundColour = irr::video::SColor(255, 5, 14, 18);
    panelColour = irr::video::SColor(255, 13, 28, 35);
    borderColour = irr::video::SColor(255, 53, 86, 96);
    textColour = irr::video::SColor(255, 222, 239, 238);
    mutedTextColour = irr::video::SColor(255, 136, 160, 165);
    accentColour = irr::video::SColor(255, 81, 213, 197);
    warningColour = irr::video::SColor(255, 230, 182, 80);
    dangerColour = irr::video::SColor(255, 226, 88, 83);
}

void InstrumentGUI::setMode(InstrumentDisplayMode mode)
{
    this->mode = mode;
}

void InstrumentGUI::toggleMode()
{
    if (mode == InstrumentDisplayVessel) {
        mode = InstrumentDisplayNavigation;
    } else {
        mode = InstrumentDisplayVessel;
    }
}

InstrumentDisplayMode InstrumentGUI::getMode() const
{
    return mode;
}

void InstrumentGUI::draw(const InstrumentData& data, bool connected)
{
    drawHeader(data, connected);

    if (!connected || !data.valid) {
        drawWaiting();
    } else if (mode == InstrumentDisplayVessel) {
        drawVesselDisplay(data);
    } else {
        drawNavigationDisplay(data);
    }

    guienv->drawAll();
}

void InstrumentGUI::drawHeader(const InstrumentData& data, bool connected)
{
    irr::u32 su = driver->getScreenSize().Width;
    irr::core::rect<irr::s32> header(0, 0, su, 54);
    driver->draw2DRectangle(irr::video::SColor(255, 9, 23, 29), header);
    driver->draw2DLine(irr::core::vector2d<irr::s32>(0, 53), irr::core::vector2d<irr::s32>(su, 53), borderColour);

    std::string title = (mode == InstrumentDisplayVessel) ? "VESSEL / MACHINERY" : "NAVIGATION / ENVIRONMENT";
    drawText(title, irr::core::rect<irr::s32>(18, 8, su / 2, 36), textColour, false, true);

    std::string status = connected ? "LIVE" : "WAITING";
    irr::video::SColor statusColour = connected ? accentColour : warningColour;
    drawText(status, irr::core::rect<irr::s32>(su - 310, 8, su - 210, 36), statusColour, true, true);

    std::string portText = "PORT " + Utilities::lexical_cast<std::string>(listenPort);
    drawText(portText, irr::core::rect<irr::s32>(su - 205, 8, su - 110, 36), mutedTextColour, true, true);

    std::string timeText = data.valid ? formatTime(data.timestamp) : "--";
    drawText(timeText, irr::core::rect<irr::s32>(su - 520, 30, su - 18, 52), mutedTextColour, false, true);
}

void InstrumentGUI::drawWaiting()
{
    irr::u32 su = driver->getScreenSize().Width;
    irr::u32 sh = driver->getScreenSize().Height;
    irr::core::rect<irr::s32> box(su * 0.20, sh * 0.38, su * 0.80, sh * 0.58);
    drawPanel(box, "CONNECTION");
    drawText("Waiting for instrument data", irr::core::rect<irr::s32>(box.UpperLeftCorner.X, box.UpperLeftCorner.Y + 44, box.LowerRightCorner.X, box.UpperLeftCorner.Y + 82), textColour, true, true);
    drawText("Add this display host:port to the simulator UDP display list.", irr::core::rect<irr::s32>(box.UpperLeftCorner.X, box.UpperLeftCorner.Y + 84, box.LowerRightCorner.X, box.UpperLeftCorner.Y + 120), mutedTextColour, true, true);
}

void InstrumentGUI::drawVesselDisplay(const InstrumentData& data)
{
    irr::u32 su = driver->getScreenSize().Width;
    irr::u32 sh = driver->getScreenSize().Height;
    irr::s32 margin = 14;
    irr::s32 gap = 12;
    irr::s32 top = 54 + margin;
    irr::s32 usableWidth = su - 2 * margin - gap;
    irr::s32 usableHeight = sh - top - margin - gap;
    irr::s32 columnWidth = usableWidth / 2;
    irr::s32 rowHeight = usableHeight / 2;

    irr::core::rect<irr::s32> propulsion(margin, top, margin + columnWidth, top + rowHeight);
    irr::core::rect<irr::s32> steering(margin + columnWidth + gap, top, su - margin, top + rowHeight);
    irr::core::rect<irr::s32> motion(margin, top + rowHeight + gap, margin + columnWidth, sh - margin);
    irr::core::rect<irr::s32> auxiliaries(margin + columnWidth + gap, top + rowHeight + gap, su - margin, sh - margin);

    drawPanel(propulsion, "PROPULSION");
    irr::core::rect<irr::s32> p = inset(propulsion, 16);
    p.UpperLeftCorner.Y += 24;
    irr::s32 halfWidth = (p.LowerRightCorner.X - p.UpperLeftCorner.X) / 2;
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y, p.UpperLeftCorner.X + halfWidth - 6, p.UpperLeftCorner.Y + 72), "PORT RPM", formatFloat(data.portEngineRpm, 0), "rpm", accentColour);
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X + halfWidth + 6, p.UpperLeftCorner.Y, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 72), data.isSingleEngine ? "ENGINE RPM" : "STBD RPM", formatFloat(data.stbdEngineRpm, 0), "rpm", accentColour);
    drawText("PORT COMMAND " + formatPercent(data.portEngineCommand), irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 88, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 110), mutedTextColour);
    drawBar(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 114, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 138), data.portEngineCommand, -1, 1, accentColour);
    drawText((data.isSingleEngine ? "ENGINE COMMAND " : "STBD COMMAND ") + formatPercent(data.stbdEngineCommand), irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 152, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 174), mutedTextColour);
    drawBar(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 178, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 202), data.stbdEngineCommand, -1, 1, accentColour);

    drawPanel(steering, "STEERING");
    irr::core::rect<irr::s32> s = inset(steering, 16);
    s.UpperLeftCorner.Y += 24;
    drawRudder(irr::core::rect<irr::s32>(s.UpperLeftCorner.X, s.UpperLeftCorner.Y, s.UpperLeftCorner.X + halfWidth - 6, s.LowerRightCorner.Y), data.rudderDeg, data.wheelDeg);
    drawValue(irr::core::rect<irr::s32>(s.UpperLeftCorner.X + halfWidth + 6, s.UpperLeftCorner.Y, s.LowerRightCorner.X, s.UpperLeftCorner.Y + 62), "RUDDER", formatSignedFloat(data.rudderDeg, 1), "deg", textColour);
    drawValue(irr::core::rect<irr::s32>(s.UpperLeftCorner.X + halfWidth + 6, s.UpperLeftCorner.Y + 72, s.LowerRightCorner.X, s.UpperLeftCorner.Y + 134), "WHEEL", formatSignedFloat(data.wheelDeg, 1), "deg", mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(s.UpperLeftCorner.X + halfWidth + 6, s.UpperLeftCorner.Y + 144, s.LowerRightCorner.X, s.UpperLeftCorner.Y + 206), "ROT", formatSignedFloat(data.rateOfTurnDegPerMin, 1), "deg/min", data.hasTurnIndicator ? accentColour : mutedTextColour);
    std::string steeringState = data.emergencySteering ? "EMERGENCY STEERING" : "FOLLOW-UP";
    irr::video::SColor steeringColour = data.emergencySteering ? warningColour : accentColour;
    drawText(steeringState, irr::core::rect<irr::s32>(s.UpperLeftCorner.X + halfWidth + 6, s.UpperLeftCorner.Y + 214, s.LowerRightCorner.X, s.UpperLeftCorner.Y + 238), steeringColour, true, true);
    drawText(std::string("PUMPS ") + (data.rudderPump1 ? "1" : "-") + " " + (data.rudderPump2 ? "2" : "-"), irr::core::rect<irr::s32>(s.UpperLeftCorner.X + halfWidth + 6, s.UpperLeftCorner.Y + 238, s.LowerRightCorner.X, s.UpperLeftCorner.Y + 260), mutedTextColour, true, true);

    drawPanel(motion, "MOTION / HULL");
    irr::core::rect<irr::s32> m = inset(motion, 16);
    m.UpperLeftCorner.Y += 24;
    irr::s32 thirdWidth = (m.LowerRightCorner.X - m.UpperLeftCorner.X) / 3;
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X, m.UpperLeftCorner.Y, m.UpperLeftCorner.X + thirdWidth - 6, m.UpperLeftCorner.Y + 72), "STW", formatFloat(data.speedThroughWaterKts, 1), "kt", accentColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X + thirdWidth, m.UpperLeftCorner.Y, m.UpperLeftCorner.X + 2 * thirdWidth - 6, m.UpperLeftCorner.Y + 72), "SOG", formatFloat(data.sogKts, 1), "kt", textColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X + 2 * thirdWidth, m.UpperLeftCorner.Y, m.LowerRightCorner.X, m.UpperLeftCorner.Y + 72), "HDG", formatFloat(data.headingDeg, 0), "deg", textColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X, m.UpperLeftCorner.Y + 90, m.UpperLeftCorner.X + thirdWidth - 6, m.UpperLeftCorner.Y + 162), "DEPTH", formatFloat(data.depthM, 1), "m", data.hasDepthSounder ? textColour : mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X + thirdWidth, m.UpperLeftCorner.Y + 90, m.UpperLeftCorner.X + 2 * thirdWidth - 6, m.UpperLeftCorner.Y + 162), "DBK", formatFloat(data.depthBelowKeelM, 1), "m", data.depthBelowKeelM < 2 ? warningColour : accentColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X + 2 * thirdWidth, m.UpperLeftCorner.Y + 90, m.LowerRightCorner.X, m.UpperLeftCorner.Y + 162), "ROLL", formatSignedFloat(data.rollDeg, 1), "deg", textColour);
    drawValue(irr::core::rect<irr::s32>(m.UpperLeftCorner.X, m.UpperLeftCorner.Y + 180, m.UpperLeftCorner.X + thirdWidth - 6, m.UpperLeftCorner.Y + 252), "PITCH", formatSignedFloat(data.pitchDeg, 1), "deg", textColour);

    drawPanel(auxiliaries, "AUXILIARIES");
    irr::core::rect<irr::s32> a = inset(auxiliaries, 16);
    a.UpperLeftCorner.Y += 24;
    drawText("BOW THRUSTER " + formatPercent(data.bowThruster), irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 22), data.hasBowThruster ? mutedTextColour : dangerColour);
    drawBar(irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y + 26, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 50), data.bowThruster, -1, 1, accentColour);
    drawText("STERN THRUSTER " + formatPercent(data.sternThruster), irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y + 68, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 90), data.hasSternThruster ? mutedTextColour : dangerColour);
    drawBar(irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y + 94, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 118), data.sternThruster, -1, 1, accentColour);
    drawValue(irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y + 138, a.UpperLeftCorner.X + halfWidth - 6, a.UpperLeftCorner.Y + 210), "PORT AZI", formatSignedFloat(data.portSchottelDeg, 0), "deg", data.isAzimuthDrive ? textColour : mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(a.UpperLeftCorner.X + halfWidth + 6, a.UpperLeftCorner.Y + 138, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 210), "STBD AZI", formatSignedFloat(data.stbdSchottelDeg, 0), "deg", data.isAzimuthDrive ? textColour : mutedTextColour);
    drawText(std::string("CLUTCH ") + (data.portClutch ? "PORT IN" : "PORT OUT") + " / " + (data.stbdClutch ? "STBD IN" : "STBD OUT"), irr::core::rect<irr::s32>(a.UpperLeftCorner.X, a.UpperLeftCorner.Y + 224, a.LowerRightCorner.X, a.UpperLeftCorner.Y + 252), data.isAzimuthDrive ? mutedTextColour : dangerColour, true, true);
}

void InstrumentGUI::drawNavigationDisplay(const InstrumentData& data)
{
    irr::u32 su = driver->getScreenSize().Width;
    irr::u32 sh = driver->getScreenSize().Height;
    irr::s32 margin = 14;
    irr::s32 gap = 12;
    irr::s32 top = 54 + margin;
    irr::s32 usableWidth = su - 2 * margin - gap;
    irr::s32 usableHeight = sh - top - margin - gap;
    irr::s32 columnWidth = usableWidth / 2;
    irr::s32 rowHeight = usableHeight / 2;

    irr::core::rect<irr::s32> compass(margin, top, margin + columnWidth, top + rowHeight);
    irr::core::rect<irr::s32> position(margin + columnWidth + gap, top, su - margin, top + rowHeight);
    irr::core::rect<irr::s32> environment(margin, top + rowHeight + gap, margin + columnWidth, sh - margin);
    irr::core::rect<irr::s32> water(margin + columnWidth + gap, top + rowHeight + gap, su - margin, sh - margin);

    drawPanel(compass, "HEADING / COURSE");
    drawCompass(inset(irr::core::rect<irr::s32>(compass.UpperLeftCorner.X, compass.UpperLeftCorner.Y + 24, compass.LowerRightCorner.X, compass.LowerRightCorner.Y), 12), data.headingDeg, data.cogDeg);

    drawPanel(position, "POSITION / SPEED");
    irr::core::rect<irr::s32> p = inset(position, 16);
    p.UpperLeftCorner.Y += 24;
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 62), "LAT", formatLatLong(data.latitudeDeg, true), "", data.hasGps ? textColour : mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 72, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 134), "LONG", formatLatLong(data.longitudeDeg, false), "", data.hasGps ? textColour : mutedTextColour);
    irr::s32 halfWidth = (p.LowerRightCorner.X - p.UpperLeftCorner.X) / 2;
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 150, p.UpperLeftCorner.X + halfWidth - 6, p.UpperLeftCorner.Y + 222), "SOG", formatFloat(data.sogKts, 1), "kt", textColour);
    drawValue(irr::core::rect<irr::s32>(p.UpperLeftCorner.X + halfWidth + 6, p.UpperLeftCorner.Y + 150, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 222), "COG", formatFloat(data.cogDeg, 0), "deg", textColour);
    drawText(formatTime(data.timestamp), irr::core::rect<irr::s32>(p.UpperLeftCorner.X, p.UpperLeftCorner.Y + 234, p.LowerRightCorner.X, p.UpperLeftCorner.Y + 262), mutedTextColour, true, true);

    drawPanel(environment, "WEATHER / WIND");
    irr::core::rect<irr::s32> e = inset(environment, 16);
    e.UpperLeftCorner.Y += 24;
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X, e.UpperLeftCorner.Y, e.UpperLeftCorner.X + halfWidth - 6, e.UpperLeftCorner.Y + 72), "TRUE WIND", formatFloat(data.windDirectionTrueDeg, 0), "deg", textColour);
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X + halfWidth + 6, e.UpperLeftCorner.Y, e.LowerRightCorner.X, e.UpperLeftCorner.Y + 72), "WIND SPEED", formatFloat(data.windSpeedKts, 1), "kt", accentColour);
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X, e.UpperLeftCorner.Y + 88, e.UpperLeftCorner.X + halfWidth - 6, e.UpperLeftCorner.Y + 160), "APP WIND", formatSignedFloat(data.apparentWindRelativeDeg, 0), "deg rel", warningColour);
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X + halfWidth + 6, e.UpperLeftCorner.Y + 88, e.LowerRightCorner.X, e.UpperLeftCorner.Y + 160), "APP SPEED", formatFloat(data.apparentWindSpeedKts, 1), "kt", warningColour);
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X, e.UpperLeftCorner.Y + 176, e.UpperLeftCorner.X + halfWidth - 6, e.UpperLeftCorner.Y + 248), "VISIBILITY", formatFloat(data.visibilityNm, 1), "nm", textColour);
    drawValue(irr::core::rect<irr::s32>(e.UpperLeftCorner.X + halfWidth + 6, e.UpperLeftCorner.Y + 176, e.LowerRightCorner.X, e.UpperLeftCorner.Y + 248), "RAIN", formatFloat(data.rain, 1), "/10", mutedTextColour);

    drawPanel(water, "WATER / DEPTH");
    irr::core::rect<irr::s32> w = inset(water, 16);
    w.UpperLeftCorner.Y += 24;
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X, w.UpperLeftCorner.Y, w.UpperLeftCorner.X + halfWidth - 6, w.UpperLeftCorner.Y + 72), "CURRENT SET", formatFloat(data.currentDirectionDeg, 0), "deg", textColour);
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X + halfWidth + 6, w.UpperLeftCorner.Y, w.LowerRightCorner.X, w.UpperLeftCorner.Y + 72), "CURRENT DRIFT", formatFloat(data.currentSpeedKts, 2), "kt", accentColour);
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X, w.UpperLeftCorner.Y + 88, w.UpperLeftCorner.X + halfWidth - 6, w.UpperLeftCorner.Y + 160), "DEPTH", formatFloat(data.depthM, 1), "m", data.hasDepthSounder ? textColour : mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X + halfWidth + 6, w.UpperLeftCorner.Y + 88, w.LowerRightCorner.X, w.UpperLeftCorner.Y + 160), "DBK", formatFloat(data.depthBelowKeelM, 1), "m", data.depthBelowKeelM < 2 ? warningColour : accentColour);
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X, w.UpperLeftCorner.Y + 176, w.UpperLeftCorner.X + halfWidth - 6, w.UpperLeftCorner.Y + 248), "TIDE HEIGHT", formatSignedFloat(data.tideHeightM, 2), "m", mutedTextColour);
    drawValue(irr::core::rect<irr::s32>(w.UpperLeftCorner.X + halfWidth + 6, w.UpperLeftCorner.Y + 176, w.LowerRightCorner.X, w.UpperLeftCorner.Y + 248), "WEATHER", formatFloat(data.weather, 1), "/12", mutedTextColour);
}

void InstrumentGUI::drawPanel(const irr::core::rect<irr::s32>& rect, const std::string& title)
{
    driver->draw2DRectangle(panelColour, rect);
    driver->draw2DLine(rect.UpperLeftCorner, irr::core::vector2d<irr::s32>(rect.LowerRightCorner.X, rect.UpperLeftCorner.Y), borderColour);
    driver->draw2DLine(rect.UpperLeftCorner, irr::core::vector2d<irr::s32>(rect.UpperLeftCorner.X, rect.LowerRightCorner.Y), borderColour);
    driver->draw2DLine(irr::core::vector2d<irr::s32>(rect.LowerRightCorner.X, rect.UpperLeftCorner.Y), rect.LowerRightCorner, borderColour);
    driver->draw2DLine(irr::core::vector2d<irr::s32>(rect.UpperLeftCorner.X, rect.LowerRightCorner.Y), rect.LowerRightCorner, borderColour);
    drawText(title, irr::core::rect<irr::s32>(rect.UpperLeftCorner.X + 12, rect.UpperLeftCorner.Y + 6, rect.LowerRightCorner.X - 12, rect.UpperLeftCorner.Y + 28), mutedTextColour);
}

void InstrumentGUI::drawText(const std::string& text, const irr::core::rect<irr::s32>& rect, irr::video::SColor colour, bool centre, bool verticalCentre)
{
    irr::gui::IGUIFont* font = guienv->getSkin()->getFont();
    if (!font) {
        return;
    }
    irr::core::stringw wide(text.c_str());
    font->draw(wide.c_str(), rect, colour, centre, verticalCentre);
}

void InstrumentGUI::drawValue(const irr::core::rect<irr::s32>& rect, const std::string& label, const std::string& value, const std::string& unit, irr::video::SColor valueColour)
{
    drawText(label, irr::core::rect<irr::s32>(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y, rect.LowerRightCorner.X, rect.UpperLeftCorner.Y + 20), mutedTextColour, true, true);
    drawText(value, irr::core::rect<irr::s32>(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y + 20, rect.LowerRightCorner.X, rect.LowerRightCorner.Y - 18), valueColour, true, true);
    drawText(unit, irr::core::rect<irr::s32>(rect.UpperLeftCorner.X, rect.LowerRightCorner.Y - 22, rect.LowerRightCorner.X, rect.LowerRightCorner.Y), mutedTextColour, true, true);
}

void InstrumentGUI::drawBar(const irr::core::rect<irr::s32>& rect, float value, float minimum, float maximum, irr::video::SColor fillColour)
{
    driver->draw2DRectangle(irr::video::SColor(255, 4, 10, 13), rect);
    irr::s32 width = rect.LowerRightCorner.X - rect.UpperLeftCorner.X;
    irr::s32 zeroX = rect.UpperLeftCorner.X;
    if (minimum < 0 && maximum > 0) {
        zeroX = rect.UpperLeftCorner.X + (irr::s32)((0 - minimum) / (maximum - minimum) * width);
    }

    if (value < minimum) {
        value = minimum;
    }
    if (value > maximum) {
        value = maximum;
    }

    irr::s32 valueX = rect.UpperLeftCorner.X + (irr::s32)((value - minimum) / (maximum - minimum) * width);
    irr::core::rect<irr::s32> fillRect;
    fillRect.UpperLeftCorner.Y = rect.UpperLeftCorner.Y + 2;
    fillRect.LowerRightCorner.Y = rect.LowerRightCorner.Y - 2;
    fillRect.UpperLeftCorner.X = valueX < zeroX ? valueX : zeroX;
    fillRect.LowerRightCorner.X = valueX > zeroX ? valueX : zeroX;
    driver->draw2DRectangle(fillColour, fillRect);
    driver->draw2DLine(irr::core::vector2d<irr::s32>(zeroX, rect.UpperLeftCorner.Y), irr::core::vector2d<irr::s32>(zeroX, rect.LowerRightCorner.Y), mutedTextColour);
}

void InstrumentGUI::drawRudder(const irr::core::rect<irr::s32>& rect, float rudderDeg, float wheelDeg)
{
    irr::s32 centreX = (rect.UpperLeftCorner.X + rect.LowerRightCorner.X) / 2;
    irr::s32 centreY = (rect.UpperLeftCorner.Y + rect.LowerRightCorner.Y) / 2 + 18;
    irr::s32 radius = (rect.LowerRightCorner.Y - rect.UpperLeftCorner.Y) / 3;
    if (radius > (rect.LowerRightCorner.X - rect.UpperLeftCorner.X) / 2 - 12) {
        radius = (rect.LowerRightCorner.X - rect.UpperLeftCorner.X) / 2 - 12;
    }

    irr::core::vector2d<irr::s32> centre(centreX, centreY);
    for (int i = -30; i <= 30; i += 5) {
        irr::f32 angle = -i * 2.0f * irr::core::DEGTORAD;
        irr::f32 xVector = std::sin(angle);
        irr::f32 yVector = std::cos(angle);
        irr::s32 startX = centreX + (irr::s32)(xVector * radius * 0.80f);
        irr::s32 startY = centreY + (irr::s32)(yVector * radius * 0.80f);
        irr::s32 endX = centreX + (irr::s32)(xVector * radius);
        irr::s32 endY = centreY + (irr::s32)(yVector * radius);
        driver->draw2DLine(irr::core::vector2d<irr::s32>(startX, startY), irr::core::vector2d<irr::s32>(endX, endY), mutedTextColour);
    }

    irr::core::vector2d<irr::s32> rudderHead(0, radius);
    irr::core::vector2d<irr::s32> wheelHead(0, radius * 0.85f);
    rudderHead.rotateBy(-rudderDeg * 2.0f);
    wheelHead.rotateBy(-wheelDeg * 2.0f);
    driver->draw2DLine(centre, centre + wheelHead, warningColour);
    driver->draw2DLine(centre, centre + rudderHead, accentColour);
    driver->draw2DPolygon(centre, 5, textColour, 18);
    drawText("PORT", irr::core::rect<irr::s32>(rect.UpperLeftCorner.X, centreY + radius - 12, centreX - 16, centreY + radius + 12), mutedTextColour, true, true);
    drawText("STBD", irr::core::rect<irr::s32>(centreX + 16, centreY + radius - 12, rect.LowerRightCorner.X, centreY + radius + 12), mutedTextColour, true, true);
}

void InstrumentGUI::drawCompass(const irr::core::rect<irr::s32>& rect, float headingDeg, float cogDeg)
{
    irr::s32 centreX = (rect.UpperLeftCorner.X + rect.LowerRightCorner.X) / 2;
    irr::s32 centreY = (rect.UpperLeftCorner.Y + rect.LowerRightCorner.Y) / 2;
    irr::s32 radius = (rect.LowerRightCorner.Y - rect.UpperLeftCorner.Y) / 2 - 12;
    if (radius > (rect.LowerRightCorner.X - rect.UpperLeftCorner.X) / 2 - 12) {
        radius = (rect.LowerRightCorner.X - rect.UpperLeftCorner.X) / 2 - 12;
    }
    irr::core::vector2d<irr::s32> centre(centreX, centreY);

    driver->draw2DPolygon(centre, radius, irr::video::SColor(255, 4, 11, 15), 96);
    for (int i = 0; i < 360; i += 5) {
        irr::f32 relative = (i - headingDeg) * irr::core::DEGTORAD;
        irr::f32 xVector = std::sin(relative);
        irr::f32 yVector = -std::cos(relative);
        irr::f32 startFactor = (i % 30 == 0) ? 0.78f : 0.88f;
        irr::s32 startX = centreX + (irr::s32)(xVector * radius * startFactor);
        irr::s32 startY = centreY + (irr::s32)(yVector * radius * startFactor);
        irr::s32 endX = centreX + (irr::s32)(xVector * radius);
        irr::s32 endY = centreY + (irr::s32)(yVector * radius);
        driver->draw2DLine(irr::core::vector2d<irr::s32>(startX, startY), irr::core::vector2d<irr::s32>(endX, endY), (i % 30 == 0) ? textColour : mutedTextColour);

        if (i % 90 == 0) {
            std::string label = "N";
            if (i == 90) { label = "E"; }
            if (i == 180) { label = "S"; }
            if (i == 270) { label = "W"; }
            irr::s32 textX = centreX + (irr::s32)(xVector * radius * 0.62f);
            irr::s32 textY = centreY + (irr::s32)(yVector * radius * 0.62f);
            drawText(label, irr::core::rect<irr::s32>(textX - 18, textY - 12, textX + 18, textY + 12), textColour, true, true);
        }
    }

    irr::f32 cogRelative = (cogDeg - headingDeg) * irr::core::DEGTORAD;
    irr::core::vector2d<irr::s32> cogPoint(centreX + (irr::s32)(std::sin(cogRelative) * radius * 0.72f),
                                           centreY - (irr::s32)(std::cos(cogRelative) * radius * 0.72f));
    driver->draw2DLine(centre, cogPoint, warningColour);
    driver->draw2DLine(centre, irr::core::vector2d<irr::s32>(centreX, centreY - radius * 0.92f), accentColour);
    drawValue(irr::core::rect<irr::s32>(centreX - 90, centreY - 38, centreX + 90, centreY + 38), "HDG", formatFloat(headingDeg, 0), "deg", accentColour);
}

std::string InstrumentGUI::formatFloat(float value, int precision) const
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::string InstrumentGUI::formatSignedFloat(float value, int precision) const
{
    std::ostringstream stream;
    if (value > 0) {
        stream << "+";
    }
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

std::string InstrumentGUI::formatPercent(float value) const
{
    std::ostringstream stream;
    if (value > 0) {
        stream << "+";
    }
    stream << std::fixed << std::setprecision(0) << value * 100 << "%";
    return stream.str();
}

std::string InstrumentGUI::formatTime(uint64_t timestamp) const
{
    if (timestamp == 0) {
        return "--";
    }
    return Utilities::timestampToString((time_t)timestamp, "%d %b %Y %H:%M:%S UTC");
}

std::string InstrumentGUI::formatLatLong(float value, bool latitude) const
{
    char hemisphere;
    if (latitude) {
        hemisphere = value >= 0 ? 'N' : 'S';
    } else {
        hemisphere = value >= 0 ? 'E' : 'W';
    }

    float absValue = std::fabs(value);
    int degrees = (int)absValue;
    float minutes = (absValue - degrees) * 60.0f;

    std::ostringstream stream;
    stream << degrees << " " << std::fixed << std::setprecision(3) << minutes << "' " << hemisphere;
    return stream.str();
}

irr::core::rect<irr::s32> InstrumentGUI::inset(const irr::core::rect<irr::s32>& rect, irr::s32 amount) const
{
    return irr::core::rect<irr::s32>(
        rect.UpperLeftCorner.X + amount,
        rect.UpperLeftCorner.Y + amount,
        rect.LowerRightCorner.X - amount,
        rect.LowerRightCorner.Y - amount);
}
