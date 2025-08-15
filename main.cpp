#include "ThisThread.h"
#include "mbed.h"
#include "DebouncedInterrupt.h"

// === 状态控制 ===
volatile bool run = true; // 是否运行电机
Ticker ticker;
// === 速度设置 ===
const float max_speed = 1.0;   // 可调成 0.6 ~ 1.0
const float turn_speed = 0.6;  // 可调成 0.3 ~ 0.7

// === 传感器（TCRT5000） ===
DigitalIn sensor_left(PA_8);   // 左传感器 D0 接 PA0
DigitalIn sensor_right(PA_0);  // 右传感器 D0 接 PA1

// === 电机方向控制（L293D） ===
DigitalOut in1(PF_5);  // 左电机 in1
DigitalOut in2(PF_3);  // 左电机 in2
DigitalOut in3(PF_1);  // 右电机 in3
DigitalOut in4(PC_15); // 右电机 in4

// === 电机速度控制（PWM） ===
PwmOut L_motor_pwm(PF_6); // 左电机 ENA
PwmOut R_motor_pwm(PA_3); // 右电机 ENB

// === 定时器中断 & 按钮控制 ===
InterruptIn user_button(BUTTON1);

// === 巡线逻辑 ===
void SetMotorSpeedISR() {
    //printf("SetMotorSpeedISR run=%d\n", run);
    //if (run == true){
    int left = sensor_left.read();
    int right = sensor_right.read();
        
    if (left == 0 && right == 0) {
        L_motor_pwm.write(max_speed);
        R_motor_pwm.write(max_speed);
    }
    else if (left == 1 && right == 0) {
        L_motor_pwm.write(turn_speed);
        R_motor_pwm.write(max_speed);
    }
    else if (left == 0 && right == 1) {
        L_motor_pwm.write(max_speed);
        R_motor_pwm.write(turn_speed);
    }
    else {
        L_motor_pwm.write(0);
        R_motor_pwm.write(0);
    }
    //}
    //else{
        //L_motor_pwm.write(0.0);
        //R_motor_pwm.write(0.0);
    //}
    
}

// === 按钮控制电机启停 ===
void MotorsOnOffISR() {
    run = !run;
    printf("Button pressed! Now run = %d\n", run);
}

int main() {
    in1 = 1;
    in2 = 0;
    in3 = 0;
    in4 = 1;
    

    // 设置 PWM 频率（1kHz）
    L_motor_pwm.period(0.001f);
    R_motor_pwm.period(0.001f);

    // 按钮中断绑定
    //user_button.fall(&MotorsOnOffISR);
    ticker.attach(&SetMotorSpeedISR,20ms);
    

    while(true){
        ThisThread::sleep_for(20ms);
    }
    
}
