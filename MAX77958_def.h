#ifndef MAX77958_DEF_H
#define MAX77958__DEF_H

#include <stdint.h>


#define MAX77958_ADDRESS 0x27	//Default Address when GPIO 6 is left floating
#define MAX77958_DEV_ID 0x58	//Always 0x58 for current version of silicon

#define AP_DATA_SIZE	33		//Maximum number of AP bytes to read or write
#define AP_MAX_PDOS		8		//Maximum number of PDOs for a USB-C device


/*** MAX77958 Register Addresses ***/
typedef enum  {
	DEVICE_ID    	= 0x00,
	DEVICE_REV   	= 0x01,
	FW_REV       	= 0x02,
	FW_SUB_VER   	= 0x03,
	UIC_INT      	= 0x04,
	CC_INT       	= 0x05,
	PD_INT       	= 0x06,
	ACTION_INT   	= 0x07,
	USBC_STATUS1 	= 0x08,
	USBC_STATUS2 	= 0x09,
	BC_STATUS    	= 0x0A,
	DP_STATUS    	= 0x0B,
	CC_STATUS0   	= 0x0C,
	CC_STATUS1   	= 0x0D,
	PD_STATUS0   	= 0x0E,
	PD_STATUS1   	= 0x0F,
	UIC_INT_M    	= 0x10,
	CC_INT_M     	= 0x11,
	PD_INT_M     	= 0x12,
	ACTION_INT_M 	= 0x13,
	OPCODE_IN_S  	= 0x21,
	OPCODE_DATA_IN 	= 0x22,
	OPCODE_IN_E  	= 0x41,
	OPCODE_OUT_S 	= 0x51,
	OPCODE_DATA_OUT = 0x52,
	SW_RESET     	= 0x80,
	I2C_CNFG     	= 0xE0
}MAX77958_REG_ADD;

/*** MAX77958 Opcodes ***/
typedef enum {
	BC_CTRL1_Config_Read_Opcode              = 0x01,
	BC_CTRL1_Config_Write_Opcode             = 0x02,
	Control1_Read_Opcode                     = 0x05,
	Control1_Write_Opcode                    = 0x06,
	CC_Control1_Read_Opcode                  = 0x0B,
	CC_Control1_Write_Opcode                 = 0x0C,
	CC_Control4_Read_Opcode                  = 0x11,
	CC_Control4_Write_Opcode                 = 0x12,
	GPIO_Control_Read_Opcode                 = 0x23,
	GPIO_Control_Write_Opcode                = 0x24,
	GPIO0_GPIO1_ADC_Read_Opcode              = 0x27,
	Get_Sink_Cap_Opcode                      = 0x2F,
	Current_Src_Cap_Opcode                   = 0x30,
	Get_Source_Cap_Opcode                    = 0x31,
	Src_Cap_Request_Opcode                   = 0x32,
	Set_Src_Cap_Opcode                       = 0x33,
	Read_the_Response_for_Get_Request_Opcode = 0x35,
	Send_Get_Response_Opcode                 = 0x36,
	Send_Swap_Request_Opcode                 = 0x37,
	Send_Swap_Response_Opcode                = 0x38,
	APDO_SrcCap_Request_Opcode               = 0x3A,
	Set_PPS_Opcode                           = 0x3C,
	SNK_PDO_Request_Opcode                   = 0x3E,
	SNK_PDO_Set_Opcode                       = 0x3F,
	Get_PD_Message_Opcode                    = 0x4A,
	Customer_Configuration_Read_Opcode       = 0x55,
	Customer_Configuration_Write_Opcode      = 0x56,
	Master_I2C_Control_Read_Opcode           = 0x85,
	Master_I2C_Control_Write_Opcode          = 0x86
}MAX77958_OPCODE;

