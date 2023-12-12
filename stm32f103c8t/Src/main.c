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
#include "lcd.h"
#include "serial_communication.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LCD_SCROLL_TIME 1500
#define COUNT_MS 965
#define LCD_MAX_LENGTH 16
#define DOUBLE_CLICK__TIME 700

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

// serial communication code
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

// ramps swtich btn
volatile uint8_t sw_state_stop_play=1; // 1: start, 0: stop

// serial send data
uint8_t data[8][1] = {
		{0x00}, {0x01}, // 000 001
		{0x02}, {0x03}, // 010 011
		{0x04}, {0x05}, // 011 100
		{0x06}, {0x07}  // 101 110
};

// btn flags
GPIO_PinState btn_flag_1 = 0;
GPIO_PinState btn_flag_2 = 0;
GPIO_PinState btn_flag_3 = 0;
GPIO_PinState btn_flag_4 = 0;

// next & prev double click ticks
uint32_t np_old_tick     = 0;
uint32_t np_current_tick = 0;
GPIO_PinState np_is_double_click = 0;

// volume up & down double click ticks
uint32_t vol_old_tick     = 0;
uint32_t vol_current_tick = 0;
GPIO_PinState vol_is_double_click = 0;

// display mode transform click ticks
uint32_t dis_old_tick     = 0;
uint32_t dis_current_tick = 0;
GPIO_PinState dis_is_double_click = 0;

// lcd display reset
GPIO_PinState display_reset = 0;

// duration variables
GPIO_PinState is_duration = 0;

// serial display types
char* display_type_1 = "song";
char* display_type_2 = "duration";
char* display_type_3 = "volume";
char* display_type_4 = "stop";
//uint32_t display_mode = 0;

