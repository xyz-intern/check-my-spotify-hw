/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "TM1637.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*
 * lcd code define
 */
#define I2C_ADDR 0x27 // I2C address of the PCF8574
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit

#define LCD_SCROLL_TIME 1500
#define COUNT_MS 1000
#define LCD_MAX_LENGTH 16
#define MAX_LENGTH  100

#define DOUBLE_CLICK__TIME 700
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
typedef struct LCDAT {
	char** tokens;
	size_t count;
} LCDAT;

// serial communication code
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

// lcd code
uint8_t backlight_state = 1;
uint8_t count_i = 0;

volatile uint8_t displayColon =0;

// ramps swtich btn
volatile uint8_t sw_state_stop_play=1; // 1: start, 0: stop

volatile uint8_t lcd_title_scroll=0;
volatile uint8_t lcd_artists_scroll=0;

char title_tmp[MAX_LENGTH] = {};
char artists_tmp[MAX_LENGTH] = {};

uint32_t duration_time_tmp =0;
uint32_t full_time_tmp=0;
uint32_t display_mode = 1;

uint8_t data[6][1] = {
		{0x00}, {0x01}, // 000 001
		{0x02}, {0x03}, // 010 011
		{0x04}, {0x05}  // 011 100
};

GPIO_PinState btn_flag_1 = 0;
GPIO_PinState btn_flag_2 = 0;
GPIO_PinState btn_flag_3 = 0;
GPIO_PinState btn_flag_4 = 0;

uint32_t np_old_tick     = 0;
uint32_t np_current_tick = 0;
uint32_t np_delay_time = 0 ;
GPIO_PinState np_is_double_click = 0;

uint32_t vol_old_tick     = 0;
uint32_t vol_current_tick = 0;
uint32_t vol_delay_time = 0 ;
GPIO_PinState vol_is_double_click = 0;

GPIO_PinState is_playing = 0;
char* display_type_1 = "song";
char* display_type_2 = "duration";

char test[10] = {};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/*
 * lcd functions
 */
void lcd_write_nibble(uint8_t nibble, uint8_t rs);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_init();
void lcd_write_string(char *str);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_clear(void);
void lcd_backlight(uint8_t state);

/*
void count_seven_segment(void);
*/