/***	MAX77958 System Messages	***/ 
typedef enum {
	SYSERROR_NONE						= 0x00,
	SYSERROR_BOOT_WDT					= 0x03,
	SYSERROR_BOOT_SWRSTREQ 				= 0x04, 
	SYSMSG_BOOT_POR						= 0x05,
	SYSERROR_HV_NOVBUS					= 0x10, 
	SYSERROR_HV_FMETHOD_RXPERR			= 0x11,
	SYSERROR_HV_FMETHOD_RXBUFOW			= 0x12,
	SYSERROR_HV_FMETHOD_RXTFR			= 0x13,
	SYSERROR_HV_FMETHOD_MPNACK			= 0x14,
	SYSERROR_HV_FMETHOD_RESET_FAIL		= 0x15,
	SYSMsg_AFC_Done						= 0x20, 
	SYSERROR_SYSPOS						= 0x30,
	SYSERROR_APCMD_UNKNOWN				= 0x31,
	SYSERROR_APCMD_INPROGRESS			= 0x32, 
	SYSERROR_APCMD_FAIL 				= 0x33
}MAX77958_SYSMsg;

/***	MAX77958 PD Messages	***/
typedef enum {
	Nothing_happened							= 0x00,
	Sink_PD_PSRdy_Received						= 0x01,
	Sink_PD_Error_Recovery						= 0x02,
	Sink_PD_SenderResponseTimer_Timeout			= 0x03,
	Source_PSRdy_Sent							= 0x04,
	Source_PD_Error_Recovery					= 0x05,
	Source_PD_SenderResponseTimer_Timeout		= 0x06,
	PD_DR_Swap_Request_Received					= 0x07,
	PD_PR_Swap_Request_Received					= 0x08,
	PD_VCONN_Swap_Request_Received				= 0x09,
	Received_PD_Message_in_illegal_state		= 0x0A,
	Sink_PD_Evaluate_State_SrcCap_Received		= 0x0B,
	VDM_Attention_Message_Received				= 0x11,
	Reject_Received								= 0x12,
	Not_Supported_Received						= 0x13,
	PD_PR_Swap_SNKTOSRC_Cleanup					= 0x14,
	PD_PR_Swap_SRCTOSNK_Cleanup					= 0x15,
	HardReset_Received							= 0x16,
	PD_PowerSupply_VbusEnable					= 0x17,
	PD_PowerSupply_VbusDisable					= 0x18,
	HardReset_Sent								= 0x19,
	PD_PR_Swap_SRCTOSWAP						= 0x1A,
	PD_PR_Swap_SWAPTOSNK						= 0x1B,
	PD_PR_Swap_SNKTOSWAP						= 0x1C,
	PD_PR_Swap_SWAPTOSRC						= 0x1D,
	Sink_PD_Disabled							= 0x20,
	Source_PD_Disabled							= 0x21,
	Get_Source_Capabilities_Extended_Received	= 0x30,
	Get_Status_Received							= 0x31,
	Get_Battery_Cap_Received					= 0x32,
	Get_Battery_Status_Received					= 0x33,
	Get_Manufacturer_Info_Received				= 0x34,
	Source_Capabilities_Extended_Received		= 0x35,
	Status_Received								= 0x36,
	Battery_Capabilities_Received				= 0x37,
	Battery_Status_Received						= 0x38,
	Manufacturer_Info_Received					= 0x39,
	Security_Request_Received					= 0x3A,
	Security_Response_Received					= 0x3B,
	Firmware_Update_Request_Received			= 0x3C,
	Firmware_Update_Response_Received			= 0x3D,
	Alert_Received								= 0x3E,
	VDM_NAK_Received							= 0x40,
	VDM_BUSY_Received							= 0x41,
	VDM_ACK_Received							= 0x42,
	VDM_REQ_Received							= 0x43,
	DiscoverMode_Received						= 0x63,
	PD_Status_Received 							= 0x65
}MAX77958_PDMsg;

/***	Typical USB-C Voltage and Current Rating	***/

typedef enum {
	USBC_5VOUT  = 0x064,
	USBC_9VOUT  = 0x0B4,
	USBC_12VOUT = 0x0F0,
	USBC_15VOUT = 0x12C,
	USBC_20VOUT = 0x190
}USBC_PDO_Voltage_t;

typedef enum {
	USBC_1P5AOUT = 0x096,
	USBC_3AOUT   = 0x12C,
	USBC_5AOUT   = 0x1F4
}USBC_PDO_Current_t;



/***	PDO Types	***/

typedef enum {
	USBC_FIXEDOUT 	= 0x0,
	USBC_BATTERY 	= 0x1,
	USBC_AUGMENTED  = 0x2,
	USBC_PPS 		= 0x3
}USBC_PDO_Type_t;	//Standard source types

