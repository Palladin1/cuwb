#ifndef EEPRTABLE_H
#define EEPRTABLE_H


#define R_ADR(a) (a/2)


uint16_t EEMEM SOFT_VER[(0x1000/2)]  = 
{
    [R_ADR(CostLitreCoefEEPROMAdr)]     =   0x0032,				//Cost Litre Coef
	[R_ADR(PulseLitreCoefEEPROMAdr)]    =  	0x2b80,									//Pulse Litre Coef
	[R_ADR(PumpOnTimeCoefEEPROMAdr)]    = 	0x0006,									//Pump On Time Coef
	[R_ADR(PumpOffTimeCoefEEPROMAdr)]   = 	0x000B,									//Pump Off Time Coef
	[R_ADR(BillTimePulseCoefEEPROMAdr)] = 	0x190F,									//Bill Time Pulse Coef
	[R_ADR(CoinTimePulseCoefEEPROMAdr)] = 	0x190F,									//Coin Time Pulse Coef
	[R_ADR(ExtEeprDataAdrEEPROMAdr)]    = 	0xfff0,									//Ext Eepr DataAdr
	[R_ADR(VodomatNumberEEPROMAdr)]     = 	0x0001,									//Vodomat Number

	[R_ADR(BoardVersionEEPROMAdr)]      = 	0x0101,									//Board version
	[R_ADR(SMSWaterLevelEEPROMAdr)]     = 	0x2710,                                 //value of water for send sms 100 Litre
	[R_ADR(AmountWaterEEPROMAdr)]       = 	0x0000,                                 //current quantity of water 
		                                    0x0000,                                 //

	[R_ADR(MaxSizeBarrelEEPROMAdr)]     = 	0x86A0,                                 //how meny a water can put into the barrel
		                                    0x0001,                                 //
	[R_ADR(ExtEeprCarAdrEEPROMAdr)]     = 	0x0000,                                 //overflow the external memory
	[R_ADR(RepIntervalEEPROMAdr)]       =   0x0002,                                 //2 minuts 
  
    [R_ADR(MoneyCounterEEPROMAdr)]     =	0x0000,                                 //
		                                    0x0000,                                 //
											0x0000,                                 //
		                                    0x0000,                                 //


	[R_ADR(ApnUserPassAdressEEPROMAdr)] = 	0x7722,                                 // "w
		                                    0x7777,                                 // ww
		                                    0x752e,                                 // .u
		                                    0x636d,                                 // mc
		                                    0x752e,                                 // .u
		                                    0x2261,                                 // a"
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

	[R_ADR(ServerNameEEPROMAdr)]        =	0x7622,                                 //"v
		                                    0x646f,                                 //od
		                                    0x2e61,                                 //a.
		                                    0x3776,                                 //v7
		                                    0x752e,                                 //.u
		                                    0x2261,                                 //a"
		                                    0x382c,                                 //,8
		                                    0x0030,                                 //0
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		
	[R_ADR(ScriptNameEEPROMAdr)]        =   0x6675,                                 //uf
		                                    0x636b,                                 //kc
		                                    0x3237,                                 //72
											0x2e31,                                 //1.
		                                    0x6870,                                 //ph
		                                    0x0070,                                 //p
		                                    0x0000,                                 //
		                                    0x0000,                                 //
		
	[R_ADR(PasswordEEPROMAdr)]          =	0x6655,                                 //Uf
		                                    0x4b37,                                 //7K
		                                    0x3243,                                 //C2
		                                    0x6131,                                 //1a
		                                    0x0056,                                 //V


	[R_ADR(RegistratorWaterEEPROMAdr)]  =	0x0000,                                 //
		                                    0x0000,                                 //
	[R_ADR(RegistratorCashEEPROMAdr)]  =	0x0000,                                 //
		                                    0x0000,                                 //
   											

   [R_ADR(LockParamEEPROMAdr)]          =   0x0100,
   [R_ADR(SoftVersionEEPROMAdr)]        =   0x3336,
   
}; 

#endif







	
