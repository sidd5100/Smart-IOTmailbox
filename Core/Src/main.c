/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "RC522.h"
#include "2RC522.h"
#include "string.h"
#include "liquidcrystal_i2c.h"
#include "stdlib.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define startAddress 0x080E0000

typedef struct{
	uchar mailCount;
	uchar parcelCount;
	uchar mailFlag;
	uchar parcelFlag;
}user_t;

typedef enum {
	HOME_SCREEN,
    MAIN_MENU,
    USER_MENU,
    MAIL_MENU,
    PACKAGE_MENU,
    CHECK_MAIL,
    RETRIEVE_MAIL,
    CHECK_PACKAGE,
    RETRIEVE_PACKAGE
} MenuState_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRUE 1
#define FALSE 0
#define MAX_ATTEMPTS 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uchar status;
uchar str[16];
uchar sNum[5];

//user UID's
uchar user1[5] = {227,255,193,12,209};
uchar user2[5] = {163,139,152,61,141};
uchar user3[5] = {181,129,175,33,186};
uchar user4[5] = {85,31,230,33,141};
uchar user5[5] = {201,78,64,148,83};
uchar user6[5] = {57,149,59,148,3};


user_t user1Info = {0,0,0,0};
user_t user2Info = {0,0,0,0};
user_t user3Info = {0,0,0,0};

MenuState_t currentState = MAIN_MENU;
uint8_t currentUser = 0;  // User 1, 2, or 3

char txData[];


char genPassword1[5] = "1234";
char genPassword2[5] = "5678";
char genPassword3[5] = "9876";
char genPassword4[5] = "5555";
char genPassword5[5] = "6666";
char genPassword6[5] = "7777";
char inputPassword[5];   // Buffer to store entered password
uint8_t passwordIndex = 0;
uint8_t attempts = 0;
bool enteringPassword = FALSE;

char buf[70];
char buf1[8];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */


char scanKeys(void);
uchar compareIntArrays(uchar *arr1, uchar *arr2, uchar size);
uchar readtagsFromRDR1 ();
uchar readtagsFromRDR2 ();
void clearArrayEle (uchar *arrPtr, uchar size);
uchar checkValidUser(uchar *arr1, uchar *arr2, uchar *arr3, uchar *arr4, uchar *arr5, uchar *arr6, uchar size);

void welcomeMsglcd (void);
void newNotificationlcd(const char *line1, const char *line2, uint16_t delay_ms);
void setLCDHMIscreen(const char *line1, const char *line2);

void DisplayMenu(MenuState_t state, char key);
char* getUserPassword();
void HandleInput(char key);