/***	PDO Standard Type Data Structures	***/

typedef union {				//Fixed Supply PDO
	uint8_t  PDO_arr[4];
	uint32_t PDO_Value;
	struct {
		uint32_t PDO_SRC_Max_Current 		: 10;	//10mA
		uint32_t PDO_SRC_Voltage 	 		: 10;	//50mV
		uint32_t PDO_SRC_Peak_Current		: 2;
		uint32_t PDO_SRC_RESERVERD			: 3;
		uint32_t PDO_SRC_DualRole_Data		: 1;
		uint32_t PDO_SRC_COMM_CAPABLE		: 1;
		uint32_t PDO_SRC_EXT_PWR			: 1;
		uint32_t PDO_SRC_USB_Suspend_Sup	: 1;
		uint32_t PDO_SRC_DualRole_Pwr		: 1;
		uint32_t PDO_SRC_Supply_Type 		: 2;	//USBC_FIXEDOUT(0x0)
	};
}MAX77958_PDO_FixedSupply_t;

typedef union { 			//Battery PDO
	uint8_t  PDO_arr[4];
	uint32_t PDO_Value;
	struct {
		uint32_t BATT_SRC_Max_Power			: 10;	//250mW
		uint32_t BATT_SRC_Min_Voltage		: 10;	//50mV
		uint32_t BATT_SRC_Max_Voltage		: 10;	//50mV
		uint32_t PDO_SRC_Supply_Type 		: 2;	//USBC_BATTERY(0x1)
	};
} MAX77958_PDO_Battery_t; 

typedef union { 			//Augmented PDO
	uint8_t  PDO_arr[4];
	uint32_t PDO_Value;
	struct {
		uint32_t APDO_SRC_Max_Current		: 10;	//10mA
		uint32_t APDO_SRC_Max_Voltage		: 10;	//50mV
		uint32_t APDO_SRC_Min_Voltage		: 10;	//50mV
		uint32_t PDO_SRC_Supply_Type 		: 2;	//USBC_AUGMENTED (0x2)
	};
} MAX77958_PDO_Augmented_t;	

typedef union { 			//PPS PDO
	uint8_t  PDO_arr[4];
	uint32_t PDO_Value;
	struct {
		uint32_t PPS_SRC_Max_Current		: 7;	//50mA
		uint32_t 							: 1;	//Reserved
		uint32_t PPS_SRC_Min_Voltage		: 8;	//100mV
		uint32_t							: 1; 	//Reserved
		uint32_t PPS_SRC_Max_Voltage		: 8;	//100mV
		uint32_t 							: 2;	//Reserved
		uint32_t PPS_SRC_PowerLimited		: 1;
		uint32_t PPS_SRC_Augmented_Type		: 2;
		uint32_t PPS_SRC_Supply_Type 		: 2;	//USBC_PPS(0x3)
	};
} MAX77958_PDO_PPS_t;


typedef union {				//Configuration info for get_SRC_Cap()
	uint8_t val;
	struct {
		uint8_t  		 : 3;  //RSVD
		uint8_t DataRole : 1;
		uint8_t PwrRole  : 1;
		uint8_t NumOfPDO : 3;
	};
}MAX77958_SRC_CAP_CNFG_t;

/*** USB-C Data Structure	***/

typedef struct{
	uint8_t AP_DATA_IN[AP_DATA_SIZE];	//Data to send to AP Registers
	uint8_t AP_DATA_OUT[AP_DATA_SIZE];	//Most recently recieved data from AP Registers
	uint32_t PDO_DATA[AP_MAX_PDOS];		//Maximum number of PDOs
	uint8_t num_PDOs;					//Number of PDOs available
	uint8_t current_PDO_pos;			//Current PDO position
}MAX77958_USBC_DATA_t;



/***    MAX77958 Register Structures    ***/

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t AttachedHoldI   : 1;
        uint8_t ChgTypI         : 1;
        uint8_t StopModeI       : 1;
        uint8_t DCDTmoI         : 1;
        uint8_t VbADCI          : 1;
        uint8_t VBUSDetI        : 1;
        uint8_t SYSMsgI         : 1;
        uint8_t APCmdResl       : 1;
    };
}MAX77958_UIC_INT_REG_t;
    
