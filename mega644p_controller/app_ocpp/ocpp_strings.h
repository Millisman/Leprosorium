#include "ocpp_parser.h"
#pragma once

extern const char str_Authorize[];
extern const char str_BootNotification[];
extern const char str_Heartbeat[];
extern const char str_MeterValues[];
extern const char str_SetChargingProfile[];
extern const char str_StatusNotification[];
extern const char str_StartTransaction[];
extern const char str_StopTransaction[];
extern const char str_TriggerMessage[];
extern const char str_RemoteStartTransaction[];
extern const char str_RemoteStopTransaction[];
extern const char str_ChangeConfiguration[];
extern const char str_GetConfiguration[];
extern const char str_Reset[];
extern const char str_UpdateFirmware[];
extern const char str_FirmwareStatusNotification[];
extern const char str_GetDiagnostics[];
extern const char str_DiagnosticsStatusNotification[];
extern const char str_UnlockConnector[];
extern const char str_ClearChargingProfile[];
extern const char str_ChangeAvailability[];
extern const char str_CancelReservation[];
extern const char str_ClearCache[];
extern const char str_DataTransfer[];
extern const char str_GetLocalListVersion[];
extern const char str_GetCompositeSchedule[];
extern const char str_ReserveNow[];
extern const char str_SendLocalList[];
extern const char str_Pending[];
extern const char str_Rejected[];
extern const char str_Inoperative[];
extern const char str_Operative[];
extern const char str_Undefined[];
extern const char str_Scheduled[];
extern const char str_Available[];
extern const char str_Preparing[];
extern const char str_Charging[];
extern const char str_SuspendedEVSE[];
extern const char str_SuspendedEV[];
extern const char str_Finishing[];
extern const char str_Reserved[];
extern const char str_Unavailable[];
extern const char str_Faulted[];
extern const char str_ConnectorLockFailure[];
extern const char str_EVCommunicationError[];
extern const char str_GroundFailure[];
extern const char str_HighTemperature[];
extern const char str_InternalError[];
extern const char str_LocalListConflict[];
extern const char str_NoError[];
extern const char str_OtherError[];
extern const char str_OverCurrentFailure[];
extern const char str_OverVoltage[];
extern const char str_PowerMeterFailure[];
extern const char str_PowerSwitchFailure[];
extern const char str_ReaderFailure[];
extern const char str_ResetFailure[];
extern const char str_UnderVoltage[];
extern const char str_WeakSignal[];
extern const char str_Accepted[];
extern const char str_Blocked[];
extern const char str_Expired[];
extern const char str_Invalid[];
extern const char str_ConcurrentTx[];
extern const char str_DeAuthorized[];
extern const char str_EmergencyStop[];
extern const char str_EVDisconnected[];
extern const char str_HardReset[];
extern const char str_Local[];
extern const char str_Other[];
extern const char str_PowerLoss[];
extern const char str_Reboot[];
extern const char str_Remote[];
extern const char str_SoftReset[];
extern const char str_UnlockCommand[];
extern const char str_status[];
extern const char str_currentTime[];
extern const char str_interval[];
extern const char str_idTag[];
extern const char str_connectorId[];
extern const char str_errorCode[];

// extern const char      str_http_11[];
// extern const char      str_http_10[];
// extern const char  str_SecWsAccept[];
// // // // extern const char    str_websocket[];
// // // // // extern const char   str_Connection[];
// // // // // extern const char      str_upgrade[];
// extern const char         str_UUID[];


// // // // // // extern const char   str_chargePointVendor[];
// // // // // // extern const char   str_chargePointModel[];
// // // // // // extern const char   str_chargePointSerialNumber[];
// // // // // // extern const char   str_chargeBoxSerialNumber[];
// // // // // // extern const char   str_firmwareVersion[];
// // // // // // extern const char   str_iccid[];
// // // // // // extern const char   str_imsi[];
// // // // // // extern const char   str_meterSerialNumber[];
// // // // // // extern const char   str_meterType[];

// extern const char WSH_GET[];
// extern const char WSH_HTTP[];
// extern const char WSH_KEY[];
// extern const char WSH_EOL2[];

extern const char   str_reason[];
extern const char   str_transactionId[];
extern const char   str_meterStart[];
extern const char   str_meterStop[];
extern const char   str_timestamp[];




#define REGISTRATIONSTATUS_COUNT 3
#define AUTHORIZATIONSTATUS_COUNT 5