void generate_password(uchar *password);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uchar statusRDR1;
	uchar statusRDR2;
	uchar compareFlag;
	uchar compareFlag2;
	uchar userValidmail;
	uchar userValidpack;

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
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */


  MFRC522_1_Init();
  MFRC522_2_Init();

  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);

  HD44780_Init(2);
  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("HELLO");
  HD44780_SetCursor(6,0);
  HD44780_PrintStr("WORLD");
  HAL_Delay(2000);
  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("Welcome to ");
  HD44780_SetCursor(0,1);
  HD44780_PrintStr("Smart IOTmailbox");
  HAL_Delay(5000);
	//HD44780_Clear();
	//HD44780_SetCursor(0,0);
	//HD44780_PrintStr("Press 0 To Strt");


  /*LCD code
  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("HELLO");
  HAL_Delay(2000);
  HD44780_NoBacklight();
  HAL_Delay(2000);
  HD44780_Backlight();

  HAL_Delay(2000);
  HD44780_Cursor();
  HAL_Delay(2000);
  HD44780_Blink();
  HAL_Delay(5000);
  HD44780_NoBlink();
  HAL_Delay(2000);
  HD44780_NoCursor();
  HAL_Delay(2000);

  HD44780_NoDisplay();
  HAL_Delay(2000);
  HD44780_Display();

  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("Learning STM32 with LCD is fun :-)");
  int x;
  for(int x=0; x<40; x=x+1)
  {
	 HD44780_ScrollDisplayLeft();  //HD44780_ScrollDisplayRight();
	 HAL_Delay(500);
  }

  char snum[5];
  for ( int x = 1; x <= 200 ; x++ )
  {
      itoa(x, snum, 10);
	  HD44780_Clear();
	  HD44780_SetCursor(0,0);
	  HD44780_PrintStr(snum);
	  HAL_Delay (1000);
  }*/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	 //scan for key pressed
		 char key = scanKeys();
		 if(key)
		 {
			 //function for action related to key pressed
			 HandleInput(key);//HMI function
		 }


	  //Scan from reader 1 for post mails
	  	statusRDR1 = readtagsFromRDR1();
	  	//IF valid read then enter
	  	if(! statusRDR1)
	  	{
	  		userValidmail = checkValidUser(sNum, user1, user2, user3, user4, user5, 5);
	  		if(userValidmail)
	  		{
	  			//user 1 is assigned two mails
				compareFlag =  compareIntArrays(sNum, user1, 5);
				compareFlag2 =  compareIntArrays(sNum, user4, 5);
				//check how many mails user 1 has
				if(compareFlag || compareFlag2 )
				{
					user1Info.mailCount++;
					if(user1Info.mailFlag != TRUE)
					{
						user1Info.mailFlag = TRUE;
					}

					//If has one mail
					if(user1Info.mailCount == 1 )
					{
						 //sorting flap
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1100);
						HAL_Delay(2000);
						//top flap
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
						HAL_Delay(1000);

						//re-allign flap to initial position
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
						HAL_Delay(1000);
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1500);

						newNotificationlcd("User 1 got", "a new mail", 2000);
						generate_password(genPassword1);
						newNotificationlcd(genPassword1, " ", 1000);

						for(int i=0; i<30;i++)
						{
							HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
							HAL_Delay(100); // 10ms delay
						}
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);

						//save the notification locally and send it to the ESP32
						sprintf(buf1,"USER1\n\r");
						HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
						clearArrayEle(buf1 ,8);

						//save the notification locally and send it to the ESP32
						sprintf(buf,"User 1 has got a new mail. Your one time password is %s\n",genPassword1);
						HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
						clearArrayEle(buf ,70);

					}

					//if has more than one mail
					else if(user1Info.mailCount > 1 )
					{
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1100);
						HAL_Delay(2000);
						//top flap
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
						HAL_Delay(1000);

						//re-allign flap to initial position
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
						HAL_Delay(1000);
						__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1500);

						newNotificationlcd("User 1 got", "2 new mail", 2000);
						generate_password(genPassword1);
						newNotificationlcd(genPassword1, " ", 1000);
						//flash led for 3 secs and set it
						for(int i=0; i<30;i++)
						{
							HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
							HAL_Delay(100); // 10ms delay
						}
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
						//save the notification locally and send it to the ESP32
						sprintf(buf1,"USER1\n\r");
						HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
						clearArrayEle(buf1 ,8);
						//save the notification locally and send it to the ESP32
						sprintf(buf,"User 1 has got 2 new mails. Your one time password is %s\n",genPassword1);
						HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
						clearArrayEle(buf ,70);
					}
					clearArrayEle(sNum ,5);
					welcomeMsglcd();


				}

				//if user 2 has mail
				compareFlag =  compareIntArrays(sNum, user2, 5);
				if(compareFlag)
				{
					if(user2Info.mailFlag != TRUE)
					{
						user2Info.mailFlag = TRUE;
					}
					//sorting flap
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 800);
					HAL_Delay(2000);
					//top flap
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
					HAL_Delay(1000);

					//re-allign flap to initial position
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
					HAL_Delay(1000);
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1500);

					newNotificationlcd("User 2 got", "a new mail", 2000);
					generate_password(genPassword2);
					newNotificationlcd(genPassword2, " ", 1000);
					//flash led for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

					//save the notification locally and send it to the ESP32
					sprintf(buf1,"USER2\n\r");
					HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
					clearArrayEle(buf1 ,8);

					//save the notification locally and send it to the ESP32
					sprintf(buf,"User 2 has got a new mail. Your one time password is %s\n\r",genPassword2);
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
					clearArrayEle(buf ,70);

					clearArrayEle(sNum ,5);
					welcomeMsglcd();
				}

				//if user 3 has mail
				compareFlag =  compareIntArrays(sNum, user3, 5);
				if(compareFlag)
				{
					if(user3Info.mailFlag != TRUE)
					{
						user3Info.mailFlag = TRUE;
					}
					//intake flap
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
					HAL_Delay(3000);
					//re-allign flap to initial position
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);

					newNotificationlcd("User 3 got", "a new mail", 2000);
					generate_password(genPassword3);
					newNotificationlcd(genPassword3, " ", 1000);
					//flash led for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

					//save the notification locally and send it to the ESP32
					sprintf(buf,"USER3\n");
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
					HAL_Delay(200);

					//save the notification locally and send it to the ESP32
					sprintf(buf,"User 3 has got a new mail. Your one time password is %s\n",genPassword3);
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);

					clearArrayEle(sNum ,5);
					welcomeMsglcd();
				}
	  		}//end of valid user

			//if not a valid user display message
	  		else
	  		{
	  			newNotificationlcd("Not a valid user", " ", 2000);
	  		}
	  		clearArrayEle(sNum ,5);
	  		welcomeMsglcd();
	  	}//end of first read

	  	//Scan from reader 2 for packages
	  	statusRDR2 = readtagsFromRDR2();

	  	//if a valid read
	  	if(! statusRDR2)
	  	{
	  		userValidpack = checkValidUser(sNum, user1, user2, user3, user4, user5, 5);
	  		if(userValidpack)
	  		{
	  			//Operate compartment only for 1 user. Should not operate if other User has package
				compareFlag =  compareIntArrays(sNum, user1, 5);
				compareFlag2 =  compareIntArrays(sNum, user5, 5);
				if((compareFlag || compareFlag2) && !(user1Info.parcelFlag) && !(user2Info.parcelFlag) && !(user3Info.parcelFlag))
				{
					if(user1Info.parcelFlag != TRUE)
					{
						user1Info.parcelFlag = TRUE;
					}
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
					setLCDHMIscreen("Put the package","in the box below");
					//allow delay to place the package and close after delay
					HAL_Delay(10000);
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

					newNotificationlcd("User 1 got", "a new package", 2000);
					//generate new password and display
					generate_password(genPassword4);
					newNotificationlcd(genPassword4, " ", 3000);
					//flash led for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

					//save the notification locally and send it to the ESP32
					sprintf(buf1,"USER1\n\r");
					HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
					clearArrayEle(buf1 ,8);
					//save the notification locally and send it to the ESP32
					sprintf(buf,"User 1 has got a new package. Your one time password is %s\n",genPassword4);
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);

					clearArrayEle(sNum ,5);
					welcomeMsglcd();
				}

				//Operate compartment only for 1 user. Should not operate if other User has package
				compareFlag =  compareIntArrays(sNum, user2, 5);
				if(compareFlag && !(user1Info.parcelFlag) && !(user2Info.parcelFlag) && !(user3Info.parcelFlag))
				{
					if(user2Info.parcelFlag != TRUE)
					{
						user2Info.parcelFlag = TRUE;
					}

					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
					setLCDHMIscreen("Put the package","in the box below");
					//allow delay to place the package and close after delay
					HAL_Delay(10000);
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

					newNotificationlcd("User 2 got", "a new package", 2000);
					//generate new password and display
					generate_password(genPassword5);
					newNotificationlcd(genPassword5, " ", 3000);
					//flash led for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

					//save the notification locally and send it to the ESP32
					sprintf(buf1,"USER2\n\r");
					HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
					clearArrayEle(buf1 ,8);
					//save the notification locally and send it to the ESP32
					sprintf(buf,"User 2 has got a new package. Your one time password is %s\n",genPassword5);
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);

					clearArrayEle(sNum ,5);
					welcomeMsglcd();
				}

				//Operate compartment only for 1 user. Should not operate if other User has package
				compareFlag =  compareIntArrays(sNum, user3, 5);
				if(compareFlag && !(user1Info.parcelFlag) && !(user2Info.parcelFlag) && !(user3Info.parcelFlag))
				{
					if(user3Info.parcelFlag != TRUE)
					{
						user3Info.parcelFlag = TRUE;
					}

					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
					setLCDHMIscreen("Put the package","in the box below");
					//allow delay to place the package and close after delay
					HAL_Delay(10000);
					__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

					newNotificationlcd("User 3 got", "a new mail", 2000);
					//generate new password and display
					generate_password(genPassword6);
					newNotificationlcd(genPassword6, " ", 3000);
					//flash LED for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

					//save the notification locally and send it to the ESP32
					sprintf(buf1,"USER3\n\r");
					HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
					clearArrayEle(buf1 ,8);
					//save the notification locally and send it to the ESP32
					sprintf(buf,"User 2 has got a new package. Your one time password is %s\n",genPassword6);
					HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);

					clearArrayEle(sNum ,5);
					welcomeMsglcd();
				}

				//If not a valid read display message
				if(user1Info.parcelFlag || user2Info.parcelFlag || user1Info.parcelFlag)
				{
					newNotificationlcd("Already Occupied", " ", 2000);
					//flash led for 3secs and set it
					for(int i=0; i<30;i++)
					{
						HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
						HAL_Delay(100); // 10ms delay
					}
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
				}
				clearArrayEle(sNum ,5);
				welcomeMsglcd();
	  		}//end of valid user

	  		else
	  		{
	  			newNotificationlcd("Not a valid user", " ", 2000);
	  		}
	  		clearArrayEle(sNum ,5);
	  		welcomeMsglcd();
	  	}//end of second read

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, R1_pin_Pin|R2_pin_Pin|R3_pin_Pin|R4_pin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 PC6 PC7
                           PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : R1_pin_Pin R2_pin_Pin R3_pin_Pin R4_pin_Pin */
  GPIO_InitStruct.Pin = R1_pin_Pin|R2_pin_Pin|R3_pin_Pin|R4_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : C1_pin_Pin C2_pin_Pin C4_pin_Pin C3_pin_Pin */
  GPIO_InitStruct.Pin = C1_pin_Pin|C2_pin_Pin|C4_pin_Pin|C3_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//Function to erase flash memory



