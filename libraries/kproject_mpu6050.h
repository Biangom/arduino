#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define        SMPLRT_DIV                0x19
#define        CONFIG                        0x1A
#define        GYRO_CONFIG                0x1B
#define        ACCEL_CONFIG        0x1C
#define        ACCEL_XOUT_H        0x3B
#define        ACCEL_XOUT_L        0x3C
#define        ACCEL_YOUT_H        0x3D
#define        ACCEL_YOUT_L        0x3E
#define        ACCEL_ZOUT_H        0x3F
#define        ACCEL_ZOUT_L        0x40
#define        TEMP_OUT_H                0x41
#define        TEMP_OUT_L                0x42
#define        GYRO_XOUT_H                0x43
#define        GYRO_XOUT_L                0x44
#define        GYRO_YOUT_H                0x45
#define        GYRO_YOUT_L                0x46
#define        GYRO_ZOUT_H                0x47
#define        GYRO_ZOUT_L                0x48
#define        PWR_MGMT_1                0x6B
#define        WHO_AM_I                0x75
#define        SlaveAddress        0xD0

unsigned char dis[6];
int	dis_data;

void  InitMPU6050();

void  delay_mpu();
void  I2C_Start();
void  I2C_Stop();
void  I2C_SendACK(unsigned char ack);
void I2C_RecvACK();
void  I2C_SendByte(unsigned char dat);
unsigned char I2C_RecvByte();
void  I2C_ReadPage();
void  I2C_WritePage();

void  display_ACCEL_x();
void  display_ACCEL_y();
void  display_ACCEL_z();
unsigned char Single_ReadI2C(unsigned char REG_Address);
void  Single_WriteI2C(unsigned char REG_Address, unsigned char REG_data);

void delay_mpu()
{
    delayMicroseconds(1);
}

void I2C_Start()
{
    pinMode(MPU_SDA_PIN, OUTPUT);
    pinMode(MPU_SCL_PIN, OUTPUT);
   
    digitalWrite(MPU_SDA_PIN, HIGH);
    digitalWrite(MPU_SCL_PIN, HIGH);
    delay_mpu();
    digitalWrite(MPU_SDA_PIN, LOW);
    delay_mpu();
    digitalWrite(MPU_SCL_PIN, LOW);
}

void I2C_Stop()
{
    pinMode(MPU_SDA_PIN, OUTPUT);
    pinMode(MPU_SCL_PIN, OUTPUT);

    digitalWrite(MPU_SDA_PIN, LOW);
    digitalWrite(MPU_SCL_PIN, HIGH);
    delay_mpu();
    digitalWrite(MPU_SDA_PIN, HIGH);
    delay_mpu();
}

void I2C_SendACK(unsigned char ack)
{
    pinMode(MPU_SDA_PIN, OUTPUT);
    pinMode(MPU_SCL_PIN, OUTPUT);

    digitalWrite(MPU_SDA_PIN, ack);
    digitalWrite(MPU_SCL_PIN, HIGH);
    delay_mpu();
    digitalWrite(MPU_SCL_PIN, LOW);
    delay_mpu();
}

void I2C_RecvACK()
{
    // ack clock
    pinMode(MPU_SDA_PIN, OUTPUT);
    pinMode(MPU_SCL_PIN, OUTPUT);
    digitalWrite(MPU_SDA_PIN, LOW);
    delayMicroseconds(1);
    digitalWrite(MPU_SCL_PIN, HIGH);
    delayMicroseconds(2);
    digitalWrite(MPU_SCL_PIN, LOW);
    delayMicroseconds(1);
    return ;
    /*


        digitalWrite(MPU_SCL_PIN,HIGH);
        delay_mpu();
        CY = MPU_SDA_PIN;
        digitalWrite(MPU_SCL_PIN,LOW);
        delay_mpu();
        return CY;
    */
}

void I2C_SendByte(unsigned char dat)
{
    pinMode(MPU_SDA_PIN, OUTPUT);
    pinMode(MPU_SCL_PIN, OUTPUT);
    // send address 8 bit
    for ( int i = 0; i < 8; i++)
    {
        unsigned char temp = dat & ( 0b10000000 >> i );
        unsigned char bit_value = ( temp != 0 );
        digitalWrite(MPU_SDA_PIN, bit_value);
        delayMicroseconds(1);
        digitalWrite(MPU_SCL_PIN, HIGH);
        delayMicroseconds(1);
        digitalWrite(MPU_SCL_PIN, LOW);
        delayMicroseconds(1);
    }
    I2C_RecvACK();
}

unsigned char I2C_RecvByte()
{
    unsigned char val = 0b00000000;
    pinMode(MPU_SDA_PIN, INPUT_PULLUP);
    pinMode(MPU_SCL_PIN, OUTPUT);
    digitalWrite(MPU_SCL_PIN, LOW);
    // send address 8 bit
    for ( int i = 0; i < 8; i++)
    {
        delayMicroseconds(1);
        digitalWrite(MPU_SCL_PIN, HIGH);
        delayMicroseconds(1);
        val = val | ( digitalRead(MPU_SDA_PIN) << ( 7 - i) );
        digitalWrite(MPU_SCL_PIN, LOW);
        delayMicroseconds(1);
    }

    return val;
    /*
        unsigned char i;
        unsigned char dat = 0;
        digitalWrite(MPU_SDA_PIN,HIGH);
        for (i = 0; i < 8; i++)
        {
            dat <<= 1;
            digitalWrite(MPU_SCL_PIN,HIGH);
            delay_mpu();
            dat |= MPU_SDA_PIN;
            digitalWrite(MPU_SCL_PIN,LOW);
            delay_mpu();
        }
        return dat;
    */
}

void Single_WriteI2C(unsigned char REG_Address, unsigned char REG_data)
{
    I2C_Start();
    I2C_SendByte(SlaveAddress);
    I2C_SendByte(REG_Address);
    I2C_SendByte(REG_data);
    I2C_Stop();
}

unsigned char Single_ReadI2C(unsigned char REG_Address)
{
    unsigned char REG_data;
    I2C_Start();
    I2C_SendByte(SlaveAddress);
    I2C_SendByte(REG_Address);
    I2C_Start();
    I2C_SendByte(SlaveAddress + 1);
    REG_data = I2C_RecvByte();
    I2C_SendACK(1);
    I2C_Stop();
    return REG_data;
}

void InitMPU6050()
{
    Single_WriteI2C(PWR_MGMT_1, 0b10000000);
    delay(3);
    Single_WriteI2C(PWR_MGMT_1, 0x00);
    Single_WriteI2C(SMPLRT_DIV, 0x07);
    Single_WriteI2C(CONFIG, 0x06);
    Single_WriteI2C(GYRO_CONFIG, 0x18);
    Single_WriteI2C(ACCEL_CONFIG, 0x01);
}

int GetData(unsigned char REG_Address)
{
    unsigned char H, L;
    H = Single_ReadI2C(REG_Address);
    L = Single_ReadI2C(REG_Address + 1);
    return ((H << 8) + L);
}

unsigned char GetDeviceID()
{
    return Single_ReadI2C(WHO_AM_I);
}

