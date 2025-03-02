#ifndef MBED_SSD1680_H
#define MBED_SSD1680_H

#include "mbed.h"

#define USE_HORIZONTIAL 2 /* 0 1 2 3 */
#if USE_HORIZONTIAL == 0 | USE_HORIZONTIAL == 2
#define EPD_W 296
#define EPD_H 128
#define ALLSCREEN_BYTES ((EPD_H % 8) ? (EPD_H / 8 + 1) : (EPD_H / 8)) * EPD_W
#else
#define EPD_W 128
#define EPD_H 296
#define ALLSCREEN_BYTES ((EPD_W % 8) ? (EPD_W / 8 + 1) : (EPD_W / 8)) * EPD_H
#endif
/* Color parameter setting */
#define WHITE 0xFF
#define BLACK 0x00
/* SSD1680 Command Register set */
#define	CMD_DRIVER_OUTPUT_CTL	        0x01
#define	CMD_GATE_DRV_VOLT_CTL	        0x03
#define	CMD_SOURCE_DRV_VOLT_CTL	        0x04
#define	CMD_INIT_CODE_SETTING_OTP_PRGM	0x08
#define	CMD_WR_REG_FOR_INIT_CODE_SETTING	0x09
#define	CMD_RD_REG_FOR_INIT_CODE_SETTING	0x0A
#define	CMD_BOOSTER_SOFT_START_CTL	    0x0C
#define	CMD_DEEP_SLEEP_MODE	            0x10
#define	CMD_DATA_ENTRY_MODE_SETTING	    0x11
#define	CMD_SW_RESET	                0x12
#define	CMD_HV_READY_DETECT     	    0x14
#define	CMD_VCI_DETECT      	        0x15
#define	CMD_TEMP_SNS_CTL	            0x18
#define	CMD_TEMP_SNS_CTL_WR	            0x1A
#define	CMD_TEMP_SNS_CTL_RD	            0x1B
#define	CMD_TEMP_SNS_CTL_WR_EX	        0x1C
#define	CMD_MASTER_ACTIVATION	        0x20
#define	CMD_DISPLAY_UPDATE_CTL1	        0x21
#define	CMD_DISPLAY_UPDATE_CTL2	        0x22
#define	CMD_WR_RAM_BLACKWHITE	        0x24
#define	CMD_WR_RAM_RED      	        0x26
#define	CMD_READ_RAM	                0x27
#define	CMD_VCOM_SNS	                0x28
#define	CMD_VCOM_SNS_DURATION	        0x29
#define	CMD_PRGM_VCOM_OTP	            0x2A
#define	CMD_WR_REG_FOR_VCOM_CTL	        0x2B
#define	CMD_WR_VCOM_REG	                0x2C
#define	CMD_OTP_REG_RD_FOR_DISPLAY_OPTION	0x2D
#define	CMD_USER_ID_RD	                0x2E
#define	CMD_STATUS_BIT_RD	            0x2F
#define	CMD_PRGM_WS_OTP 	            0x30
#define	CMD_LOAD_WS_OTP 	            0x31
#define	CMD_WR_LUT_REG  	            0x32
#define	CMD_CRC_CALC    	            0x34
#define	CMD_CRC_STAT_RD	                0x35
#define	CMD_PRGM_OTP_SELECT	            0x36
#define	CMD_WR_REG_FOR_DISPLAY_OPTION	0x37
#define	CMD_WR_REG_FOR_USER_ID	        0x38
#define	CMD_OTP_PRGM_MODE	            0x39
#define	CMD_BORDER_WAVE_CTL	            0x3C
#define	CMD_END_OPTION	                0x3F
#define	CMD_RD_RAM_OPTION   	        0x41
#define	CMD_SET_RAM_X_ADDR  	        0x44
#define	CMD_SET_RAM_Y_ADDR  	        0x45
#define	CMD_AUTO_WR_RED_RAM_FOR_REG_PAT	0x46
#define	CMD_AUTO_WR_BW_RAM_FOR_REG_PAT	0x47
#define	CMD_SET_RAM_X_ADDRESS_CNT	    0x4E
#define	CMD_SET_RAM_Y_ADDRESS_CNT	    0x4F
#define	CMD_NOP	                        0x7F

class SSD1680 {
    public:
    SSD1680(PinName sck,PinName mosi,PinName miso,PinName res,PinName dc,PinName cs,PinName lcd33,PinName busy);

    void wr_bus(int data);
    void wr_reg(int reg);
    void wr_data8(int data);
    void sleep(void);
    void update(void);
    void partupdate(void);
    void fastupdate(void);
    void clear_R26H(void);
    void all_fill(int color);
    void displayimage(const unsigned char *ImageBW);
    void initialize(void);
    void readbusy(void);
    void hw_reset(void);

    void drawPoint(uint16_t x,uint16_t y,uint8_t color);
    void clear(uint16_t xs,uint16_t ys,uint16_t xe,uint16_t ye,uint8_t color);
    void showPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t pic[], uint8_t color);

    private:
    SPI _spi;
    DigitalOut _res;
    DigitalOut _dc;
    DigitalOut _cs;
    DigitalOut _lcd33;
    DigitalIn _busy;
};
#endif

