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
#include "stdlib.h"
#include "app.h"
#include "Mc32DriverAdc.h"
#include "Mc32DriverLcd.h"
#include "bsp.h" 
#include"system_definitions.h"
#include "peripheral/oc/plib_oc.h"
#include "system_config/default/framework/driver/tmr/drv_tmr_static.h"
#include "system_config/default/framework/driver/oc/drv_oc_static.h"


S_pwmSettings PWMData;      // pour les settings

static uint16_t tablMesure0 [10]; //Tableau pour moyenne glissante canal 0
static uint16_t tablMesure1 [10]; //Tableau pour moyenne glissante canal 1


void GPWM_Initialize(S_pwmSettings *pData)
{
   int i;
   for (i=0;i<= 9; i++){
       tablMesure0 [i] = 0;
       tablMesure1 [i] = 0;
   }
   
   // Init état du pont en H
   BSP_EnableHbrige();
   
   // start timers
   DRV_TMR0_Start(); //Init Timer
   DRV_TMR1_Start();
   DRV_TMR2_Start();
   DRV_TMR3_Start();
   
   //Start OCs
   DRV_OC0_Start();
   DRV_OC1_Start();

    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
     //Variable pour moyenne glissante
    int moyenne0 = 0; //Receptionne la valeur moyenne canal 0
    int moyenne1 = 0; //Receptionne la valeur moyenne canal 1
    uint8_t i;
    
    //compteur pour le sotckage de valeur adc
    static uint8_t count = 0;
    // Lecture du convertisseur AD
    S_ADCResults adcRes;
    
    adcRes = BSP_ReadAllADC();
    
    //Injection de la nouvelle valeur dans le compteur
    //En fonction du nombre de lecture, le mettre dans la case prévu
    tablMesure0[count] = adcRes.Chan0;
    tablMesure1[count] = adcRes.Chan1;
    //Incrémenter le nombre de mesure
    count ++;
    //Test limite du compteur
    if (count > (VALEURMOYENNE - 1)){
        count = 0;
    }
    
    //Valeur moyenne
    for (i = 0; i < VALEURMOYENNE; i ++){
        moyenne0 += tablMesure0[i];
        moyenne1 += tablMesure1[i];
    }
    moyenne0 = moyenne0/VALEURMOYENNE;
    moyenne1 = moyenne1/VALEURMOYENNE;
    
    //Calcul des vitesses absolues
    pData->absSpeed = (moyenne0*LIMITEMAXVITESSEABS)/LIMITEMAXADC;
    pData->absAngle = (moyenne1*LIMITEMAXANGLEABS)/LIMITEMAXADC;
    //Calcul des vitesses
    pData->SpeedSetting = (int8_t)(moyenne0*LIMITEMAXVITESSEABS/LIMITEMAXADC) - LIMITEMAXVITESSE;
    pData->AngleSetting = (int8_t)(moyenne1*LIMITEMAXANGLEABS/LIMITEMAXADC) - LIMITEMAXXANGLE;
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_ClearLine(2);
    lcd_ClearLine(3);
    lcd_ClearLine(4);
    lcd_gotoxy(1,2);
    printf_lcd("SpeedSetting = %3d",pData->SpeedSetting);
    lcd_gotoxy(1,3);
    printf_lcd("absSpeed = %3d",abs(pData->SpeedSetting));
    lcd_gotoxy(1,4);
    printf_lcd("Angle = %3d",pData->AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    uint16_t oc2 = 0;
    uint16_t oc3 = 0;
    //Gestion pont en H
    
    if (pData->SpeedSetting == 0){
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 0;
    }
    else{
        if (pData->SpeedSetting > 0){
            AIN1_HBRIDGE_W = 0;
            AIN2_HBRIDGE_W = 1;
        }
        else{
            AIN1_HBRIDGE_W = 1;
            AIN2_HBRIDGE_W = 0;
        }
    }
    oc2 = abs((PERIODECOUNTERTIMDEUX*pData->SpeedSetting)/ LIMITEMAXVITESSE);
    //oc2 = 4000;
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, oc2);
    
    oc3 = ((pData->absAngle * (VALMAXPCTTIMERDEUX - OFFSETTIMERDEUX))/LIMITEMAXANGLEABS) + OFFSETTIMERDEUX;
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, oc3); 
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    static uint8_t count = 0;
    static uint8_t pwm = 0;
    //_________
    //Si periode (100 cycle) du timer4,
    //Je stock la valeur de la vitesse dans pwm
    if (count > 99){
        count = 0;
        pwm = abs(pData->SpeedSetting);
    }
    
    //en fonction de PWM, je change le duty de la led2
    //PWM software
    if ((count > pwm)||(count == pwm)){
        BSP_LEDOff(BSP_LED_2);
    }
    else{
        BSP_LEDOn(BSP_LED_2);
    }
    count ++;    
}