//FUnction to scan keys from keypad
/*Description : 	This function reads the keypad connected to uC and dtermines which key was pressed
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		None
Output: 		Char '' value of the key pressed
*/
char scanKeys(void)
{
    // Set all rows LOW initially
    HAL_GPIO_WritePin(R1_pin_GPIO_Port, R1_pin_Pin | R2_pin_Pin | R3_pin_Pin | R4_pin_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);  // Small delay for stability

    char keypad[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    char lastKey = 0;  // Stores the last key pressed, initialized to no key
    char currentKey = 0;  // Stores the currently detected key
    char noKey = 0;  // No key pressed condition


    GPIO_TypeDef* row_ports[4] = {R1_pin_GPIO_Port, R2_pin_GPIO_Port, R3_pin_GPIO_Port, R4_pin_GPIO_Port};
    uint16_t row_pins[4] = {R1_pin_Pin, R2_pin_Pin, R3_pin_Pin, R4_pin_Pin};

    GPIO_TypeDef* col_ports[4] = {C1_pin_GPIO_Port, C2_pin_GPIO_Port, C3_pin_GPIO_Port, C4_pin_GPIO_Port};
    uint16_t col_pins[4] = {C1_pin_Pin, C2_pin_Pin, C3_pin_Pin, C4_pin_Pin};

    // Read the keypad in rows
    for (int row = 0; row < 4; row++)
    {
        // Set all rows low, then one row high at a time
        // Reset all rows to low
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_RESET);
        // Set one row high
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_SET);
        HAL_Delay(1);  // Short delay to stabilize

        // Check each column for key press
        for (int col = 0; col < 4; col++)
        {
            if (HAL_GPIO_ReadPin(col_ports[col], col_pins[col]) == GPIO_PIN_SET)
            {
                currentKey = keypad[row][col];  // Store the current key

                // Register the key press only if it is not already registered
                if (currentKey != lastKey)
                {
                    lastKey = currentKey;  // Update lastKey to current key
                    return currentKey;  // Immediately return the key that is pressed
                }
            }
        }
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(R1_pin_GPIO_Port, R1_pin_Pin | R2_pin_Pin | R3_pin_Pin | R4_pin_Pin, GPIO_PIN_RESET);
    // If no key is pressed, return the "no press" state
    if (currentKey == noKey)
    {
        return '\0';  // Return '0' or some indicator to show no key is pressed
    }
}


