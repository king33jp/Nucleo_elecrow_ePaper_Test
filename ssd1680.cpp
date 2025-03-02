#include "mbed.h"
#include "ssd1680.h"

uint8_t ImageBW[ALLSCREEN_BYTES];

SSD1680::SSD1680(PinName sck,PinName mosi,PinName miso,PinName res,PinName dc,PinName cs,PinName lcd33,PinName busy) :
_spi(mosi,miso,sck),_res(res),_dc(dc),_cs(cs),_lcd33(lcd33),_busy(busy)
{
    _spi.format(8,0);   // mode=0(first sck is low)
    _spi.frequency(100000);
    _lcd33 = 1;
    _cs=1;
}

void SSD1680::wr_bus(int data){
    _cs = 0;
    _spi.write(data);
    _cs = 1;
}

void SSD1680::wr_reg(int reg){
    _dc = 0;
    wr_bus(reg);
    _dc = 1;
}

void SSD1680::wr_data8(int data){
    _dc = 1;
    wr_bus(data);
    _dc = 0;
}

void SSD1680::readbusy(void){
    int bb=1;
    while( bb==1 ){
        bb = _busy;
    };
}
void SSD1680::hw_reset(void){
    thread_sleep_for(100);
    _res = 1;
    thread_sleep_for(10);
    _res = 0;
    thread_sleep_for(10);
    _res = 1;
    thread_sleep_for(10);
    readbusy();
}
void SSD1680::sleep(void){
    wr_reg(CMD_DEEP_SLEEP_MODE);
    wr_data8(0x01);
    thread_sleep_for(100);
}
void SSD1680::update(void){
    wr_reg(CMD_DISPLAY_UPDATE_CTL2);// 0x22
    wr_data8(0xF4);
    wr_reg(CMD_MASTER_ACTIVATION);// 0x20
    readbusy();
}
void SSD1680::fastupdate(void){
    hw_reset();
    wr_reg(CMD_DISPLAY_UPDATE_CTL2);   // 0x22 Display Update Control 2
    /* 高速リフレッシュモードでは、指定された温度パラメータを書き込む必要があります。
     * クロックをオン,周囲温度の読み取りを開始,LUTをロード,フルリフレッシュモードで動作
     * クロックをオフ
    */
    wr_data8(0xB1); // Load temperature value //Load LUT with DISPLAY Mode 1 //Disable clock signal
    wr_reg(CMD_MASTER_ACTIVATION);   // 0x20 Activate Display Update Sequence
    readbusy();

    wr_reg(CMD_TEMP_SNS_CTL_WR);   // 0x1A 温度パラメータ書き込み 
    wr_data8(0x64);
    wr_data8(0x00);

    wr_reg(CMD_DISPLAY_UPDATE_CTL2);   // 0x22 Display Update Control 2
    wr_data8(0x91); // Load LUT with DISPLAY Mode 1 //Disable clock signal
    wr_reg(CMD_MASTER_ACTIVATION);   // 0x20 Activate Display Update Sequence
    readbusy();

    wr_reg(CMD_DISPLAY_UPDATE_CTL2);   // 0x22 Display Update Control 2
    wr_data8(0xC7); // Enable Analog //Display with DISPLAY Mode 1 //Disable Analog //Disable OSC
    wr_reg(CMD_MASTER_ACTIVATION);   // 0x20 Activate Display Update Sequence
    readbusy();
}
void SSD1680::partupdate(void){
    wr_reg(0x22);
    wr_data8(0xFC);
    wr_reg(0x20);
    readbusy();
}
/* R26hの簡単な紹介はこちら
 * SSD1680 はドライバーの白黒 EPD、白黒と赤の EPD をサポートします。
 * 白黒 EPD を駆動する場合、26H レジスタは保存された前のフレームのデータ内容として理解できます。
 * ローカルリフレッシュモードで理想的なリフレッシュ効果を確実に得るためには、R26Hレジスタの内容を消去する必要があります。
 * 黒、白、赤のEPDを駆動する場合、26Hレジスタには赤の画像データが格納されます。*/
