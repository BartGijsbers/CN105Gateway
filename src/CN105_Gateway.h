#include "Arduino.h"
#define packetBufferSize 23

static const int HEADER_LEN = 8;
const byte HEADER[HEADER_LEN] = {0xfc, 0x41, 0x01, 0x30, 0x10, 0x01, 0x00, 0x00};
const int RCVD_PKT_FAIL = 0;
const int RCVD_PKT_CONNECT_SUCCESS = 1;

typedef uint8_t byte;

struct item
{
    byte PacketType;
    byte Command;
    byte VarIndex;
    byte VarType;
    char VarShortName[5];
    char VarLongName[35];
    char noSpacesName[35];
};

const byte VarType_TIME_DATE = 0;
const byte VarType_TEMPERATURE = 1;
const byte VarType_ONE_BYTE_TEMPERATURE = 2;
const byte VarType_POWER_STATE = 3;
const byte VarType_OPERATION_MODE = 4; 
const byte VarType_WATERFLOW = 5; 
const byte VarType_HW_MODE = 6; 
const byte VarType_MODE_SETTING = 7;
const byte VarType_HEAT_COOL = 8;
const byte VarType_DEFROST = 9;
const byte VarType_HEXVALUE = 10;
const byte VarType_DECVALUE = 11;
const byte VarType_RUNTIME = 12;
const byte VarType_ONE_BYTE_TEMPERATURE_20 = 13;
const byte VarType_DATE = 14;
const byte VarType_3BYTEVALUE = 15;
const byte VarType_2BYTEHEXVALUE = 16;
const byte VarType_OnOff = 17;

const byte SET_REQUEST = 0x41;               // to wp
const byte SET_RESPONSE = 0x61;              // from wp
const byte GET_REQUEST = 0x42;               // to wp
const byte GET_RESPONSE = 0x62;              // from wp
const byte CONNECT_REQUEST = 0x5A;           // to wp
const byte CONNECT_RESPONSE = 0x7A;          // from wp
const byte EXTENDED_CONNECT_REQUEST = 0x5B;  // to wp
const byte EXTENDED_CONNECT_RESPONSE = 0x7B; // from wp

// holds all command entry's which will be requested of the ecodan
byte commandEntrys[] = {0x01, 0x02, 0x03, 0x04, 0x07, 0x09, 0x0b, 0x0c, 0x0d, 0x0e, 0x11, 0x13, 0x14, 0x15, 0x26, 0x28, 0x29, 0xa1, 0xa2};
// MelCloud requests:: 1,2,3,4,5,6,7,9,b,c,d,e,10,11,13,14,15,16,17,18,19,1a,1c,1d,1e,1f,20,26,27,28,29,a1,a2

