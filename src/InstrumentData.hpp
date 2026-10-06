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

#ifndef __INSTRUMENTDATA_HPP_INCLUDED__
#define __INSTRUMENTDATA_HPP_INCLUDED__

#include <stdint.h>

struct InstrumentData
{
    InstrumentData();

    bool valid;
    int version;

    uint64_t timestamp;
    uint64_t timeOffset;
    float scenarioTime;
    float accelerator;

    float longitudeDeg;
    float latitudeDeg;
    float posX;
    float posZ;

    float headingDeg;
    float cogDeg;
    float sogKts;
    float speedThroughWaterKts;
    float rateOfTurnDegPerMin;

    float depthM;
    float maxSounderDepthM;
    float depthBelowKeelM;

    float wheelDeg;
    float rudderDeg;
    float portEngineCommand;
    float stbdEngineCommand;
    float portEngineRpm;
    float stbdEngineRpm;
    float bowThruster;
    float sternThruster;

    float portSchottelDeg;
    float stbdSchottelDeg;
    float portAzimuthThrustLever;
    float stbdAzimuthThrustLever;

    float pitchDeg;
    float rollDeg;

    float weather;
    float rain;
    float visibilityNm;
    float windDirectionTrueDeg;
    float windSpeedKts;
    float apparentWindFromDeg;
    float apparentWindRelativeDeg;
    float apparentWindSpeedKts;
    float currentDirectionDeg;
    float currentSpeedKts;
    float tideHeightM;

    bool hasGps;
    bool hasDepthSounder;
    bool isSingleEngine;
    bool isAzimuthDrive;
    bool isAzimuthAsternAllowed;
    bool hasBowThruster;
    bool hasSternThruster;
    bool hasTurnIndicator;
    bool streamOverride;
    bool portClutch;
    bool stbdClutch;
    bool rudderPump1;
    bool rudderPump2;
    bool emergencySteering;
};

#endif