//Function to read from Tag reader1
/*Description:  This function reads from the MFRC522 reader1 using polling method in uC. Reader is connected on SPI bus,
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		None
Output: 		uint8_t: either 0,1,2 0 for sucessful read and 2 if error occured 1 if incomplete read
*/
uchar readtagsFromRDR1 (void)
{
	uchar readSucess;
	uchar status;
	Write_1_MFRC522(Status2Reg, 0);
	Read_1_MFRC522(Status2Reg);
	readSucess = MFRC522_1_Request( PICC_REQIDL , str);
	if(readSucess == MI_OK)
	{
	  readSucess = MFRC522_1_Anticoll(str);
	  if(readSucess == MI_OK)
	  {
		  memcpy(sNum,str,5);
		  status = MFRC522_1_SelectTag(sNum);
		  if(status > 0)
		  {
			  MFRC522_1_Halt();// Halt previous card
			  MFRC522_1_StopCrypto1();

		  }
	  }
	}
	MFRC522_1_Halt();// Halt previous card
	MFRC522_1_StopCrypto1();
	HAL_Delay(100);
	return readSucess;
}

//Function to read from Tag reader2
/*Description:  This function reads from the MFRC522 reader2 using polling method in uC. Reader is connected on SPI bus,
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		None
Output: 		uint8_t: either 0,1,2 0 for successful read and 2 if error occured 1 if incomplete read
*/
uchar readtagsFromRDR2 (void)
{
	uchar readSucess;
	uchar status;
	Write_2_MFRC522(Status2Reg, 0);
	Read_2_MFRC522(Status2Reg);
	readSucess = MFRC522_2_Request( PICC_REQIDL , str);
	if(readSucess == MI_OK)
	{
      readSucess = MFRC522_2_Anticoll(str);
	  if(readSucess == MI_OK)
	  {
		  memcpy(sNum,str,5);
		  status = MFRC522_2_SelectTag(sNum);
		  if(status > 0)
		  {
			  MFRC522_2_Halt();// Halt previous card
			  MFRC522_2_StopCrypto1();

		  }
	  }
	}
	MFRC522_2_Halt();// Halt previous card
	MFRC522_2_StopCrypto1();
	HAL_Delay(100);
	return readSucess;
}