// holds all known value's and there type (used for decoding) 
// if new value's and type are found, please add them here
item items[] = {
    {GET_RESPONSE, 0x01, 1+5, VarType_TIME_DATE, "Time", "Time & Date", "Time_Date"},
    {GET_RESPONSE, 0x02, 3+5, VarType_DEFROST, "Defr", "Defrost", "Defrost"},
    {GET_RESPONSE, 0x03, 2+5, VarType_2BYTEHEXVALUE, "ErrC", "Error Code", "Error_Code"},
    {GET_RESPONSE, 0x04, 1+5, VarType_DECVALUE, "Freq", "Frequency value in Hz?", "Frequency"},
    {GET_RESPONSE, 0x07, 4+5, VarType_DECVALUE, "HeSt", "Heating stage?", "Heating_Stage"},
    {GET_RESPONSE, 0x07, 6+5, VarType_DECVALUE, "Kw  ", "Output power kw", "Ouput_Power_Kw"},
    {GET_RESPONSE, 0x09, 1+5, VarType_TEMPERATURE, "Z1Tp", "Zone 1 Room Temp Setpoint", "Zone1_Room_Temp_Setpoint"},
    {GET_RESPONSE, 0x09, 3+5, VarType_TEMPERATURE, "Z2Tp", "Zone 2 Room Temp Setpoint", "Zone2_Room_Temp_Setpoint"},
    {GET_RESPONSE, 0x09, 5+5, VarType_TEMPERATURE, "FlSp", "Zone 1 Flow Temp Setpoint", "Zone1_Flow_Temp_Setpoint"},
    {GET_RESPONSE, 0x09, 7+5, VarType_TEMPERATURE, "FlTp", "Zone 2 Flow Temp SetPoint", "Zone2_Flow_Temp_Setpoint"},
    {GET_RESPONSE, 0x09, 9+5, VarType_TEMPERATURE, "HWTp", "Legionella Temp Setpoint", "Legionella_Temp_Setpoint"},
    {GET_RESPONSE, 0x09, 11+5, VarType_DECVALUE, "HWTD", "Hot Water Max Temp Drop", "Hot_Water_Max_Temp_Drop"},
    {GET_RESPONSE, 0x09, 12+5, VarType_ONE_BYTE_TEMPERATURE_20, "HWTp", "Flow temp range max", "Flow_Temp_Range_Max"},
    {GET_RESPONSE, 0x09, 13+5, VarType_ONE_BYTE_TEMPERATURE_20, "HWTp", "Flow temp range min", "Flow_Temp_Range_Min"},
    {GET_RESPONSE, 0x0b, 1+5, VarType_TEMPERATURE, "Z1Tp", "Zone 1 Room Temp", "Zone1_Room_Temp"},
    {GET_RESPONSE, 0x0b, 8+5, VarType_TEMPERATURE, "GaTp", "Gas Return Temp", "Gas_Return_Temp"},
    {GET_RESPONSE, 0x0b, 10+5, VarType_ONE_BYTE_TEMPERATURE, "GaTp", "Gas Return Temp Signed", "Gas_Return_Temp_Signed"},
    {GET_RESPONSE, 0x0b, 11+5, VarType_ONE_BYTE_TEMPERATURE, "OtTp", "Outside Temperature", "Outside_Temperature"},
    {GET_RESPONSE, 0x0c, 1+5, VarType_TEMPERATURE, "HTpF", "Water Feed Temp", "Water_Feed_Temp"},
    {GET_RESPONSE, 0x0c, 3+5, VarType_ONE_BYTE_TEMPERATURE, "HTpF", "Water Feed Temp Signed", "Water_Feed_Temp_Signed"},
    {GET_RESPONSE, 0x0c, 4+5, VarType_TEMPERATURE, "HTpR", "Water Return Temp", "Water_Return_Temp"},
    {GET_RESPONSE, 0x0c, 6+5, VarType_ONE_BYTE_TEMPERATURE, "HTpR", "Water Return Temp Signed", "Water_Return_Temp_Signed"},
    {GET_RESPONSE, 0x0c, 7+5, VarType_TEMPERATURE, "WaTp", "Water Temperature", "Water_Temperature"},
    {GET_RESPONSE, 0x0d, 1+5, VarType_TEMPERATURE, "BTpF", "Boiler Flow Temperature", "Boiler_Flow_Temperature"},
    {GET_RESPONSE, 0x0d, 4+5, VarType_TEMPERATURE, "BTpR", "Boiler Return Temperature", "Boiler_Return_Temperature"},
    {GET_RESPONSE, 0x0d, 7+5, VarType_TEMPERATURE, "BTpF", "Unknown Temp 0x0d pos 7", "Unknown1"},
    {GET_RESPONSE, 0x0d, 10+5, VarType_TEMPERATURE, "BTpR", "Unknown Temp 0x0d pos 10", "Unknown2"},
    {GET_RESPONSE, 0x0e, 1+5, VarType_TEMPERATURE, "BTpF", "Unknown Temp 0x0e pos 1", "Unknown3"},
    {GET_RESPONSE, 0x0e, 4+5, VarType_TEMPERATURE, "BTpR", "Unknown Temp 0x0e pos 4", "Unknown4"},
    {GET_RESPONSE, 0x11, 3+5, VarType_ONE_BYTE_TEMPERATURE, "Unkn", "Unknown Temp Signed 0x11 pos 3", "Unknown5"},
    {GET_RESPONSE, 0x13, 3+5, VarType_RUNTIME, "RunT", "HeatPump Runtime", "HeatPump_Runtime"},
    {GET_RESPONSE, 0x14, 12+5, VarType_DECVALUE, "Flow", "Water Flow l/m", "Water_Flow_lm"},
    {GET_RESPONSE, 0x15, 3+5, VarType_DECVALUE, "Unkn", "cmnd=0x15 pos=3", "Unknown6"},
    {GET_RESPONSE, 0x26, 3+5, VarType_POWER_STATE, "PwrS", "Power State", "Power_State"},
    {GET_RESPONSE, 0x26, 4+5, VarType_OPERATION_MODE, "OpMo", "Operating Mode", "Operating_Mode"},
    {GET_RESPONSE, 0x26, 5+5, VarType_HW_MODE, "HWMo", "Hot Water Mode", "Hot_Water_Mode"},
    {GET_RESPONSE, 0x26, 6+5, VarType_MODE_SETTING, "MoSe", "A/C Mode Zone 1", "AC_Mode_Zone1"}, //Operating mode room/flow/curve
    {GET_RESPONSE, 0x26, 7+5, VarType_HEAT_COOL, "HeCo", "Heating/Cooling", "Heating_Cooling"},
    {GET_RESPONSE, 0x26, 8+5, VarType_TEMPERATURE, "HWSp", "Hot Water Setpoint", "Hot_Water_Setpoint"},
    {GET_RESPONSE, 0x26, 10+5, VarType_TEMPERATURE, "EFSp", "Flow SetPoint Zone 1", "Flow_Setpoint_Zone1"},
    {GET_RESPONSE, 0x26, 12+5, VarType_TEMPERATURE, "EFTp", "Flow Setpoint Zone 2", "Flow_Setpoint_Zone2"}, 
    {GET_RESPONSE, 0x28, 4+5, VarType_OnOff, "Holi", "Holiday Mode", "Holiday_Mode"},
    {GET_RESPONSE, 0x28, 5+5, VarType_OnOff, "HWTi", "Hot Water Timer", "Hot_Water_Timer"},
    {GET_RESPONSE, 0x29, 4+5, VarType_TEMPERATURE, "Unkn", "Zone 1 Room Temp Setpoint2", "Zone1_Room_Temp_Setpoint2"},
    {GET_RESPONSE, 0x29, 6+5, VarType_TEMPERATURE, "Unkn", "Zone 2 Room Temp Setpoint2", "Zone2_Room_Temp_Setpoint2"},
    {GET_RESPONSE, 0xa1, 1+5, VarType_DATE, "Date", "Date Consumed Energy", "Date_Consumed_Energy"},
    {GET_RESPONSE, 0xa1, 4+5, VarType_3BYTEVALUE, "Ener", "Consumed Energy Yesterday Kw", "Consumed_Energy_Yesterday_Kw"},
    {GET_RESPONSE, 0xa2, 1+5, VarType_DATE, "Date", "Date Produced Energy", "Date_Produced_Energy"},
    {GET_RESPONSE, 0xa2, 4+5, VarType_3BYTEVALUE, "Date", "Produced Energy Yesterday Kw", "Produced_Energy_Yesterday_Kw"},
    {0, 0, 0, 0, " ", " "}
};


