#include "gpio.h"
#include "rf.h"
#include "drv_rgb.h"
#include "uart.h"
#include "ll.h"
#include "string.h"


typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char l;
} COLOR_RGB;


//hsv data struct
typedef struct {
    float H;
    float S;
    float V;
} COLOR_HSV;

COLOR_RGB rgb_v;



/* 呼吸灯曲线表 */
const uint16_t index_wave[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4,
                               4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12,
                               13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 30, 31, 32, 33,
                               34, 36, 37, 38, 40, 41, 43, 45, 46, 48, 50, 52, 54, 56, 58, 60, 62, 65, 67, 70, 72, 75, 78, 81, 84, 87, 90,
                               94, 97, 101, 105, 109, 113, 117, 122, 126, 131, 136, 141, 146, 152, 158, 164, 170, 176, 183, 190, 197, 205,
                               213, 221, 229, 238, 247, 256, 256, 247, 238, 229, 221, 213, 205, 197, 190, 183, 176, 170, 164, 158, 152, 146,
                               141, 136, 131, 126, 122, 117, 113, 109, 105, 101, 97, 94, 90, 87, 84, 81, 78, 75, 72, 70, 67, 65, 62, 60, 58,
                               56, 54, 52, 50, 48, 46, 45, 43, 41, 40, 38, 37, 36, 34, 33, 32, 31, 30, 28, 27, 26, 25, 25, 24, 23, 22, 21, 20,
                               20, 19, 18, 18, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6,
                               6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                               2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                              };

uint8_t rgb_io_pin[3]= {GPIO_P03,GPIO_P02}; //P03 里面 P02外围


void drv_ws2812_gpio_init(void)
{
    gpio_config(GPIO_P02,OUTPUT,PULL_NONE); //常亮
    gpio_config(GPIO_P03,OUTPUT,PULL_NONE); //中间

    gpio_set(GPIO_P02,0); //外围
    gpio_set(GPIO_P03,0); //中间电源控制引脚

    gpio_config(GPIO_P10,OUTPUT,PULL_NONE); //外围
    gpio_config(GPIO_P11,OUTPUT,PULL_NONE); //中间电源控制引脚

    gpio_set(GPIO_P10,1); //外围
    gpio_set(GPIO_P11,1); //中间电源控制引脚
    // Delay_ms(500);
}

void Delay_tick(uint32_t X) {
    while(X--)__nop();
}
void Delay_tick2(uint32_t X) {
    while(X--);
}

void switch_clk(uint8_t clk);

void drv_ws2812_set_color(uint8_t ch, unsigned long RGB)
{
    unsigned long GRB = 0;
    unsigned char i =0;
    unsigned char k=0;
    unsigned long Dat = 0;


    switch_clk(2); //切换到64M
    //Delay_ms(10);

    uint32_t  io_reg_val_hi = 0;
    uint32_t  io_reg_val_low = 0;
    uint8_t  index[24];

    GRB = (  ((RGB&0x00FF00)<<8) | ((RGB&0xff0000)>>8) | (RGB&0x0000ff) );
    Dat = GRB;
    //UART_PRINTF("RGB = %#.06lX\r\n",RGB);
    //UART_PRINTF("GRB = %#.06lX\r\n",Dat);

    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[ch],1);
    io_reg_val_low = gpio_set_value_get(rgb_io_pin[ch],0);

    memset(index,0,24);


    //UART_PRINTF("val bin = ");
    for(k = 0; k < 24; k++)
    {
        if(Dat & 0x00800000)
        {

            index[k]=1;

        }
        else //0码
        {
            index[k]=0;
        }
        //UART_PRINTF("%d ",index[k]);
        Dat <<= 1;
    }

    //UART_PRINTF("\r\n");

    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
    Delay_ms(3); //复位码
    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    Delay_us(200); //复位码
    switch(ch)
    {
    case 0:
        //GLOBAL_INT_STOP();
        for(k = 0; k < 24; k++)
        {

            if(index[k] == 0)
            {

                *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                __nop();
                __nop();
                __nop();
                __nop();
                __nop();
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                Delay_tick2(1);
            }
            else
            {
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                __nop();
                Delay_tick(1);
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
            }
        }
        //GLOBAL_INT_START();
        break;

    case 1:
        GLOBAL_INT_STOP();
        for(i=0; i<12; i++)
        {
            for(k = 0; k < 24; k++)
            {

                if(index[k] == 0)
                {

                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                    __nop();
                    __nop();
                    __nop();
                    __nop();
                    __nop();
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                    Delay_tick2(1);
                }
                else
                {
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                    __nop();
                    Delay_tick(1);
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                }
            }

        }
        GLOBAL_INT_START();
        break;
    }

    switch_clk(1);
    //Delay_ms(20);
}