GPIO_PinState music_play = 0;
GPIO_PinState song_info_blick  = 0;
GPIO_PinState duration_blick = 0;
GPIO_PinState volume_tick= 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
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
		// first press time
		np_current_tick = HAL_GetTick();
		// if double press
		if ( np_current_tick - np_old_tick < DOUBLE_CLICK__TIME && btn_flag_2 ==1 ) // last click - now click >= Double Clcik Time
		{
			// double click
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
		if ( vol_current_tick - vol_old_tick < DOUBLE_CLICK__TIME && btn_flag_3 ==1 )
		{
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
		display_reset = 1;
		btn_flag_4=1;
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

  // lcd code
  lcd_init();
  lcd_backlight(1);
  lcd_set_cursor(0, 1);
  lcd_write_string("Start Spotify!");

  // serial variables
  uint16_t serial_len = 0 ;

  uint32_t start_tick_title= HAL_GetTick();
  uint32_t start_tick_artists= HAL_GetTick();

  uint32_t song_info_tick= HAL_GetTick();
  uint32_t duration_tick = 0;

  LCDAT tokens;
  tokens.count = 0;
  tokens.tokens = NULL;

  MUSIC music_tmp;
  memset(music_tmp.ti_tmp, 0, sizeof(music_tmp.ti_tmp));
  memset(music_tmp.ar_tmp, 0, sizeof(music_tmp.ar_tmp));
  memset(music_tmp.vo_tmp, 0, sizeof(music_tmp.vo_tmp));

  LCD_SCROLL lcd_status;
  lcd_status.lcd_title_scroll   = 0;
  lcd_status.lcd_artists_scroll = 0;
  lcd_status.song_info_print = 0 ;
  lcd_status.scroll_flag = 0;
  lcd_status.display_mode = 0;

  TIMES time_tmp;
  time_tmp.duration_time_tmp = 0;
  time_tmp.full_time_tmp = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (display_reset == 1)
	  {
		  display_reset = 0;
		  lcd_clear();
	  }

	  if ( btn_flag_4 == 1 )
	  {
		  lcd_status.display_mode++;
		  if ( lcd_status.display_mode == 0 )
		  {
			  lcd_status.song_info_print=1;
		  }
		  else if ( lcd_status.display_mode == 1 )
		  {
			  duration_blick =0;
		  }
		  else if ( lcd_status.display_mode == 2 )
		  {
			  // volume logic
		  }
		  else
		  {
			  lcd_status.display_mode = 0;
		  }

		  btn_flag_4 = 0;
	  }

	  // Are you serial data ?
	  serial_len = strlen((const char*)UserRxBuffer);

	  // if serial data is here
	  if ( serial_len > 0) // buffer overflow
	  {
		  // RxBuffer -> TxBuffer
		  strncpy((char *)UserTxBufferFS, (const char*)UserRxBuffer,serial_len);
		  CDC_Transmit_FS(UserRxBuffer, serial_len);
		  UserTxBufferFS[serial_len] = '\0';

		  tokens.count = 0;
		  tokens.tokens = NULL;
		  free(tokens.tokens);
		  // split`,` data
		  tokens = split((char *)UserTxBufferFS,"|");
		  lcd_clear();

		  char* serial_type = *(tokens.tokens+0);

		  // when recive song info through serial
		  if ( strcmp(serial_type,display_type_1) == 0 )
		  {
			  if ( (tokens.count) == 4 )
			  {
				  music_play = 1;
				  rcv_song_process(&tokens,&music_tmp,&lcd_status);
				  start_tick_title = HAL_GetTick();
				  start_tick_artists = HAL_GetTick();

			  }
			  // when serial data len > 3
			  else
			  {
				  lcd_set_cursor(0, 0);
				  lcd_write_string("Data Missing");
			  }
		  }

		  // when receive song duration through serial
		  else if ( strcmp(serial_type,display_type_2) == 0)
		  {
			  if ( (tokens.count) == 3 )
			  {
				  music_play = 1;
				  rcv_time_process(&tokens, &time_tmp, &lcd_status);
				  duration_tick = HAL_GetTick();
			  }
			  else
			  {
				  lcd_set_cursor(0, 0);
				  lcd_write_string("Duration Not");
			  }
		  }

		  else if (strcmp(serial_type,display_type_3) == 0)
		  {
			  if ( (tokens.count) == 2 )
			  {
				  volume_screen(&tokens, &music_tmp, &lcd_status);
			  }
		  }
		  else if (strcmp(serial_type,display_type_4) == 0)
		  {
			  music_play=0;
			  rcv_stop_process(&tokens, &music_tmp);
		  }

		  // Essential memory init
		  memset(UserRxBuffer, 0, sizeof(UserRxBuffer));
		  memset(UserTxBufferFS, 0, sizeof(UserTxBufferFS));
		  serial_len = 0;
	  }

	  if ( btn_flag_1 == 1 )
	  {
		  btn_flag_1 = 0;
		  // excute logic
		  if ( sw_state_stop_play )
		  {   // play
			  CDC_Transmit_FS(data[0],1);
			  music_play = 1;
		  }
		  else
		  {   // stop
			  CDC_Transmit_FS(data[1],1);
			  music_play = 0;
			  lcd_status.lcd_title_scroll=0;
			  lcd_status.lcd_artists_scroll=0;
		  }
		  sw_state_stop_play = !sw_state_stop_play; // 버튼 상태를 변경합니다.
	  }

	  if ( np_is_double_click == 0 && btn_flag_2 == 1)
	  {
		  music_play=0;
		  btn_flag_2 = 0;
		  CDC_Transmit_FS(data[2], 1); // 010 next
	  }

	  if ( np_is_double_click == 1 && btn_flag_2 == 0 )
	  {
		  music_play=0;
		  np_is_double_click=0;
		  CDC_Transmit_FS(data[3], 1); // 011 prev
	  }

	  if ( vol_is_double_click == 0 && btn_flag_3 == 1)
	  {   // volume up
		  btn_flag_3 = 0;
		  lcd_clear();
		  CDC_Transmit_FS(data[4], 1); // 100 next
	  }
	  if ( vol_is_double_click == 1 && btn_flag_3 == 0 )
	  {   // volume down
		  vol_is_double_click=0;
		  CDC_Transmit_FS(data[5], 1); // 101 prev
	  }

	  if ( lcd_status.display_mode == 0 )
	  {   // screen 1
		  if ( music_play == 1 )
		  {
			  // title scrolling
			  if ( HAL_GetTick() - start_tick_title >= LCD_SCROLL_TIME)
			  {
				  if ( lcd_status.lcd_title_scroll == 1 )
				  {
					  int title_tmp_size = strlen(music_tmp.ti_tmp);
					  char ch_ti = 0;
					  ch_ti = music_tmp.ti_tmp[0];
					  strcpy(music_tmp.ti_tmp,music_tmp.ti_tmp+1);
					  music_tmp.ti_tmp[title_tmp_size-1] = ch_ti;
				  }
				  lcd_set_cursor(0, 0);
				  lcd_write_string(music_tmp.ti_tmp);
				  start_tick_title = HAL_GetTick();
			  }

			  // artists scrolling
			  if ( HAL_GetTick() - start_tick_artists >= LCD_SCROLL_TIME)
			  {
				  if ( lcd_status.lcd_artists_scroll == 1 )
				  {
					  int artists_tmp_size = strlen(music_tmp.ar_tmp);
					  char ch_ar = 0;
					  ch_ar = music_tmp.ar_tmp[0];
					  strcpy(music_tmp.ar_tmp,music_tmp.ar_tmp+1);
					  music_tmp.ar_tmp[artists_tmp_size-1] = ch_ar;
				  }
				  lcd_set_cursor(1, 0);
				  lcd_write_string(music_tmp.ar_tmp);
				  start_tick_artists = HAL_GetTick();
			  }

			  if ( lcd_status.song_info_print==1 )
			  {
				  lcd_status.song_info_print = 0;
				  lcd_song_info_printing(music_tmp.ti_tmp,music_tmp.ar_tmp);
			  }
		  }
		  else
		  {
			  if ( HAL_GetTick() - song_info_tick >= COUNT_MS )
			  {
				  if ( song_info_blick == 0 )
				  {
					  lcd_clear();
				  }
				  else
				  {
					  if ( strlen(music_tmp.ti_tmp) == 0 )
					  {
						  lcd_no_data_printing();
					  }
					  else
					  {
						  lcd_song_info_printing(music_tmp.ti_tmp,music_tmp.ar_tmp);
					  }
				  }
				  song_info_blick = !song_info_blick;
				  song_info_tick= HAL_GetTick();
			  }
		  }
	  }

	  // screen 2
	  if ( HAL_GetTick() - duration_tick >= COUNT_MS ) // 1초 지났을 때
	  {
		  if ( time_tmp.duration_time_tmp >= time_tmp.full_time_tmp && music_play == 1)
		  {
			  music_play = 0;
			  duration_blick = 0;
		  }

		  if ( lcd_status.display_mode == 1 )
		  {
			  if ( music_play == 1 )
			  {
				  lcd_duration_printing(time_tmp.duration_time_tmp,time_tmp.full_time_tmp);
			  }
			  else
			  { // no play
				  if ( duration_blick == 0 )
				  {
					  lcd_no_data_printing();
				  }
				  else
				  {
					  lcd_clear();
				  }
				  duration_blick = !duration_blick;
			  }
		  }

		  // music time counter because need keep going
		  if ( music_play == 1 )
		  {
			  time_tmp.duration_time_tmp += 1;
		  }

		  // Run 1 sec so duration tick setting
		  duration_tick = HAL_GetTick();
	  }

	  if ( lcd_status.display_mode == 2 )
	  {
		  if ( HAL_GetTick() - volume_tick >= COUNT_MS )
		  {
			  lcd_volume_printing(music_tmp.vo_tmp);
		  }
		  else
		  {
			  lcd_clear();
		  }
		  volume_tick=HAL_GetTick();
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