void SSD1680::clear_R26H(void){
    int i;
    wr_reg(CMD_WR_RAM_RED);   // 0x26 Write RAM (RED)
    /* このコマンドの後、別のコマンドが書き込まれるまで、
     * データエントリは RED RAM に書き込まれます。
     * アドレスポインタはそれに応じて進みます。*/
    for(i=0; i<ALLSCREEN_BYTES; i++){
        wr_data8( WHITE );
    }
    readbusy();
}
void SSD1680::all_fill(int color){
    int i;
    wr_reg(CMD_BORDER_WAVE_CTL);   // 0x3C Border Waveform Control
    if( color != 0 ){
        wr_data8(0x01);
    } else {
        wr_data8(0x00);
    }
    wr_reg(CMD_WR_RAM_BLACKWHITE);// 0x24
    for(i=0; i<ALLSCREEN_BYTES; i++){
        wr_data8( color );
    }
    readbusy();
}
void SSD1680::displayimage(const unsigned char *ImageBW){
    int i;
    wr_reg(CMD_BORDER_WAVE_CTL);    // 0x3C
    wr_data8(0x01);
    wr_reg(CMD_WR_RAM_BLACKWHITE);  // 0x24
    for(i=0; i<ALLSCREEN_BYTES; i++){
        wr_data8(~ImageBW[i]);
    }
}
void SSD1680::initialize(void){
    hw_reset();
    wr_reg(CMD_SW_RESET); /* 0x12 Software Reset */
    readbusy();   /* reset確認待ち */

    wr_reg(CMD_DRIVER_OUTPUT_CTL);   /* 0x01 ドライバー出力制御の設定 */
    wr_data8(0x27); /* MUXライン設定の構成 */
    wr_data8(0x01); /* MUXライン設定の構成 */
    wr_data8(0x00); /* EPDスキャンモードを設定する */

    /* データ入力モードを設定し、書き込みRAMを行ごとに書き込むように設定します。 */
    wr_reg(CMD_DATA_ENTRY_MODE_SETTING); // 0x11
    wr_data8(0x03);

    /* RAMの開始XアドレスとYアドレスを設定する */
    wr_reg(CMD_SET_RAM_X_ADDR); // 0x44 
    wr_data8(0x00);
    wr_data8(0x0F);

    wr_reg(CMD_SET_RAM_Y_ADDR); //0x45
    wr_data8(0x00);
    wr_data8(0x00);
    wr_data8(0x27);
    wr_data8(0x01);

    wr_reg(CMD_BORDER_WAVE_CTL);   /* 0x3C 境界線の色を設定する */
    wr_data8(0x01); // 0x01->0x05

    wr_reg(CMD_TEMP_SNS_CTL); /* 0x18 内部温度センサーとして構成 */
    wr_data8(0x80);

    //    EPD_WR_REG(CMD_DISPLAY_UPDATE_CTL2);//0x22
    //    EPD_WR_DATA8(0xF4);
    //    EPD_WR_REG(CMD_MASTER_ACTIVATION);//0x20
    //    EPD_READBUSY();

    wr_reg(CMD_SET_RAM_X_ADDRESS_CNT);//0x4E
    wr_data8(0x00);
    wr_reg(CMD_SET_RAM_Y_ADDRESS_CNT);//0x4F
    wr_data8(0x00);
    wr_data8(0x00);

    //    EPD_WR_REG(0x0C);           /* ブースターの起動時間を設定する  */

    readbusy();
}

void SSD1680::drawPoint(uint16_t x, uint16_t y, uint8_t color){
    uint8_t dat = 0;
    uint16_t xpoint, ypoint;
    uint32_t Addr;
    switch (USE_HORIZONTIAL) {
    case 0:
        xpoint = EPD_H - y - 1;
        ypoint = x;
        break;
    case 1:
        xpoint = x;
        ypoint = y;
        break;
    case 2:
        xpoint = y;
        ypoint = EPD_W - x - 1;
        break;
    case 3:
        xpoint = EPD_W - x - 1;
        ypoint = EPD_H - y - 1;
        break;
    default:
        return;
    }
#if USE_HORIZONTIAL == 0 | USE_HORIZONTIAL == 2
    Addr = xpoint / 8 + ypoint * ((EPD_H % 8 == 0) ? (EPD_H / 8) : (EPD_H / 8 + 1));
#else
    Addr = xpoint / 8 + ypoint * ((EPD_W % 8 == 0) ? (EPD_W / 8) : (EPD_W / 8 + 1));
#endif
    dat = ImageBW[Addr];
    if (color == BLACK){
        ImageBW[Addr] = dat | (0x80 >> (xpoint % 8));
    } else {
        ImageBW[Addr] = dat & ~(0x80 >> (xpoint % 8));
    }
}
void SSD1680::clear(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint8_t color){
    uint16_t x, y;
    //uint32_t Addr;
    for (y = ys; y < ye; y++){
        for (x = xs; x < xe; x++){
            drawPoint(x, y, color);
        }
    }
}

void SSD1680::showPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t pic[], uint8_t color){
    uint8_t t, temp;
    uint16_t x0 = x;
    uint32_t i, TypefaceNum; // 画像のサイズ（バイト単位）
    TypefaceNum = ((width % 8) ? (width / 8 + 1) : (width / 8)) * height;
    for (i = 0; i < TypefaceNum; i++) {
        temp = pic[i];
        for (t = 0; t < 8; t++){
            if (temp & 0x80){
                drawPoint(x, y, color);
            } else {
                drawPoint(x, y, !color);
            }
            x++;
            temp <<= 1;
        }
        if ((x - x0) == width){
            x = x0;
            y++;
        }
    }
}