uint32_t rgb2grb(uint32_t RGB)
{
    uint32_t GRB = 0;
    GRB = (  ((RGB&0x00FF00)<<8) | ((RGB&0xff0000)>>8) | (RGB&0x0000ff) );
    return GRB;
}


#include "string.h"

uint8_t  index_0_1 [12][24];
uint32_t GBR_TABLE[12];
uint32_t LED_OFF[12]= {0,0,0,0,0,0};


void rgb_table_2_grb_table(uint32_t* grb_table,uint32_t* rgb_table,uint16_t array_len)
{
    uint16_t i=0;
    for(i=0; i<array_len; i++)
    {
        grb_table[i] = rgb2grb(rgb_table[i]);
        //UART_PRINTF("GRB = %#lX ",grb_table[i]);
    }
    //UART_PRINTF("\r\n",GBR_TABLE[i]);
}

//一次性亮不同颜色的灯
void drv_ws2812_test2(uint8_t ch, uint32_t RGB_TABLE[],uint32_t len)
{

    unsigned char i =0;
    unsigned char k=0;

    uint32_t Dat = 0;
    uint32_t io_reg_val_hi = 0;
    uint32_t io_reg_val_low = 0;



    for(i=0; i<6; i++)
    {
        GBR_TABLE[i] = rgb2grb(RGB_TABLE[i]);
        UART_PRINTF("GRB = %#lX ",GBR_TABLE[i]);
    }
    UART_PRINTF("\r\n",GBR_TABLE[i]);

    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[ch],1);
    io_reg_val_low = gpio_set_value_get(rgb_io_pin[ch],0);

    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    Delay_ms(1);

    for(i=0; i<6; i++)
    {
        Dat = GBR_TABLE[i];
        UART_PRINTF("GRB i=%d: ",i);
        for(k = 0; k < 24; k++)
        {
            if(Dat & 0x00800000) // 1
            {
                index_0_1[i][k]=1;
            }
            else //0码
            {
                index_0_1[i][k]=0;
            }
            Dat <<= 1;

            UART_PRINTF("%d ",index_0_1[i][k]);
        }
        UART_PRINTF("\r\n");
    }

    GLOBAL_INT_STOP();

    for(i=0; i<6; i++)
    {
        for(k = 0; k < 24; k++)
        {
            switch(index_0_1[i][k])
            {
            case 1:

                *(&REG_APB5_GPIOB_DATA) = io_reg_val_hi;
                __nop();//190ns
                *(&REG_APB5_GPIOB_DATA) = io_reg_val_low;
                break;
            case 0:
                *(&REG_APB5_GPIOB_DATA) = io_reg_val_hi;
                *(&REG_APB5_GPIOB_DATA) = io_reg_val_low;
                __nop();

                break;
            }
        }
    }


    GLOBAL_INT_START();

}

//流水灯 环绕效果

void drv_ws2812_test(uint32_t RGB)
{

    unsigned char i =0;
    unsigned char k=0;

    uint32_t Dat = 0;

    uint32_t GRB = 0;

    uint32_t io_reg_val_hi = 0;
    uint32_t io_reg_val_low = 0;

    GRB = (  ((RGB&0x00FF00)<<8) | ((RGB&0xff0000)>>8) | (RGB&0x0000ff) );


    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[1],1);
    io_reg_val_low = gpio_set_value_get(rgb_io_pin[1],0);

    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
    Delay_ms(3); //复位码
    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    Delay_us(200); //复位码

    for(char a =0 ; a<12; a++)
    {
        memset(&(index_0_1[a][0]),0,24);
    }

    for(uint8_t  b=0; b<12; b++) //装填数据
    {
        Dat = RGB;
        for(k = 0; k < 24; k++)
        {
            if(Dat & 0x00800000) //1
            {
                index_0_1[b][k]=1;
            }
            else //0码
            {
                index_0_1[b][k]=0;
            }
            Dat <<= 1;
        }
    }

    for(i=0; i<12; i++)
    {

        for(k = 0; k < 24; k++)
        {
            if(index_0_1[i][k] == 0)
            {

                *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                __nop();
                __nop();
                __nop();
                __nop();
                __nop();
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                Delay_tick2(1);
            }
            else
            {
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                __nop();
                Delay_tick(1);
                *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
            }
        }

    }

}


