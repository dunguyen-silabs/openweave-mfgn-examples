/*
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
 *    @file
 *      A trait data sink implementation for the Weave Security BoltLockSettingsTrait.
 */

#include "BoltLockSettingsTraitDataSink.h"
#include "BoltLockSettingsTrait.h"

#include "DeviceController.h"

using namespace nl::Weave::TLV;
using namespace nl::Weave::Profiles::DataManagement;

using namespace Schema::Weave::Trait::Security;
using namespace Schema::Weave::Trait::Security::BoltLockSettingsTrait;

BoltLockSettingsTraitDataSink::BoltLockSettingsTraitDataSink() : TraitDataSink(&BoltLockSettingsTrait::TraitSchema) {}

WEAVE_ERROR
BoltLockSettingsTraitDataSink::SetLeafData(PropertyPathHandle aLeafHandle, TLVReader & aReader)
{
    WEAVE_ERROR err = WEAVE_NO_ERROR;

    switch (aLeafHandle)
    {
    case BoltLockSettingsTrait::kPropertyHandle_AutoRelockOn: {
        bool auto_relock_on;
        err = aReader.Get(auto_relock_on);
        nlREQUIRE_SUCCESS(err, exit);

        GetDeviceController().EnableAutoLock(auto_relock_on);

        WeaveLogProgress(Support, "Auto Relock %s", (auto_relock_on) ? "ENABLED" : "DISABLED");
        break;
    }

    case BoltLockSettingsTrait::kPropertyHandle_AutoRelockDuration: {
        uint32_t auto_lock_duration = 0;
        err                         = aReader.Get(auto_lock_duration);
        nlREQUIRE_SUCCESS(err, exit);

        GetDeviceController().SetAutoLockDuration(auto_lock_duration);

        WeaveLogProgress(Support, "Auto Relock Duration (secs): %u", auto_lock_duration);
        break;
    }

    default:
        break;
    }

exit:
    return err;
}
