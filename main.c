#include "sysinc.h"
#include "car_ctrl.h"
#include "hc_sr04.h"

#define KEY_CTRL PAin(8)

#define DISTANCE_FOLLOW 1500
#define DISTANCE_DIF  500

void timer2(){
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  //检查TIM3更新中断发生与否
		  PCout(13)= !PCout(13);
	  	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
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
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	
	
	TIM_ms_Init(TIM2, 500, TIM_IT_Update,2, ENABLE);	//初始化定时器2，定是时间500ms
 	TIMER_CallbackInstall(HW_TIMER2, timer2);	 
	
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);  //初始化串口1，波特率9600
	 
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	 	
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_6, GPIO_Mode_IPU);//初始化与模块相连的引脚
	GPIO_QuickInit(HW_GPIOA, GPIO_Pin_4, GPIO_Mode_IPU);//初始化与模块相连的引脚
	
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_8, 3, 3);
	EXTI_CallbackInstall(EXTI_Pin_8, key_int);	
	
	car_init();          //小车驱动初始化
	car_set_motor_speed(4000, 4000);
	flag_run=1;
	for(int w=0;w<=100;w++)
	{
		SYSTICK_DelayMs(1000);
	}
	while(1){  

		SYSTICK_DelayMs(20);
		
		if(flag_run){//是否使用遥控器标志
			
				LeftSensorValue  = PAin(4);//左右传感器回传值IO口
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
 

