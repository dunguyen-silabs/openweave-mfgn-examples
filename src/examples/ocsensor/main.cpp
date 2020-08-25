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

/**
 * @file
 *   This file implements the main function for the "ocsensor" example application.
 */

#include "HardwarePlatform.h"
#include "AppTask.h"
#include "DeviceController.h"

#include <stdbool.h>
#include <stdint.h>

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/tasklet.h>
#include <openthread/link.h>
#include <openthread/dataset.h>
#include <openthread/error.h>
#include <openthread/icmp6.h>
#include <openthread/platform/openthread-system.h>

#include <Weave/DeviceLayer/WeaveDeviceLayer.h>
#include <Weave/DeviceLayer/ThreadStackManager.h>
#include <Weave/DeviceLayer/internal/testing/ConfigUnitTest.h>
#include <Weave/DeviceLayer/internal/testing/GroupKeyStoreUnitTest.h>
#include <Weave/DeviceLayer/internal/testing/SystemClockUnitTest.h>

// Thread Polling Configuration.
#define THREAD_ACTIVE_POLLING_INTERVAL_MS 100
#define THREAD_INACTIVE_POLLING_INTERVAL_MS 1000

using namespace ::nl;
using namespace ::nl::Inet;
using namespace ::nl::Weave;
using namespace ::nl::Weave::DeviceLayer;

void SuccessOrAbort(WEAVE_ERROR retCode, const char * msg)
{
    if (retCode != WEAVE_NO_ERROR)
    {
        WeaveLogError(Support, msg);
        WeaveLogError(Support, "******* ERROR CODE [%d] *******", retCode);
        WeaveDie();
    }
}

int main(void)
{
    WEAVE_ERROR ret;

    // Platform-specific initializations. Weave logging not setup yet.
    GetHardwarePlatform().Init();

    otSysInit(0, NULL); // This must go here for efr32 (i.e. before either OW or OT stack inits)
    WeaveLogProgress(Support, "Initializing the Weave stack");
    ret = PlatformMgr().InitWeaveStack();
    SuccessOrAbort(ret, "PlatformMgr().InitWeaveStack() failed.");

    WeaveLogProgress(Support, "Initializing the OpenThread stack");
    ret = ThreadStackMgr().InitThreadStack();
    SuccessOrAbort(ret, "ThreadStackMgr().InitThreadStack() failed.");

    // Configure device to operate as a Thread sleepy end-device.
    ret = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_SleepyEndDevice); // FIXME:
    SuccessOrAbort(ret, "ConnectivityMgr().SetThreadDeviceType() failed.");

    // Configure the Thread polling behavior for the device.
    {
        ConnectivityManager::ThreadPollingConfig pollingConfig;
        pollingConfig.Clear();
        pollingConfig.ActivePollingIntervalMS   = THREAD_ACTIVE_POLLING_INTERVAL_MS;
        pollingConfig.InactivePollingIntervalMS = THREAD_INACTIVE_POLLING_INTERVAL_MS;
        ret                                     = ConnectivityMgr().SetThreadPollingConfig(pollingConfig);
        SuccessOrAbort(ret, "ConnectivityMgr().SetThreadPollingConfig() failed.");
    }

    WeaveLogProgress(Support, "Starting the Weave task");
    ret = PlatformMgr().StartEventLoopTask();
    SuccessOrAbort(ret, "PlatformMgr().StartEventLoopTask() failed.");

    WeaveLogProgress(Support, "Starting the OpenThread task");
    ret = ThreadStackMgrImpl().StartThreadTask();
    SuccessOrAbort(ret, "ThreadStackMgr().StartThreadTask() failed.");

    WeaveLogProgress(Support, "Initializing the DeviceController");
    GetDeviceController().Init();

    // Start the Application Task.
    // Method to be called on every cycle of the event loop is provided as argument.
    WeaveLogProgress(Support, "Starting the Application Task");
    ret = GetAppTask().StartAppTask(DeviceController::EventLoopCycle);
    SuccessOrAbort(ret, "GetAppTask().Init() failed.");

    /*
     * make a specific address unique to my thread network...
     * won't get woken up...
     *
     * other forms of multicast --> other form of group id.
     * use mesh local prefix...
     *
     * swith device back as routers...
     * don't make it a sleepy device and that should work...
     * for scope:
     * setthreaddevicetype -->
     *
     */

    /* FIXME - no platform specifics in here!
    // Activate deep sleep mode
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    {
        struct mallinfo minfo = mallinfo();
        NRF_LOG_INFO("System Heap Utilization: heap size %" PRId32 ", arena size %" PRId32 ", in use %" PRId32 ", free %" PRId32,
                GetHeapTotalSize(), minfo.arena, minfo.uordblks, minfo.fordblks);
    }
     */

    WeaveLogProgress(Support, "Starting the FreeRTOS scheduler");
    vTaskStartScheduler();

    //--------------------------------------------------------------------------

    // Should never get here
    WeaveLogProgress(Support, "vTaskStartScheduler() failed");
    WeaveDie();
}
