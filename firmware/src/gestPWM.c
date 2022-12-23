/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2016-2017
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "GestPWM.h"
#include "Mc32DriverAdc.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
    
   // lance les timers et OC
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
    uint16_t ADC0_RawVal[];
    uint16_t ADC1_RawVal[];
    uint16_t static Counter = 0;
    
    // Lecture du convertisseur AD
    ADC0_RawVal[Counter] = S_ADCResults.Chan0;          //Lecture du canal 0 de l'ADC
    ADC1_RawVal[Counter] = S_ADCResults.Chan1;          //Lecture du canal 1 de l'ADC
    Counter++;
    
    // conversion
    
    if(Counter > 10 - 1)
    {
        Counter = 0;
    }
        
    /************************************************************/
    /*   1er ADC (Canal 0)                                      */
    /************************************************************/
    uint16_t Val0_New;
         
    Val0_New = (1023/198)*ADC0_RawVal[];          //Plage 0 à 1023 to 198
    
    /************************************************************/
    /*   2em ADC (Canal 1)                                      */
    /************************************************************/
    uint16_t Val1_New;
    
    Val1_New = (1023/180)*ADC0_RawVal[];          //Plage 0 à 1023 to 180
    
    pData->absSpeed = Val0_New;
    pData->absAngle = Val1_New;
    pData->SpeedSetting
    pData->AngleSetting
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


