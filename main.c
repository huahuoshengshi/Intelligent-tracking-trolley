#include "sysinc.h"
#include "car_ctrl.h"
#include "hc_sr04.h"

#define KEY_CTRL PAin(8)

#define DISTANCE_FOLLOW 1500
#define DISTANCE_DIF  500

void timer2(){
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  //���TIM3�����жϷ������
		  PCout(13)= !PCout(13);
	  	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
	}
}

u8 flag_run = 0;

void key_int(){
		SYSTICK_DelayMs(5);
		if(KEY_CTRL == 0){
			flag_run = !flag_run;
			while(KEY_CTRL == 0);
		}
}

int main(void){
	int LeftSensorValue = 0;
	int RightSensorValue = 0;
	NVIC_init();
	SYSTICK_DelayInit();	    	 //��ʱ������ʼ��	  	
	
	TIM_ms_Init(TIM2, 500, TIM_IT_Update,2, ENABLE);	//��ʼ����ʱ��2������ʱ��500ms
 	TIMER_CallbackInstall(HW_TIMER2, timer2);	 
	
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);  //��ʼ������1��������9600
	 
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//��ʼ����LED���ӵ�Ӳ���ӿ�
	 	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_6, GPIO_Mode_IPU);//��ʼ����ģ������������
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_4, GPIO_Mode_IPU);//��ʼ����ģ������������
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_8, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_8, key_int);	
	
	car_init();          //С��������ʼ��
	car_set_motor_speed(4000, 4000);
	flag_run=1;
	for(int w=0;w<=100;w++)
	{
		SYSTICK_DelayMs(1000);
	}
	while(1){  

		SYSTICK_DelayMs(20);
		
		if(flag_run){//�Ƿ�ʹ��ң������־
			
				LeftSensorValue  = PAin(4);//���Ҵ������ش�ֵIO��
				RightSensorValue = PAin(6);
				
				if (LeftSensorValue == 1 && RightSensorValue == 1) { 
					car_set_motor_speed(4000, 4000);
					car_forward();        
				}
				else if (LeftSensorValue == 1 && RightSensorValue == 0) {
					car_set_motor_speed(3000, 3000);
					car_back();
					SYSTICK_DelayMs(500);
					car_set_motor_speed(4000, 4000);
					car_turn_left_place();  
					SYSTICK_DelayMs(500);
				}
				else if (RightSensorValue == 1 && LeftSensorValue == 0) {
					car_set_motor_speed(3000, 3000);
					car_back();
					SYSTICK_DelayMs(500);
					car_set_motor_speed(4000, 4000);
					car_turn_right_place();  
					SYSTICK_DelayMs(500);  
				}
				else {
					car_set_motor_speed(3000, 3000);
					car_back();
					SYSTICK_DelayMs(500);
					car_set_motor_speed(4000, 4000);
					car_turn_left_place();  
					SYSTICK_DelayMs(500);
				}
		
		}else{
			car_brake();
		}
	}
}
 