void drv_ws2812_round(uint32_t rgb)
{

    unsigned char i =0;
    unsigned char k=0;

    uint32_t Dat = 0;
    uint32_t io_reg_val_hi = 0;
    uint32_t io_reg_val_low = 0;


    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[1],1);
    io_reg_val_low = gpio_set_value_get(rgb_io_pin[1],0);

    switch_clk(2); //切换到64M

    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
    Delay_ms(3); //复位码
    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    Delay_us(200); //复位码


    for(char a =0 ; a<12; a++)
    {
        memset(&(index_0_1[a][0]),0,24);

        for(k = 0; k < 24; k++)
        {
            if(Dat & 0x00800000) //1
            {
                index_0_1[a][k]=1;
            }
            else //0码
            {
                index_0_1[a][k]=0;
            }
            Dat <<= 1;
        }


        GLOBAL_INT_STOP();

        for(i=0; i<12; i++)
        {

            for(k = 0; k < 24; k++)
            {
                if(index_0_1[i][k] == 0)
                {

                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                    __nop();
                    __nop();
                    __nop();
                    __nop();
                    __nop();
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                    Delay_tick2(1);
                }
                else
                {
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
                    __nop();
                    Delay_tick(1);
                    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
                }
            }

        }
        GLOBAL_INT_START();
        Delay_ms(50);
    }


    switch_clk(1); //切换到64M

}









/**------------------------------------------------------------------------------------------------
  * @brief  : This is return  a and b of minimum value
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static float minValue(float a,float b)  //calc minimum value
{
    float temp = b;
    if(a < temp)
        temp = a;
    return temp;
}
/**------------------------------------------------------------------------------------------------
  * @brief  : This is return  a and b of maximum value
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static float maxValue(float a,float b) //calc maximum value
{
    float temp = b;
    if(a > temp)
        temp = a;
    return temp;
}

/**------------------------------------------------------------------------------------------------
  * @brief  : This is RGB to HSV convert function
  * @param  : None
  * @retval : None
  *----------------------------------------------------------------------------------------------*/
static void RGB_TO_HSV(const COLOR_RGB* input,COLOR_HSV* output)  // convert RGB value to HSV value
{
    float r,g,b,minRGB,maxRGB,deltaRGB;

    r = input->R/255.0f;
    g = input->G/255.0f;
    b = input->B/255.0f;
    minRGB = minValue(r,minValue(g,b));
    maxRGB = maxValue(r,maxValue(g,b));
    deltaRGB = maxRGB - minRGB;

    output->V = maxRGB;
    if(maxRGB != 0.0f)
        output->S = deltaRGB / maxRGB;
    else
        output->S = 0.0f;
    if (output->S <= 0.0f)
    {
        output->H = 0.0f;
    }
    else
    {
        if (r == maxRGB)
        {
            output->H = (g-b)/deltaRGB;
        }
        else
        {
            if (g == maxRGB)
            {
                output->H = 2.0f + (b-r)/deltaRGB;
            }
            else
            {
                if (b == maxRGB)
                {
                    output->H = 4.0f + (r-g)/deltaRGB;
                }
            }
        }
        output->H = output->H * 60.0f;
        if (output->H < 0.0f)
        {
            output->H += 360;
        }
        output->H /= 360;
    }

}

/**------------------------------------------------------------------------------------------------
 * @brief  : This is HSV to RGB convert function
 * @param  : None
 * @retval : None
 *----------------------------------------------------------------------------------------------*/
