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

#include "InstrumentNetworkCodec.hpp"

#include <vector>

#include "Utilities.hpp"

namespace
{
    std::string boolToString(bool value)
    {
        return value ? "1" : "0";
    }

    bool stringToBool(const std::string& value)
    {
        return value == "1";
    }

    void appendField(std::string& message, const std::string& field)
    {
        if (!message.empty()) {
            message.append(",");
        }
        message.append(field);
    }

    template<typename T>
    void appendField(std::string& message, T value)
    {
        appendField(message, Utilities::lexical_cast<std::string>(value));
    }
}

namespace InstrumentNetworkCodec
{
    std::string serialize(const InstrumentData& data)
    {
        std::string body;

        appendField(body, data.version);
        appendField(body, data.timestamp);
        appendField(body, data.timeOffset);
        appendField(body, data.scenarioTime);
        appendField(body, data.accelerator);
        appendField(body, data.longitudeDeg);
        appendField(body, data.latitudeDeg);
        appendField(body, data.posX);
        appendField(body, data.posZ);
        appendField(body, data.headingDeg);
        appendField(body, data.cogDeg);
        appendField(body, data.sogKts);
        appendField(body, data.speedThroughWaterKts);
        appendField(body, data.rateOfTurnDegPerMin);
        appendField(body, data.depthM);
        appendField(body, data.maxSounderDepthM);
        appendField(body, data.depthBelowKeelM);
        appendField(body, data.wheelDeg);
        appendField(body, data.rudderDeg);
        appendField(body, data.portEngineCommand);
        appendField(body, data.stbdEngineCommand);
        appendField(body, data.portEngineRpm);
        appendField(body, data.stbdEngineRpm);
        appendField(body, data.bowThruster);
        appendField(body, data.sternThruster);
        appendField(body, data.portSchottelDeg);
        appendField(body, data.stbdSchottelDeg);
        appendField(body, data.portAzimuthThrustLever);
        appendField(body, data.stbdAzimuthThrustLever);
        appendField(body, data.pitchDeg);
        appendField(body, data.rollDeg);
        appendField(body, data.weather);
        appendField(body, data.rain);
        appendField(body, data.visibilityNm);
        appendField(body, data.windDirectionTrueDeg);
        appendField(body, data.windSpeedKts);
        appendField(body, data.apparentWindFromDeg);
        appendField(body, data.apparentWindRelativeDeg);
        appendField(body, data.apparentWindSpeedKts);
        appendField(body, data.currentDirectionDeg);
        appendField(body, data.currentSpeedKts);
        appendField(body, data.tideHeightM);
        appendField(body, boolToString(data.hasGps));
        appendField(body, boolToString(data.hasDepthSounder));
        appendField(body, boolToString(data.isSingleEngine));
        appendField(body, boolToString(data.isAzimuthDrive));
        appendField(body, boolToString(data.isAzimuthAsternAllowed));
        appendField(body, boolToString(data.hasBowThruster));
        appendField(body, boolToString(data.hasSternThruster));
        appendField(body, boolToString(data.hasTurnIndicator));
        appendField(body, boolToString(data.streamOverride));
        appendField(body, boolToString(data.portClutch));
        appendField(body, boolToString(data.stbdClutch));
        appendField(body, boolToString(data.rudderPump1));
        appendField(body, boolToString(data.rudderPump2));
        appendField(body, boolToString(data.emergencySteering));

        return PREFIX + body;
    }

    bool deserialize(const std::string& message, InstrumentData& data)
    {
        if (message.size() <= PREFIX.size() || message.substr(0, PREFIX.size()) != PREFIX) {
            return false;
        }

        std::vector<std::string> fields = Utilities::split(message.substr(PREFIX.size()), ',');
        if (fields.size() < 56) {
            return false;
        }

        InstrumentData parsed;
        unsigned int field = 0;
        parsed.version = Utilities::lexical_cast<int>(fields.at(field++));
        parsed.timestamp = Utilities::lexical_cast<uint64_t>(fields.at(field++));
        parsed.timeOffset = Utilities::lexical_cast<uint64_t>(fields.at(field++));
        parsed.scenarioTime = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.accelerator = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.longitudeDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.latitudeDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.posX = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.posZ = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.headingDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.cogDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.sogKts = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.speedThroughWaterKts = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.rateOfTurnDegPerMin = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.depthM = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.maxSounderDepthM = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.depthBelowKeelM = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.wheelDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.rudderDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.portEngineCommand = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.stbdEngineCommand = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.portEngineRpm = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.stbdEngineRpm = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.bowThruster = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.sternThruster = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.portSchottelDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.stbdSchottelDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.portAzimuthThrustLever = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.stbdAzimuthThrustLever = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.pitchDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.rollDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.weather = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.rain = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.visibilityNm = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.windDirectionTrueDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.windSpeedKts = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.apparentWindFromDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.apparentWindRelativeDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.apparentWindSpeedKts = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.currentDirectionDeg = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.currentSpeedKts = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.tideHeightM = Utilities::lexical_cast<float>(fields.at(field++));
        parsed.hasGps = stringToBool(fields.at(field++));
        parsed.hasDepthSounder = stringToBool(fields.at(field++));
        parsed.isSingleEngine = stringToBool(fields.at(field++));
        parsed.isAzimuthDrive = stringToBool(fields.at(field++));
        parsed.isAzimuthAsternAllowed = stringToBool(fields.at(field++));
        parsed.hasBowThruster = stringToBool(fields.at(field++));
        parsed.hasSternThruster = stringToBool(fields.at(field++));
        parsed.hasTurnIndicator = stringToBool(fields.at(field++));
        parsed.streamOverride = stringToBool(fields.at(field++));
        parsed.portClutch = stringToBool(fields.at(field++));
        parsed.stbdClutch = stringToBool(fields.at(field++));
        parsed.rudderPump1 = stringToBool(fields.at(field++));
        parsed.rudderPump2 = stringToBool(fields.at(field++));
        parsed.emergencySteering = stringToBool(fields.at(field++));
        parsed.valid = true;

        data = parsed;
        return true;
    }
}
