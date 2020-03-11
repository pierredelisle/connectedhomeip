/*
 *
 *    <COPYRIGHT>
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
 *      Implementation of the fault-injection utilities for chip.
 */

#include <string.h>
#include <nlassert.h>

#include <support/CHIPFaultInjection.h>

#if CHIP_CONFIG_TEST

namespace chip {
namespace FaultInjection {

static FaultInjection::Record sFaultRecordArray[kFault_NumItems];
static int32_t sFault_WDMNotificationSize_Arguments[1];
static int32_t sFault_FuzzExchangeHeader_Arguments[1];
static class FaultInjection::Manager schipFaultInMgr;
static const FaultInjection::Name sManagerName = "chip";
static const FaultInjection::Name sFaultNames[] = {
    "AllocExchangeContext",
    "DropIncomingUDPMsg",
    "DropOutgoingUDPMsg",
    "AllocBinding",
    "SendAlarm",
    "HandleAlarm",
    "FuzzExchangeHeaderTx",
#if CHIP_CONFIG_ENABLE_RELIABLE_MESSAGING
    "WRMDoubleTx",
    "WRMSendError",
#endif // CHIP_CONFIG_ENABLE_RELIABLE_MESSAGING
    "BDXBadBlockCounter",
    "BDXAllocTransfer",
#if CHIP_CONFIG_ENABLE_SERVICE_DIRECTORY
    "SMConnectRequestNew",
    "SMLookup",
    "SMCacheReplaceEntryError",
#endif // CHIP_CONFIG_ENABLE_SERVICE_DIRECTORY
    "WDMTraitInstanceNew",
    "WDMSubscriptionHandlerNew",
    "WDMSubscriptionClientNew",
    "WDMBadSubscriptionId",
    "WDMSendUnsupportedReqMsgType",
    "WDMNotificationSize",
    "WDMSendCommandExpired",
    "WDMSendCommandBadVersion",
    "WDMSendUpdateBadVersion",
    "WDMDelayUpdateResponse",
    "WDMUpdateRequestTimeout",
    "WDMUpdateRequestSendErrorInline",
    "WDMUpdateRequestSendErrorAsync",
    "WDMUpdateRequestBadProfile",
    "WDMUpdateRequestDropMessage",
    "WDMUpdateResponseBusy",
    "WDMPathStoreFull",
    "WDMTreatNotifyAsCancel",
    "CASEKeyConfirm",
    "SecMgrBusy",
#if CHIP_CONFIG_ENABLE_TUNNELING
    "TunnelQueueFull",
    "TunnelPacketDropByPolicy",
#endif // CHIP_CONFIG_ENABLE_TUNNELING
#if CONFIG_NETWORK_LAYER_BLE
    "WOBLESend",
#endif // CONFIG_NETWORK_LAYER_BLE
};


/**
 * Get the singleton FaultInjection::Manager for Inet faults
 */
FaultInjection::Manager &GetManager(void)
{
    if (0 == schipFaultInMgr.GetNumFaults())
    {
        schipFaultInMgr.Init(kFault_NumItems,
                              sFaultRecordArray,
                              sManagerName,
                              sFaultNames);
        memset(&sFault_WDMNotificationSize_Arguments, 0, sizeof(sFault_WDMNotificationSize_Arguments));
        sFaultRecordArray[kFault_WDM_NotificationSize].mArguments = sFault_WDMNotificationSize_Arguments;
        sFaultRecordArray[kFault_WDM_NotificationSize].mLengthOfArguments =
            static_cast<uint8_t>(sizeof(sFault_WDMNotificationSize_Arguments)/sizeof(sFault_WDMNotificationSize_Arguments[0]));

        memset(&sFault_FuzzExchangeHeader_Arguments, 0, sizeof(sFault_FuzzExchangeHeader_Arguments));
        sFaultRecordArray[kFault_FuzzExchangeHeaderTx].mArguments = sFault_FuzzExchangeHeader_Arguments;
        sFaultRecordArray[kFault_FuzzExchangeHeaderTx].mLengthOfArguments =
            static_cast<uint8_t>(sizeof(sFault_FuzzExchangeHeader_Arguments)/sizeof(sFault_FuzzExchangeHeader_Arguments[0]));

    }
    return schipFaultInMgr;
}

/**
 * Fuzz a byte of a chip Exchange Header
 *
 * @param[in] p     Pointer to the encoded Exchange Header
 * @param[in] arg   An index from 0 to (CHIP_FAULT_INJECTION_NUM_FUZZ_VALUES * 5 -1)
 *                  that specifies the byte to be corrupted and the value to use.
 */
DLL_EXPORT void FuzzExchangeHeader(uint8_t *p, int32_t arg)
{
    // chip is little endian; this function alters the
    // least significant byte of the header fields.
    const uint8_t offsets[] = {
        0, // flags and version
        1, // MessageType
        2, // ExchangeId
        4, // ProfileId
        8  // AckMsgId
    };
    const uint8_t values[CHIP_FAULT_INJECTION_NUM_FUZZ_VALUES] = { 0x1, 0x2, 0xFF };
    size_t offsetIndex = 0;
    size_t valueIndex = 0;
    size_t numOffsets = sizeof(offsets)/sizeof(offsets[0]);
    offsetIndex = arg % (numOffsets);
    valueIndex = (arg / numOffsets) % CHIP_FAULT_INJECTION_NUM_FUZZ_VALUES;
    p[offsetIndex] ^= values[valueIndex];
}

} // namespace FaultInjection
} // namespace chip

#endif // CHIP_CONFIG_TEST