// working on the variables below
// this structe is hard to use becuase some setting requires more then one setting 

const byte SetType_TEMPERATURE = 1;
const byte SetType_MODE_SETTING = 2;
const byte SetType_ON_OFF = 3;
const byte SetType_None = 4;

const byte SetCommand_SetFlowTemperatureZone1 = 1;
const byte SetCommand_SetRoomTemperatureZone1 = 2;
const byte SetCommand_SetBoilerTemperature = 3;
const byte SetCommand_SetACModeZone1 = 4;
const byte SetCommand_SetOnOff = 5;
const byte SetCommand_Nothing = 6;

struct commandItem
{
    byte VarIndex;
    byte VarType;
    byte SetType;
    char VarLongName[35];
    byte packetMask[packetBufferSize];
};

commandItem commandItems[] = {
    {3+5, SetType_ON_OFF, SetCommand_SetOnOff, "SetSystemModeOnOff",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {5+5, SetType_None, SetCommand_SetOnOff, "SetHotWaterMode",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {6+5, SetType_MODE_SETTING, SetCommand_SetACModeZone1, "SetACMode",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {8+5, SetType_TEMPERATURE, SetCommand_SetBoilerTemperature, "SetBoilerTemperature",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {10+5, SetType_TEMPERATURE, SetCommand_SetFlowTemperatureZone1, "SetFlowTemperature",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {10+5, SetType_TEMPERATURE, SetCommand_SetRoomTemperatureZone1, "SetRoomTemperature",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x32, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {3+5, SetType_None, SetCommand_SetOnOff, "ForceDHW",
        {0xfc, 0x41, 0x02, 0x7a, 0x10, 0x34, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {0, 0, 0, " ",
        {0x00, 0x00, 0x02, 0x7a, 0x10, 0x32, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
};