LCDAT split(char* input, const char* delimiter);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for ( int i = 0 ; i < 1000000; i++ ) {} // debouncing delay

	if (GPIO_Pin == START_STOP_BTN_Pin)
	{
		btn_flag_1 = 1;
	}
	else if ( GPIO_Pin == NEXT_PREVIOUS_BTN_Pin )
	{

		np_current_tick = HAL_GetTick();
		// if double press ? NP_BTN_TIME 전에 또 눌린게 있다면
		if ( np_current_tick - np_old_tick < DOUBLE_CLICK__TIME && btn_flag_2 ==1 )
		{
			// 두번 눌렸다고 확인
			np_is_double_click=1;
			btn_flag_2=0;
		}
		else
		{
			np_is_double_click =0;
			btn_flag_2=1;
		}
		np_old_tick = HAL_GetTick();
	}
	else if ( GPIO_Pin == VOLUMN_UP_DOWN_Pin )
	{
		vol_current_tick = HAL_GetTick();
		// if double press ? NP_BTN_TIME 전에 또 눌린게 있다면
		if ( vol_current_tick - vol_old_tick < DOUBLE_CLICK__TIME && btn_flag_3 ==1 )
		{
			// 두번 눌렸다고 확인
			vol_is_double_click=1;
			btn_flag_3=0;
		}
		else
		{
			vol_is_double_click =0;
			btn_flag_3=1;
		}
		vol_old_tick = HAL_GetTick();
	}
	else if ( GPIO_Pin == TRANSFORM_DISPLAY_Pin )
	{
		btn_flag_4 = !btn_flag_4;

	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
//  volatile uint8_t displayColon =0;

  // lcd code
  lcd_init();
  lcd_backlight(1);
  lcd_set_cursor(0, 1);
  lcd_write_string("Start Spotify!");
  // seven segment code
  TM1637_SetBrightness(7);

//  char** tokens = NULL;
  uint16_t serial_len = 0 ;

  uint32_t start_tick_title= HAL_GetTick();
  uint32_t start_tick_artists= HAL_GetTick();

  uint32_t duration_tick = 0;
  LCDAT tokens;
  tokens.count = 0;
  tokens.tokens = NULL;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Are you serial data ?
	  serial_len = strlen((const char*)UserRxBufferFS);

//	   if serial data is here
	  if ( serial_len > 0 && serial_len < 300 ) // buffer overflow
	  {
		  // RxBuffer -> TxBuffer
		  strncpy((char *)UserTxBufferFS, (const char*)UserRxBufferFS,serial_len);
		  UserTxBufferFS[serial_len] = '\0';

		  tokens.count = 0;
		  tokens.tokens = NULL;
		  free(tokens.tokens);

		  // split `,` data
		  tokens = split((char *)UserTxBufferFS,"|");

		  lcd_clear();

		  char* serial_type = *(tokens.tokens+0);

		  if ( strcmp(serial_type,display_type_1) == 0 )
		  {
			  char* title = *(tokens.tokens+1);
			  char* artists = *(tokens.tokens+2);

			  size_t title_size = strlen(title);
			  size_t artists_size = strlen(artists);

			  memset((uint8_t *)title_tmp,0,strlen(title_tmp));
			  memset((uint8_t *)artists_tmp,0,strlen(artists_tmp));

			  strncpy((char *)title_tmp,(const char *)title,title_size);
			  strncpy((char *)artists_tmp,(const char *)artists,artists_size);

			  strcat(title_tmp,"   ");
			  strcat(artists_tmp,"   ");

			  lcd_set_cursor(0, 0);
			  lcd_write_string(title);
			  if ( title_size <= LCD_MAX_LENGTH ) { lcd_title_scroll = 0; }
			  else { lcd_title_scroll = 1; }

			  lcd_set_cursor(1, 0);
			  lcd_write_string(artists);

			  if ( artists_size <= LCD_MAX_LENGTH ) { lcd_artists_scroll  = 0; }
			  else { lcd_artists_scroll  = 1; }


			  start_tick_title = HAL_GetTick();
			  start_tick_artists = HAL_GetTick();
		  }
		  else if ( strcmp(serial_type,display_type_2) == 0)
		  {
			  char* duration_time = *(tokens.tokens+1);
			  char* full_time = *(tokens.tokens+2);

			  size_t du_size = strlen(duration_time);
			  size_t fu_size = strlen(full_time);

			  duration_time_tmp = atoi(duration_time) / 0.001;
			  full_time_tmp = atoi(full_time) / 0.001;

			  is_playing = 1;
			  duration_tick = HAL_GetTick();
		  }
		  memset(UserRxBufferFS, 0, sizeof(UserRxBufferFS));
		  memset(UserTxBufferFS, 0, sizeof(UserTxBufferFS));
		  serial_len = 0;
	  }

	  if ( HAL_GetTick() - duration_tick >= COUNT_MS)
	  {
//		  is_playing == 1 &&
		  if ( duration_time_tmp >= full_time_tmp ) {
			  duration_time_tmp = 0;
		  }

		  duration_time_tmp += 1;
		  duration_tick = HAL_GetTick();

		  if (display_mode == 2)
		  {
			  lcd_set_cursor(1, 0);
			  int dt1 = duration_time_tmp / 60;
			  itoa(dt1,test,10);
			  lcd_write_string(test);

			  lcd_set_cursor(1, 1);
			  lcd_write_string(":");

			  lcd_set_cursor(1,2);
			  int dt2 = duration_time_tmp % 60;
			  itoa(dt2,test,10);
			  lcd_write_string(test);
		  }
	  }

	  if ( btn_flag_4 == 0 ) // display mode 1
	  {
		  display_mode = 1;
		  if ( lcd_title_scroll == 1 && HAL_GetTick() - start_tick_title >= LCD_SCROLL_TIME)
		  {
			  int title_tmp_size = strlen(title_tmp);
			  char ch_ti = 0;
			  ch_ti = title_tmp[0];
			  strcpy(title_tmp,title_tmp+1);
			  title_tmp[title_tmp_size-1] = ch_ti;
			  lcd_set_cursor(0, 0);
			  lcd_write_string(title_tmp);
			  start_tick_title = HAL_GetTick();
		  }

		  if ( lcd_artists_scroll == 1 && HAL_GetTick() - start_tick_artists >= LCD_SCROLL_TIME)
		  {
			  int artists_tmp_size = strlen(artists_tmp);
			  char ch_ar = 0;
			  ch_ar = artists_tmp[0];
			  strcpy(artists_tmp,artists_tmp+1);
			  artists_tmp[artists_tmp_size-1] = ch_ar;
			  lcd_set_cursor(1, 0);
			  lcd_write_string(artists_tmp);
			  start_tick_artists = HAL_GetTick();
		  }
	  }
	  else // display 2 mode
	  {
		  display_mode = 2;
	  }

	  if ( btn_flag_1 == 1 )
	  {
		  // btn flag 0 으로 만들기
		  lcd_clear(); lcd_set_cursor(0, 6); btn_flag_1 = 0;

		  // 로직 실행
		  if ( sw_state_stop_play )
		  {
			  lcd_write_string("play"); CDC_Transmit_FS(data[0],1);
		  }
		  else
		  {
			  lcd_write_string("stop");  CDC_Transmit_FS(data[1],1);
			  lcd_artists_scroll=0; lcd_title_scroll=0; is_playing = 0;
		  }

		  sw_state_stop_play = !sw_state_stop_play; // 버튼 상태를 변경합니다.
	  }

	  if ( np_is_double_click == 0 && btn_flag_2 == 1)
	  {
		  btn_flag_2 = 0; lcd_clear(); lcd_set_cursor(0,6);
		  lcd_write_string("next"); CDC_Transmit_FS(data[2], 1); // 010 next
		  is_playing=0;
	  }
	  if ( np_is_double_click == 1 && btn_flag_2 == 0 )
	  {
		  np_is_double_click=0;lcd_clear(); lcd_set_cursor(0,6);
		  lcd_write_string("prev"); CDC_Transmit_FS(data[3], 1); // 011 prev
		  is_playing=0;
	  }

	  if ( vol_is_double_click == 0 && btn_flag_3 == 1)
	  {
		  btn_flag_3 = 0; lcd_clear(); lcd_set_cursor(0,4);
		  lcd_write_string("volumn up"); CDC_Transmit_FS(data[4], 1); // 100 next
	  }
	  if ( vol_is_double_click == 1 && btn_flag_3 == 0 )
	  {
		  vol_is_double_click=0;lcd_clear(); lcd_set_cursor(0,2);
		  lcd_write_string("volumn down"); CDC_Transmit_FS(data[5], 1); // 101 prev
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CLK1_Pin|DIO1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : START_STOP_BTN_Pin NEXT_PREVIOUS_BTN_Pin VOLUMN_UP_DOWN_Pin TRANSFORM_DISPLAY_Pin */
  GPIO_InitStruct.Pin = START_STOP_BTN_Pin|NEXT_PREVIOUS_BTN_Pin|VOLUMN_UP_DOWN_Pin|TRANSFORM_DISPLAY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK1_Pin */
  GPIO_InitStruct.Pin = CLK1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CLK1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DIO1_Pin */
  GPIO_InitStruct.Pin = DIO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DIO1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
  /*
   * 4 bit - nibble data LCD Display
   */
  uint8_t data = nibble << D4_BIT;
  data |= rs << RS_BIT;
  data |= backlight_state << BL_BIT; // Include backlight state in data
  data |= 1 << EN_BIT;
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
  HAL_Delay(1);
  data &= ~(1 << EN_BIT);
  HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
}

void lcd_send_cmd(uint8_t cmd)
{
  uint8_t upper_nibble = cmd >> 4;
  uint8_t lower_nibble = cmd & 0x0F;
  lcd_write_nibble(upper_nibble, 0);
  lcd_write_nibble(lower_nibble, 0);
  if (cmd == 0x01 || cmd == 0x02) {
    HAL_Delay(2);
  }
}

void lcd_send_data(uint8_t data)
{
  uint8_t upper_nibble = data >> 4;
  uint8_t lower_nibble = data & 0x0F;
  lcd_write_nibble(upper_nibble, 1);
  lcd_write_nibble(lower_nibble, 1);
}

void lcd_init() {
  HAL_Delay(50);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(5);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(1);
  lcd_write_nibble(0x03, 0);
  HAL_Delay(1);
  lcd_write_nibble(0x02, 0);
  lcd_send_cmd(0x28);
  lcd_send_cmd(0x0C);
  lcd_send_cmd(0x06);
  lcd_send_cmd(0x01);
  HAL_Delay(2);
}

void lcd_write_string(char *str) {
  while (*str) {
    lcd_send_data(*str++);
  }
}

void lcd_set_cursor(uint8_t row, uint8_t column) {
    uint8_t address;
    switch (row) {
        case 0:
            address = 0x00;
            break;
        case 1:
            address = 0x40;
            break;
        default:
            address = 0x00;
    }
    address += column;
    lcd_send_cmd(0x80 | address);
}

void lcd_clear(void) {
	lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_backlight(uint8_t state) {
  if (state) {
    backlight_state = 1;
  } else {
    backlight_state = 0;
  }
}
/*
void count_seven_segment(void) {
	if ( count_i >= 10000 ) count_i = 0;
	displayColon = !displayColon;
	TM1637_DisplayDecimal(count_i, displayColon);
	count_i++;
}
*/

LCDAT split(char* input, const char* delimiter)
{
	char** result = 0;
	size_t count = 0;
	char* tmp = input;
	char* last_delim = 0;
	char delim[2];
	delim[0] = delimiter[0];
	delim[1] = 0;

	/* Count how many elements will be in the array */
	while (*tmp) {
		if (delimiter[0] == *tmp )
		{
			count++;
			last_delim = tmp;
		}

		tmp++;
	}

	/* Add space for trailing token */
	count += last_delim < (input+strlen(input) - 1 );

	/* Add space for terminating null string */
	count++;

	result = malloc(sizeof(char*) * count);

	if (result)
	{
		size_t idx = 0;
		char* token = strtok(input, delim);

		while (token)
		{
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx==count-1);
		*(result + idx) = 0;
	}

	LCDAT ArtisTitle;
	ArtisTitle.tokens = result;
	ArtisTitle.count = count - 1;

	return ArtisTitle;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
