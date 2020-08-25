/*
 *
 *    Copyright (c) 2020 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "LED.h"
#include "HardwarePlatform.h"

#include <Weave/DeviceLayer/WeaveDeviceLayer.h>

void LED::Init(PlatformLED * platformLEDPtr)
{
    mPlatformLEDPtr = platformLEDPtr;

    mLastChangeTimeUS = 0;
    mBlinkOnTimeMS    = 0;
    mBlinkOffTimeMS   = 0;

    mState = true; // Forces the setting of the LED.
    Set(false);
}

void LED::Invert(void)
{
    Set(!mState);
}

void LED::Set(bool state)
{
    mBlinkOnTimeMS  = 0;
    mBlinkOffTimeMS = 0;
    mLastChangeTimeUS = ::nl::Weave::System::Platform::Layer::GetClock_MonotonicHiRes();
    DoSet(state);
}

void LED::Blink(uint32_t changeRateMS)
{
    Blink(changeRateMS, changeRateMS);
}

void LED::Blink(uint32_t onTimeMS, uint32_t offTimeMS)
{
    mBlinkOnTimeMS  = onTimeMS;
    mBlinkOffTimeMS = offTimeMS;
    Animate();
}

void LED::Animate()
{
    if (mBlinkOnTimeMS != 0 && mBlinkOffTimeMS != 0)
    {
        int64_t nowUS            = ::nl::Weave::System::Platform::Layer::GetClock_MonotonicHiRes();
        int64_t stateDurUS       = ((mState) ? mBlinkOnTimeMS : mBlinkOffTimeMS) * 1000LL;
        int64_t nextChangeTimeUS = mLastChangeTimeUS + stateDurUS;

        if (nowUS > nextChangeTimeUS)
        {
            DoSet(!mState);
            mLastChangeTimeUS = nowUS;
        }
    }
}

void LED::DoSet(bool state)
{
    if (mState == state)
        return;

    mState = state;
    if (state)
    {
        mPlatformLEDPtr->On();
    }
    else
    {
        mPlatformLEDPtr->Off();
    }
}
