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

#define LCD_ROWS 2 // Number of rows on the LCD
#define LCD_COLS 16 // Number of columns on the LCD

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

// lcd code
uint8_t backlight_state = 1;
uint8_t count_i = 0;

volatile uint8_t displayColon =0;

// ramps swtich btn
volatile uint8_t sw_state_stop_start=1; // 1: start, 0: stop
uint8_t data[6][1] = {
		{0x00}, {0x01},
		{0x02}, {0x03},
		{0x04}, {0x05}
};

GPIO_PinState btn_flag_1 = 0;
GPIO_PinState btn_flag_1_test = 0;
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
void count_seven_segment(void);

char** split(char* input, const char* delimiter);
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

  // seven segment code
  TM1637_SetBrightness(7);
  // lcd code
//  char *text = "test";
  char int_to_str[100] = {};

  // seven segment
//  int i = 0 ;
  char** tokens = NULL;
  uint16_t serial_len = 0 ;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  serial_len = strlen((const char*)UserRxBufferFS);

	  if ( serial_len > 0 )
	  {
		  strncpy((char *)UserTxBufferFS, (const char*)UserRxBufferFS,serial_len);
		  strncpy((char *)int_to_str,(const char*)UserTxBufferFS,serial_len);
		  tokens = split(int_to_str,",");
		  for ( int i = 0 ; *(tokens +i); i++)
		  {
			  char* token_str = *(tokens + i);
			  char token_str_with_a[100] = {};
			  strcpy(token_str_with_a, token_str);
			  strcat(token_str_with_a, "a");
			  CDC_Transmit_FS((uint8_t*)token_str_with_a, strlen(token_str_with_a));
			  free(*(tokens + i));
		  }
	      free(tokens);
	      tokens = NULL;
	      memset(UserRxBufferFS, 0, serial_len);
	      memset(UserTxBufferFS, 0, serial_len);
	  }

	  if ( btn_flag_1 == 1 )
	  {
		  lcd_clear(); lcd_set_cursor(0, 0); btn_flag_1 = 0;

		  if ( sw_state_stop_start )
		  {
			  lcd_write_string("start");
//			  CDC_Transmit_FS((uint8_t *)"0000\n\r",strlen("0000\n\r"));
			  CDC_Transmit_FS(data[0],1);
		  }
		  else
		  {
			  lcd_write_string("stop");
//			  CDC_Transmit_FS((uint8_t *)"0001\n\r",strlen("0001\n\r"));
			  CDC_Transmit_FS(data[1],1);
		  }

		  sw_state_stop_start = !sw_state_stop_start; // 버튼 상태를 변경합니다.
		  count_seven_segment();
	  }

	  // ramps switch 1.4 example
//	  if ( !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) & sw_state_1_off )
//	  {
//		  lcd_clear();
//		  lcd_set_cursor(0, 0);
//		  lcd_write_string("ON");
//		  sw_state_1_on = 1;
//		  sw_state_1_off= 0;
//		  count_seven_segment();
//	  }
//	  else if ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) & sw_state_1_on )
//	  {
//		  lcd_clear();
//		  lcd_set_cursor(0, 0);
//		  lcd_write_string("OFF");
//		  sw_state_1_off = 1;
//		  sw_state_1_on = 0;
//	  }

	  // seven segment code
//	  if ( i >= 10000 ) i = 0;
//	  displayColon = !displayColon;
//	  TM1637_DisplayDecimal(i, displayColon);
//	  i++;
	  /*
	   * lcd stm32 to PCf8574 I/O i2c controll code
	   */
//	  lcd_clear();
//	  lcd_set_cursor(0, 0);
//	  lcd_write_string(text);
//	  lcd_set_cursor(1, 0);
//	  lcd_write_string(int_to_str);
//	  memset(int_to_str, 0, sizeof(int_to_str));
//	  HAL_Delay(1500);

	  /*
	   * stm32 to pc / serial communication code
	   * strlen result UserRxBufferFs Size
	   * \0 을 만나기 전까지의 길이를 반환함 const char * -> 변경 불가 상수 문자열
	   * strcat((char *)UserTxBufferFS,"\r\n"); // 끝에 문자열 추가하는 거
	   */

//	  uint16_t len = strlen((const char*)UserRxBufferFS);
//
//	  if ( len > 0 )
//	  { // is data ?
//		  strncpy((char *)UserTxBufferFS, (const char*)UserRxBufferFS,len);
//	  	  CDC_Transmit_FS((uint8_t*)UserTxBufferFS, strlen((const char*)UserTxBufferFS)); // 보내기
//	  	  // memory init , 버퍼 초기화 안하면 꼬임
//	  	  memset(UserRxBufferFS, 0, sizeof(UserRxBufferFS)); // 초기화
//	  	  memset(UserTxBufferFS, 0, sizeof(UserTxBufferFS)); // 초기화
//	  }

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

  /*Configure GPIO pin : START_STOP_BTN_Pin */
  GPIO_InitStruct.Pin = START_STOP_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(START_STOP_BTN_GPIO_Port, &GPIO_InitStruct);

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

void count_seven_segment(void) {
	if ( count_i >= 10000 ) count_i = 0;
	displayColon = !displayColon;
	TM1637_DisplayDecimal(count_i, displayColon);
	count_i++;
}

char** split(char* input, const char* delimiter)
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

	return result;
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
