
#include "mbed.h"
#include "sensor/ti/hdc1050.h"
#include "common/terminal.h"

I2C i2c0(I2C_SDA,I2C_SCL);

void  HDC1050_Init(void)
{
	I2C *i2c = &i2c0;
	i2c->frequency(100000);
}	

void HDC1050_UnInit(void)
{
	
}

uint16_t  HDC1050_GetVendorID(void)
{
	char rxBuff[2];
	char ofs = TI_HDC1050_MANUFACTURER_ID_ADDR;
	uint16_t   RetVal = 0;
	I2C *i2c = &i2c0;

	i2c->write(TI_HDC1050_DEVICE_ADDR << 1, &ofs, 1);
    i2c->read(TI_HDC1050_DEVICE_ADDR << 1, rxBuff, 2);
	RetVal  = ((uint16_t)rxBuff[0]) << 8 | (uint16_t)	rxBuff[1];
	 	
	return RetVal; 	
}	

void  HDC1050_GetSensorData(float *Temperature, uint16_t *Humidity)
{
	char rxBuff[4];
	char ofs = TI_HDC1050_TEMPERATURE_ADDR;
	uint16_t humidity;
	float temperature;
	I2C *i2c = &i2c0;

	i2c->write(TI_HDC1050_DEVICE_ADDR << 1, &ofs, 1);
	wait_ms(100);
    i2c->read(TI_HDC1050_DEVICE_ADDR << 1, rxBuff, 4);

	temperature = (float)(( rxBuff[0] << 8) | (rxBuff[1] ));
	temperature = (temperature*165)/65536-40;
	humidity = (rxBuff[2] << 8) | (rxBuff[3] );
	humidity = (humidity*100)/65536;
	
	if(Temperature != NULL)
		*Temperature = temperature;
	if(Humidity != NULL)
		*Humidity = humidity;
}

#if defined(BOARD_TEST)
void HDC1050_Test(void)
{
	float Temperature;
	uint16_t Humidity;
	uint16_t VendorId;
	
	HDC1050_Init();

	VendorId = HDC1050_GetVendorID();
	if(VendorId==TI_HDC1050_MANUFACTURER_ID) 
	{	
		Printf("[HDC1050]: Test Pass \r\n");
		Printf("Vendor ID=0x%02x \r\n", VendorId);
		HDC1050_GetSensorData(&Temperature, &Humidity);		
		Printf("Temperature=%f (C) \r\n", Temperature);
		Printf("Humidity=%d RH \r\n", Humidity);
	}	
	else
		Printf("[HDC1050]: Test Fail \r\n");	
	
	HDC1050_UnInit();
	
	Printf("--------------------------------------------------\r\n"); // 50 -
}
#endif


