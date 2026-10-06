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

#include "InstrumentData.hpp"

InstrumentData::InstrumentData()
{
    valid = false;
    version = 1;

    timestamp = 0;
    timeOffset = 0;
    scenarioTime = 0;
    accelerator = 0;

    longitudeDeg = 0;
    latitudeDeg = 0;
    posX = 0;
    posZ = 0;

    headingDeg = 0;
    cogDeg = 0;
    sogKts = 0;
    speedThroughWaterKts = 0;
    rateOfTurnDegPerMin = 0;

    depthM = 0;
    maxSounderDepthM = 0;
    depthBelowKeelM = 0;

    wheelDeg = 0;
    rudderDeg = 0;
    portEngineCommand = 0;
    stbdEngineCommand = 0;
    portEngineRpm = 0;
    stbdEngineRpm = 0;
    bowThruster = 0;
    sternThruster = 0;

    portSchottelDeg = 0;
    stbdSchottelDeg = 0;
    portAzimuthThrustLever = 0;
    stbdAzimuthThrustLever = 0;

    pitchDeg = 0;
    rollDeg = 0;

    weather = 0;
    rain = 0;
    visibilityNm = 0;
    windDirectionTrueDeg = 0;
    windSpeedKts = 0;
    apparentWindFromDeg = 0;
    apparentWindRelativeDeg = 0;
    apparentWindSpeedKts = 0;
    currentDirectionDeg = 0;
    currentSpeedKts = 0;
    tideHeightM = 0;

    hasGps = false;
    hasDepthSounder = false;
    isSingleEngine = false;
    isAzimuthDrive = false;
    isAzimuthAsternAllowed = false;
    hasBowThruster = false;
    hasSternThruster = false;
    hasTurnIndicator = false;
    streamOverride = false;
    portClutch = false;
    stbdClutch = false;
    rudderPump1 = false;
    rudderPump2 = false;
    emergencySteering = false;
}