//function to compare two int8 arrays
/*Description:  This function compares the content of the 2 8bit int arrays
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		pointer to the arrays, size of the array
Output: 		returns true if all contents of array matches
*/
uchar compareIntArrays(uchar *arr1, uchar *arr2, uchar size)
{
    for (uchar i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return 0; // Arrays are different
        }
    }
    return 1; // Arrays are equal
}

//Function to clear int8 array elements
/*Description:  This function deletes the content of the 8bit int arrays
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		pointer to the array, size of the array
Output: 		returns true if all contents of array matches
*/
void clearArrayEle (uchar *arrPtr, uchar size)
{
	for(uchar i = 0; i < size; i++)
	{
		arrPtr[i]=0;
	}
}

//Function to check a valid user and display message
/*Description:  This function compares the content of the 8bit int arrays to determine whether the users UID read from the card belongs to
				the predefined users
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		pointer to the predefined user UID arrays and pointer to the UID read from the RFID tag, Size of the arrays
Output: 		returns true if read UID matches with predefined user UID's
*/
uchar checkValidUser(uchar *arr1, uchar *arr2, uchar *arr3, uchar *arr4, uchar *arr5, uchar *arr6, uchar size)
{
	uchar valid1 = 0;
	uchar valid2 = 0;
	uchar valid3 = 0;
	uchar valid4 = 0;
	uchar valid5 = 0;

	valid1 = compareIntArrays(arr1, arr2, size);
	valid2 = compareIntArrays(arr1, arr3, size);
	valid3 = compareIntArrays(arr1, arr4, size);
	valid4 = compareIntArrays(arr1, arr5, size);
	valid5 = compareIntArrays(arr1, arr6, size);

	if(valid1 || valid2 || valid3 || valid4 || valid5)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//function to display welcome message
/*Description:  This function sets the LCD display with Welcome message
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		none
Output: 		none
*/
void welcomeMsglcd (void)
{
	HD44780_Clear();
	HD44780_SetCursor(0,0);
	HD44780_PrintStr("Welcome to ");
	HD44780_SetCursor(0,1);
	HD44780_PrintStr("Smart IOTmailbox");
}

//Function to display new mail/package notification
/*Description:  This function diplays new message on the LCD display for set period of time
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		literal string value of length 16 characters,literal string value of length 16 characters,time in msec for message to display
Output: 		none
*/
void newNotificationlcd(const char *line1, const char *line2, uint16_t delay_ms)
{
	HD44780_Clear();
	HD44780_SetCursor(0,0);
	HD44780_PrintStr(line1);
	HD44780_SetCursor(0,1);
	HD44780_PrintStr(line2);
	HAL_Delay(delay_ms);
}

//Function to set a new screen on display
/*Description:  This function sets the LCD diplay with a message untill next command
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		literal string value of length 16 characters,literal string value of length 16 characters
Output: 		none
*/
void setLCDHMIscreen(const char *line1, const char *line2)
{
	HD44780_Clear();
	HD44780_SetCursor(0,0);
	HD44780_PrintStr(line1);
	HD44780_SetCursor(0,1);
	HD44780_PrintStr(line2);
}

//Function to display menu screens
/*Description:  This function diplays a particular message upon an indexing variable passed to it.
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		Indexing variable.(0-8 values of this variable has a message assigned to it)
Output: 		none
*/
void DisplayMenu(MenuState_t state, char key)
{
	uchar buffer[20];
	char astrik[5] = "****";
	astrik[passwordIndex] = '\0';  // Adjust * display

	switch(state)
	{
		case HOME_SCREEN:
			welcomeMsglcd();
			break;
		case MAIN_MENU:
			setLCDHMIscreen("A:User1 B:User2","C:User3 0:Exit");
			break;
		case USER_MENU:
			setLCDHMIscreen("1:Mail 2:Package","#:Back *:Home");
			break;
		case MAIL_MENU:
			setLCDHMIscreen("1:Check 2:Retrieve","#:Back *:Home");
			break;
		case PACKAGE_MENU:
			setLCDHMIscreen("1:Check 2:Retrieve","#:Back *:Home");
			break;
		case CHECK_MAIL:
			if(currentUser == 1)
			{
				sprintf(buffer,"User1 has %d",user1Info.mailCount);
				newNotificationlcd(buffer,"mails",2000);
				welcomeMsglcd();
			}
			if(currentUser == 2)
			{
				sprintf(buffer,"User2 has %d",user2Info.mailFlag);
				newNotificationlcd(buffer,"mail",2000);
				welcomeMsglcd();
			}
			if(currentUser == 3)
			{
				sprintf(buffer,"User3 has %d",user3Info.mailFlag);
				newNotificationlcd(buffer,"mail",2000);
				welcomeMsglcd();
			}
			break;
		case RETRIEVE_MAIL:
			if (currentState == RETRIEVE_MAIL)
			{
				enteringPassword = TRUE;
				passwordIndex = 0;
				attempts = 0;
				setLCDHMIscreen("Enter Password", "****");
			}
			break;
		case CHECK_PACKAGE:
			if(currentUser == 1)
			{
				if(user1Info.parcelFlag)
				{
					newNotificationlcd("User1 has a","Package",2000);
					welcomeMsglcd();
				}
				else{
					newNotificationlcd("User1 has no","Package",2000);
					welcomeMsglcd();
				}

			}
			if(currentUser == 2)
			{
				if(user2Info.parcelFlag)
				{
					newNotificationlcd("User2 has a","Package",2000);
					welcomeMsglcd();
				}
				else{
					newNotificationlcd("User2 has no","Package",2000);
					welcomeMsglcd();
				}
			}
			if(currentUser == 3)
			{
				if(user3Info.parcelFlag)
				{
					newNotificationlcd("User3 has a","Package",2000);
					welcomeMsglcd();
				}
				else{
					newNotificationlcd("User3 has no","Package",2000);
					welcomeMsglcd();
				}
			}
			break;
		case RETRIEVE_PACKAGE:
			if (currentState == RETRIEVE_PACKAGE)
			{
				enteringPassword = TRUE;
				passwordIndex = 0;
				attempts = 0;
				setLCDHMIscreen("Enter Password", "****");
			}
			break;
		default:
			break;
	}
}

// Function to get the correct password based on the user
/*Description:  This function gets the random 4 digit generated password for USER1/2/3. If Retriev mail option is selected it generates 4digit numeric password
  	  	  	  	unique to each user
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		none
Output: 		pointer to the array that contains the password. Unique for each user both in mail and package mode
*/
char* getUserPassword()
{
    switch (currentUser)
    {
    		case 1:
                return (currentState == RETRIEVE_MAIL) ? genPassword1 : genPassword4;
            case 2:
                return (currentState == RETRIEVE_MAIL) ? genPassword2 : genPassword5;
            case 3:
                return (currentState == RETRIEVE_MAIL) ? genPassword3 : genPassword6;
            default:
                return "0000";
    }
}

//Function to scroll through menu screens upon input
/*Description:  This function does the password matching and retrival operation. if password matches then retives the mail/package upon menu selected
 * 				Resets the count flag/variable of the user if mail/packages retrieved and sends the message to the user of retrival notification
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		key: '' literal value of the key pressed by the user on keypad returned for scanKeys(); function
Output: 		none
*/
void HandleInput(char key)
{
	char temp[5];
	if (enteringPassword) {
	        if (key >= '0' && key <= '9' && passwordIndex < 4) {
	            inputPassword[passwordIndex++] = key;
	            inputPassword[passwordIndex] = '\0';
	            char maskedDisplay[5] = "****";  // Masked display
	            maskedDisplay[passwordIndex] = '\0';
	            setLCDHMIscreen("Enter Password", maskedDisplay);
	        }
	        else if (key == '#' && passwordIndex > 0) {  // Backspace functionality
	            passwordIndex--;
	            inputPassword[passwordIndex] = '\0';
	            char maskedDisplay[5] = "****";
	            maskedDisplay[passwordIndex] = '\0';
	            setLCDHMIscreen("Enter Password", maskedDisplay);
	        }
	        else if (passwordIndex == 4) {

	        	strcpy(temp,getUserPassword());
	            if (strcmp(inputPassword, getUserPassword()) == 0) {
	                setLCDHMIscreen("Access Granted", "Opening...");
	                HAL_Delay(2000);
	                welcomeMsglcd();
	                if(currentUser == 1)
	                {
	                	if (currentState == RETRIEVE_MAIL)
						{
							user1Info.mailCount = 0;
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
							sprintf(buf1,"USER1\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 1 all mails were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
						}
	                	if(currentState == RETRIEVE_PACKAGE)
	                	{
	                		user1Info.parcelFlag = FALSE;
	                		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

	                		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
							//allow delay to place the package and close after delay
							HAL_Delay(10000);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

	                		sprintf(buf1,"USER1\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 1 all packages were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
	                		/*__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 2500);
	                		HAL_Delay(3000);
	                		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);*/
	                	}
	                }
	                if(currentUser == 2)
	                {
	                	if(currentState == RETRIEVE_MAIL)
	                	{
	                		user2Info.mailFlag = FALSE;
	                		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	                		sprintf(buf1,"USER2\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 2 all mails were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
	                	}
	                	if(currentState == RETRIEVE_PACKAGE)
						{
							user2Info.parcelFlag = FALSE;
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
							//allow delay to place the package and close after delay
							HAL_Delay(10000);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

							sprintf(buf1,"USER2\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 2 all packages were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
							/*__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 2500);
							HAL_Delay(3000);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);*/
						}
	                }
	                if(currentUser == 3)
	                {
	                	if(currentState == RETRIEVE_MAIL)
	                	{
	                		user3Info.mailFlag = FALSE;
	                		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	                		sprintf(buf1,"USER3\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 3 all mails were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
	                	}
	                	if(currentState == RETRIEVE_PACKAGE)
						{
							user2Info.parcelFlag = FALSE;
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 1500);
							//allow delay to place the package and close after delay
							HAL_Delay(10000);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 500);

							sprintf(buf1,"USER3\n\r");
							HAL_UART_Transmit(&huart2,buf1,sizeof(buf1), HAL_MAX_DELAY);
							clearArrayEle(buf1 ,8);

							//save the notification locally and send it to the ESP32
							sprintf(buf,"User 3 all packages were retrieved\n");
							HAL_UART_Transmit(&huart2,buf,sizeof(buf), HAL_MAX_DELAY);
							clearArrayEle(buf ,70);
							/*__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 2500);
							HAL_Delay(3000);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);*/
						}
	                }
	                currentState = MAIN_MENU;
	                enteringPassword = FALSE;
	            } else {
	                attempts++;
	                if (attempts >= 3) {
	                    setLCDHMIscreen("Access Denied", "Too Many Tries");
	                    HAL_Delay(2000);
	                    welcomeMsglcd();
	                    currentState = MAIN_MENU;
	                    enteringPassword = FALSE;
	                } else {
	                    setLCDHMIscreen("Incorrect!", "Try Again");
	                    HAL_Delay(1500);
	                    setLCDHMIscreen("Enter Password", "****");
	                    passwordIndex = 0;
	                }
	            }
	        }
	        return;
	    }


	switch(key)
	{
		case '0':
			if (!enteringPassword) currentState = HOME_SCREEN;
			break;
		case '*':
			currentState = MAIN_MENU;
			break;
		case '#':
			if(currentState == CHECK_MAIL || currentState == RETRIEVE_MAIL)
			{
				currentState = MAIL_MENU;
			}
			else if(currentState == CHECK_PACKAGE || currentState == RETRIEVE_PACKAGE)
			{
				currentState = PACKAGE_MENU;
			}
			else if(currentState == MAIL_MENU || currentState == PACKAGE_MENU)
			{
				currentState = USER_MENU;
			}
			else if(currentState == USER_MENU)
			{
				 currentState = MAIN_MENU;
			}
			break;
		case 'A': case 'B': case 'C':
		    currentUser = key - 'A' + 1;
		    currentState = USER_MENU;
		    break;
		case '1':  // Navigate inside user sub menus
			///if(currentState != RETRIEVE_MAIL && currentState != RETRIEVE_PACKAGE)
			if (currentState == USER_MENU) currentState = MAIL_MENU;
			else if (currentState == MAIL_MENU) currentState = CHECK_MAIL;
			else if (currentState == PACKAGE_MENU) currentState = CHECK_PACKAGE;
			break;
		case '2':
			//if(currentState != RETRIEVE_MAIL && currentState != RETRIEVE_PACKAGE)
			if (currentState == USER_MENU) currentState = PACKAGE_MENU;
			else if (currentState == MAIL_MENU) currentState = RETRIEVE_MAIL;
			else if (currentState == PACKAGE_MENU) currentState = RETRIEVE_PACKAGE;
			break;
		default:
			break;
	}
	DisplayMenu(currentState,key);
}

//Generate password
/*Description:  This function generates the randon 4digit(0-9) numerical password
Author: 		Siddhant Mahindrkar
Date: 			15/04/2025
Inputs: 		none
Output: 		pointer to the array that must store the generated password
*/
void generate_password(uchar *password)
{
	srand(HAL_GetTick());
	for (int i = 0; i < 4; i++) {
	        password[i] = '0' + (rand() % 10);  // Generate random 0-9 digit
	    }
	password[4] = '\0';
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
