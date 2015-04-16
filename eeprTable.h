#ifndef EEPRTABLE_H
#define EEPRTABLE_H


#define R_ADR(a) (a/2)


uint16_t EEMEM SOFT_VER[(0x1000/2)]  = 
{
    [R_ADR(CostLitreCoefEEPROMAdr)]     =   0x0032,				//Cost Litre Coef
	[R_ADR(PulseLitreCoefEEPROMAdr)]    =  	0x2b80,									//Pulse Litre Coef
	[R_ADR(PumpOnTimeCoefEEPROMAdr)]    = 	0x0006,									//Pump On Time Coef
	[R_ADR(PumpOffTimeCoefEEPROMAdr)]   = 	0x000B,									//Pump Off Time Coef
	[R_ADR(BillTimePulseCoefEEPROMAdr)] = 	0x372D,									//Bill Time Pulse Coef
	[R_ADR(CoinTimePulseCoefEEPROMAdr)] = 	0x372D,									//Coin Time Pulse Coef
	[R_ADR(ExtEeprDataAdrEEPROMAdr)]    = 	0x7B00,									//Ext Eepr DataAdr
	[R_ADR(VodomatNumberEEPROMAdr)]     = 	0x0001,									//Vodomat Number
	[R_ADR(PfoneNumber1EEPROMAdr)]      =	0x2B33,									// |
		                                    0x3830,									// |  
		                                    0x3636,									// |
 		                                    0x3735,									//  }  Pfone number for sending SMS	
		                                    0x3730,									// |
	                                     	0x3531,                                 // |   Pfone number for sending SMS 
	                                    	0x3900,                                 // |
                                  //		0x3939,									// |
                                 // 		0x3439,									//  }  Pfone number for sending SMS	
                                  //		0x3139,									// |
                                    //		0x3736,                                 // |   Pfone number for sending SMS 
                                  //		0x3100,                                 // |
	[R_ADR(BoardVersionEEPROMAdr)]      = 	0x0001,									//Board version
	[R_ADR(SMSWaterLevelEEPROMAdr)]     = 	0x2710,                                 //value of water for send sms 100 Litre
	[R_ADR(AmountWaterEEPROMAdr)]       = 	0x0000,                                 //qualiti of water
		                                    0x0000,                                 //
	[R_ADR(DayManeyCntEEPROMAdr)]       = 	0x0000,                                 //qualiti of maney
		                                    0x0000,                                 //
	[R_ADR(MaxSizeBarrelEEPROMAdr)]     = 	0x86A0,                                 //barrel for vater
		                                    0x0001,                                 //
	[R_ADR(ExtEeprCarAdrEEPROMAdr)]     = 	0x0000,                                 //overflow the external memory
	[R_ADR(RepIntervalEEPROMAdr)]       =   0x0002,                                 //15 minut 
	[R_ADR(LowRepLimitEEPROMAdr)]       = 	0x01e0,                                 //08.00
	[R_ADR(UpRepLimitEEPROMAdr)]        = 	0x04b0,                                 //20.00   

	[R_ADR(ApnUserPassAdressEEPROMAdr)] = 	0x6922,                                 //"i
		                                    0x746e,                                 //nt
		                                    0x7265,                                 //er
		                                    0x656e,                                 //ne
		                                    0x2274,                                 //t"
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
                                    		0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //

	[R_ADR(ServerNameEEPROMAdr)]        =	0x6d22,                                 //"m
		                                    0x6d32,                                 //2m
		                                    0x312e,                                 //.1
		                                    0x6d36,                                 //6m
		                                    0x2e62,                                 //b.
		                                    0x6f63,                                 //co
		                                    0x226d,                                 //m"
		                                    0x382c,                                 //,8
		                                    0x0030,                                 //0
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		
	[R_ADR(ScriptNameEEPROMAdr)]        =   0x6373,                                 //sc
		                                    0x6972,                                 //ri
		                                    0x7470,                                 //pt
		                                    0x702e,                                 //.p
		                                    0x7068,                                 //hp
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		
	[R_ADR(PasswordEEPROMAdr)]          =	0x3231,                                 //12
		                                    0x3433,                                 //34
		                                    0x0035,                                 //5
		                                    0x0000,                                 //
		                                    0x0000,                                 //
	[R_ADR(CollectionManeyEEPROMAdr)]   =	0x0000,                                 //
		                                    0x0000,                                 //

	[R_ADR(RegistratorWaterEEPROMAdr)]  =	0x0000,                                 //
		                                    0x0000,                                 //
   											

   [R_ADR(LockParamEEPROMAdr)]          =   0x0100,
   [R_ADR(SoftVersionEEPROMAdr)]        =   0x3332,
   
}; 

#endif







	