typedef union{
	uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t CCStatI   		: 1;
        uint8_t CCVcnStatI      : 1;
        uint8_t CCIStatI       	: 1;
        uint8_t CCPinStatI		: 1;
        uint8_t WtrI      		: 1;
        uint8_t DetAbrtI        : 1;
        uint8_t VSAFE0VI        : 1;
        uint8_t VCONNOCPI       : 1;
    };
}MAX77958_CC_INT_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t 		   		: 2;	//RSVD
        uint8_t DisplayPortI    : 1;
        uint8_t 				: 2;	//RSVD
        uint8_t DataRoleI      	: 1;
        uint8_t PSRDYI        	: 1;
        uint8_t PDMsgI        	: 1;
    };
}MAX77958_PD_INT_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
		uint8_t 	        	: 3;	//RSVD
		uint8_t VbADC        	: 5;	
    };
}MAX77958_USBC_Status1_REG_t;

typedef union{
    uint8_t reg_value;
	uint8_t SYSMsg;      //Hold the register full value
}MAX77958_USBC_Status2_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t ChgTyp		    : 2;
        uint8_t DCDTmo      	: 1;
        uint8_t PrChgTyp       	: 3;
        uint8_t         		: 1;	
		uint8_t	VBUSDet			: 1;
    };
}MAX77958_BC_STATUS_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	struct {
        uint8_t CCSTAT		    : 3;
        uint8_t CCVcnStat      	: 1;
        uint8_t CCIStat       	: 2;
        uint8_t CCPinStat       : 2;	
	};
}MAX77958_CC_STATUS0_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	struct {
        uint8_t 		    	: 1;	//RSVD
        uint8_t Wtr      		: 1;
        uint8_t DetAbrt       	: 1;
        uint8_t VSafeOv       	: 1;
		uint8_t VConnSC       	: 1;
		uint8_t VConnOCP		: 1;		
		uint8_t					: 2;	//RSVD
	};
}MAX77958_CC_STATUS1_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	uint8_t PDMsg;
}MAX77958_PD_STATUS0_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	struct {
        uint8_t 		    	: 4;	//RSVD
        uint8_t PSRDY      		: 1;
        uint8_t VConnS       	: 1;
        uint8_t PowerRole       : 1;
		uint8_t DataRole       	: 1;
	};
}MAX77958_PD_STATUS1_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	struct {
        uint8_t AttachedHoldM	: 1;	
        uint8_t ChgTypM      	: 1;
        uint8_t StopeModeM    	: 1;
        uint8_t DCDTmoM       	: 1;
		uint8_t VbADCM       	: 1;
		uint8_t VBUSDetM       	: 1;
		uint8_t SYSMsgM       	: 1;
		uint8_t APCmdResM       : 1;
	};
}MAX77958_UIC_INT_M_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
	struct {
        uint8_t CCStatM   		: 1;
        uint8_t CCVcnStatM      : 1;
        uint8_t CCIStatM       	: 1;
        uint8_t CCPinStatM		: 1;
        uint8_t WtrM      		: 1;
        uint8_t DetAbrtM        : 1;
        uint8_t VSAFE0VM        : 1;
        uint8_t VCONNOCPM       : 1;
	};
}MAX77958_CC_INT_M_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t 		   		: 2;	//RSVD
        uint8_t DisplayPortM    : 1;
        uint8_t 				: 2;	//RSVD
        uint8_t DataRoleM      	: 1;
        uint8_t PSRDYM        	: 1;
        uint8_t PDMsgM        	: 1;
    };
}MAX77958_PD_INT_M_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t Action0M    	: 1;	
        uint8_t Action1M    	: 1;
        uint8_t Action2M		: 1;
		uint8_t ExtendedActionM	: 1;	
        uint8_t PDMsgM        	: 4;	//RSVD
    };
}MAX77958_ACTION_INT_M_REG_t;

typedef union{
    uint8_t reg_value;      //Hold the register full value
    struct {
        uint8_t HS_EXT_EN    	: 1;	
        uint8_t     			: 3;	//RSVD
        uint8_t PAIR			: 3;
		uint8_t 				: 1;	//RSVD
    };
}MAX77958_I2C_CNFG_REG_t;

#endif  /* MAX77958_def_h */