static void HSV_TO_RGB(COLOR_HSV* input,COLOR_RGB* output)  //convert HSV value to RGB value
{
    float R,G,B;
    int k;
    float aa,bb,cc,f;
    if (input->S <= 0.0f)
        R = G = B = input->V;
    else
    {
        if (input->H == 1.0f)
            input->H = 0.0f;
        input->H *= 6.0f;
        k = (int)floor(input->H);
        f = input->H - k;
        aa = input->V * (1.0f - input->S);
        bb = input->V * (1.0f - input->S * f);
        cc = input->V * (1.0f -(input->S * (1.0f - f)));
        switch(k)
        {
        case 0:
            R = input->V;
            G = cc;
            B =aa;
            break;
        case 1:
            R = bb;
            G = input->V;
            B = aa;
            break;
        case 2:
            R =aa;
            G = input->V;
            B = cc;
            break;
        case 3:
            R = aa;
            G = bb;
            B = input->V;
            break;
        case 4:
            R = cc;
            G = aa;
            B = input->V;
            break;
        case 5:
            R = input->V;
            G = aa;
            B = bb;
            break;
        }
    }
    output->R = (unsigned char)(R * 255);
    output->G = (unsigned char)(G * 255);
    output->B = (unsigned char)(B * 255);
}


/**---------------------------------------------------------------------------
* @brief  : This is brightness adjusting function
* @param  : None
* @retval : None
*-----------------------------------------------------------------------------*/
uint32_t adjustBrightness(uint32_t rgb,int step)             //adjust RGB brightness
{
    uint32_t ret_rgb;
    COLOR_HSV hsv_v;

    RGB_TO_HSV(&rgb_v,&hsv_v);

    ret_rgb =  ((rgb_v.R<<24&0x00FF0000) | (rgb_v.G<<16&0x0000FF00) | (rgb_v.B&0x000000FF));

    rgb_v.l += step;
    if(rgb_v.l <= 0)
    {
        rgb_v.l = 1;
    } else if(rgb_v.l >= 100)
    {
        rgb_v.l = 100;
    }

    hsv_v.V = rgb_v.l /100.0;
    HSV_TO_RGB(&hsv_v,&rgb_v);

    return ret_rgb;
}


void BLN(uint32_t RGB) //开定时器
{

}

int wave_cnt = 0;
uint32_t test_rgb =0x68228B;
uint8_t rgb_up_down =0;
uint32_t RGB_i = 0,color=0xFF0000;
void BLN_timer_handle(void)//10ms 调用一次
{


    while(1)
    {
        if(rgb_up_down == 0)
        {

//			wave_cnt++;
//			test_rgb +=2;
//			drv_ws2812_set_color(0,test_rgb);
//
//		}
//		else if(rgb_up_down ==1)
//		{
//
//			wave_cnt--;
//			test_rgb -=2;
//
//			drv_ws2812_set_color(0,test_rgb);
//		   if(wave_cnt == 0)
//			{
//				rgb_up_down = 0;
//			}
//
//		}
//
//		if(wave_cnt == 100)
//		{
//			rgb_up_down =1;
//		}
//
//		Delay_ms(10);
        }

        //FF0000到ffff00 红变黄
        for(RGB_i = 0; RGB_i<0xff; RGB_i++) {
            color = color + 0x100;
            drv_ws2812_set_color(0,color);
            Delay_ms(10);
        }
        //ffff00到00ff00 黄变绿
        for(RGB_i = 0; RGB_i<0xff; RGB_i++) {
            color = color - 0x10000;
            drv_ws2812_set_color(0,color);
            Delay_ms(10);
        }
        color= 0x00ffff;
//        //00ff00 到00ffff 绿变青
//        for(RGB_i = 0; RGB_i<0xff; RGB_i++) {
//            color++;
//            drv_ws2812_set_color(0,color);
//            Delay_ms(20);
//        }

        //00ffff 到0000ff 青变蓝
        for(RGB_i = 0; RGB_i<127; RGB_i++) {
            color = color - 0x100;
            drv_ws2812_set_color(0,color);
            Delay_ms(20);
        }
        color= 0x0001ff;
        //0000ff 到ff00ff 蓝变紫
        for(RGB_i = 0; RGB_i<0xff; RGB_i++) {
            color = color + 0x10000;
            drv_ws2812_set_color(0,color);
            Delay_ms(20);
        }
        //ff00ff 到0000ff 紫变红
        for(RGB_i = 0; RGB_i<0xff; RGB_i++) {
            color--;
            drv_ws2812_set_color(0,color);
            Delay_ms(20);
        }

    }




}
