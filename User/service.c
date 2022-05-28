/**
 * Copyright (c) 2019-2022 ELLIPAL.
 *
 * @license (MIT license)
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:The above copyright
 * notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define NUM_DIR_LEFT 1
#define NUM_DIR_RIGHT 2
/* Private constants ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t ts_idle, ts_bat_chk, ts_sec, ts_keep;
static uint8_t num_select = 12;
static uint8_t page_select, page_total;
static uint8_t mnem_number, mnem_index, mnem_index_page;
static char mnem_buf[300], *mnem_list[24];
static uint8_t regen_yes;
static uint8_t show_level;
/* External variables --------------------------------------------------------*/
DevStatusEnum g_status, g_status_save;

/* Private function prototypes -----------------------------------------------*/
static void ShowChargeDynamic(void);
static void UserButtonHandle(uint8_t button, ButtonStatusEnum status);
static void MnemonicsNumberSelect(uint8_t dir);
static void MnemonicsCovt(char const *pmnem);
static void CommandServ(int argc, char *argv[]);
static void CommandOled(int argc, char *argv[]);

/* External functions --------------------------------------------------------*/
/**
 * @brief device service init
 *
 */
void ServiceInit(void) {
	if (IS_USB_IN()) {
		g_bat_read_level = 0;
	} else {
		g_bat_read_level = 100;
	}

	ButtonCallBackReg(UserButtonHandle);

	CMD_ENT_DEF(serv, CommandServ);
	Cmd_AddEntrance(CMD_ENT(serv));

	CMD_ENT_DEF(oled, CommandOled);
	Cmd_AddEntrance(CMD_ENT(oled));

	ssd1306_Init();
	OLED_Init();

	g_status = dev_pwr_on;
	ShowPowerOnoffPage();
	TS_DELAY(1500);

	SystemReadBatLevel();

	DBG_LOG("services init");
}

/**
 * @brief service polling
 *
 */
void ServicePolling(void) {
	if (g_status != dev_pwr_off) {
		if (TS_IS_OVER(ts_bat_chk, 3000)) {
			TS_INIT(ts_bat_chk);
			SystemReadBatLevel();
		}
		if (g_status != dev_charge && g_bat_adc_volt < 3400) {
			DBG_LOG("Battery empty.");

			ShowPowerOnoffPage();
			TS_DELAY(1000);

			OLED_ClearScreen();
			OLED_Off();

			ssd1306_SetDisplayOn(0);

			g_status = dev_pwr_off;
		}
	}

	if (g_status != dev_charge) {
		if (IS_USB_IN()) {
			SystemReadBatLevel();
			OLED_Init();
			ShowChargePage(g_bat_read_level, g_bat_read_level);
			TS_DELAY(100);
			show_level = 0;
			g_status = dev_charge;
			DBG_LOG("Charge connected.");
		}
	} else {
		ShowChargeDynamic();
		if (!IS_USB_IN()) {
			g_status = dev_pwr_on;
			ShowPowerOnoffPage();
			DBG_LOG("Charge disconnected.");
		}
	}

	// show shut down countdown senconds
	if (g_status == dev_pwr_off_count_down) {
		if (TS_IS_OVER(ts_sec, 1000)) {
			TS_INIT(ts_sec);
			ShowNoOperationPage(60 - TS_COUNT(ts_idle) / 1000);
		}
	}

	// automatic power off
	if (g_status != dev_pwr_off && g_status != dev_charge) {
		if (TS_IS_OVER(ts_idle, 60000)) {
			TS_INIT(ts_idle);

			DBG_LOG("timout power off");
			ShowPowerOnoffPage();
			TS_DELAY(2000);

			OLED_ClearScreen();
			OLED_Off();

			ssd1306_SetDisplayOn(0);

			g_status = dev_pwr_off;
		} else if (TS_IS_OVER(ts_idle, 50000)) {
			if (g_status != dev_pwr_off_count_down) {
				TS_INIT(ts_sec);
				g_status_save = g_status;
				ShowNoOperationPage(10);
				g_status = dev_pwr_off_count_down;
			}
		}
	} else {
		TS_INIT(ts_idle);
	}

	if (g_status == dev_keep) {
		if (TS_IS_OVER(ts_keep, 3000)) {
			TS_INIT(ts_keep);
			ShowImportPage();
			g_status = dev_import;
		}
	} else {
		TS_INIT(ts_keep);
	}

	if (g_status == dev_pwr_on) {
		TS_DELAY(1500);

		ShowGenerateNoticePage();
		g_status = dev_gen_notice;
	}

	// cpu power saving
	if (g_status == dev_pwr_off && !IsButtonPress() && !IS_USB_IN()) {
		DBG_LOG("cpu enter standby mode");
		DeviceSleep();
	}
}

/**
 * @brief Button Trigger handle
 *
 * @param button button index
 * @param status button new status
 */
void UserButtonHandle(uint8_t button, ButtonStatusEnum status) {
	DBG_LOG("button:%u, status:%u", button, status);

  	TS_INIT(ts_idle);

	if (g_status == dev_pwr_off && status == button_press) {
		DBG_LOG("key startup");
		OLED_Init();
		ShowPowerOnoffPage();
		g_status = dev_pwr_on;
		return;
	}

	if (g_status == dev_pwr_off_count_down && status == button_press) {
		switch (g_status_save) {
		case dev_gen_notice:
			ShowGenerateNoticePage();
			g_status = dev_gen_notice;
			break;
		case dev_gen_select:
			ShowNumberSelectPage(num_select);
			g_status = dev_gen_select;
			break;
		case dev_show_mnem:
			ShowMnemoicsPage(page_select, page_total, mnem_index,
					mnem_index_page, &mnem_list[mnem_index - 1]);
			g_status = dev_show_mnem;
			break;
		case dev_regen:
			ShowReGenerateNoticePage(regen_yes);
			g_status = dev_regen;
			break;
		case dev_keep:
			ShowKeepPage();
			g_status = dev_keep;
		case dev_import:
			ShowImportPage();
			g_status = dev_import;
			break;
		default:
			break;
		}
		return;
	}

	if (button == BUTTON_OK && status == button_press) {
		if (g_status == dev_gen_notice) {
			num_select = 12;
			ShowNumberSelectPage(num_select);
			g_status = dev_gen_select;
			return;
		}
		if (g_status == dev_gen_select) {
			uint16_t strength;
			char const *p;

			strength = num_select * 4 / 3 * 8;
			mnemonic_generate(strength);

			ShowGeneratedPage();
			TS_DELAY(3000);
			p = mnemonic_generate(strength);
			MnemonicsCovt(p);

			page_select = 1;
			mnem_index = 1;
			mnem_index_page = mnem_number;
			page_total = mnem_number / 4;
			if (mnem_number % 4) {
				page_total += 1;
			}
			ShowMnemoicsPage(page_select, page_total, mnem_index,
					mnem_index_page, &mnem_list[mnem_index - 1]);
			g_status = dev_show_mnem;
			return;
		}
		if (g_status == dev_show_mnem) {
			regen_yes = 1;
			ShowReGenerateNoticePage(regen_yes);
			g_status = dev_regen;
			return;
		}
		if (g_status == dev_regen) {
			if (regen_yes == 1) {
				ShowGenerateNoticePage();
				g_status = dev_gen_notice;
			} else {
				ShowMnemoicsPage(page_select, page_total, mnem_index,
						mnem_index_page, &mnem_list[mnem_index - 1]);
				g_status = dev_show_mnem;
			}
			return;
		}
		if (g_status == dev_keep || g_status == dev_import) {
			ShowGenerateNoticePage();
			g_status = dev_gen_notice;
			return;
		}
	}

	if (status == button_press && button != BUTTON_OK) {
		if (g_status == dev_gen_select) {
			if (button == BUTTON_LEFT) {
				MnemonicsNumberSelect(NUM_DIR_LEFT);
			}
			if (button == BUTTON_RIGHT) {
				MnemonicsNumberSelect(NUM_DIR_RIGHT);
			}
			ShowNumberSelectPage(num_select);
			return;
		}

		if (g_status == dev_regen) {
			if (button == BUTTON_LEFT) {
				regen_yes = 0;
			}
			if (button == BUTTON_RIGHT) {
				regen_yes = 1;
			}
			ShowReGenerateNoticePage(regen_yes);
			return;
		}
		if ((g_status == dev_import || g_status == dev_keep)
				&& button == BUTTON_LEFT) {
			ShowMnemoicsPage(page_select, page_total, mnem_index,
					mnem_index_page, &mnem_list[mnem_index - 1]);
			g_status = dev_show_mnem;
			return;
		}
		if (g_status == dev_keep && button == BUTTON_RIGHT) {
			ShowImportPage();
			g_status = dev_import;
		}

		if (g_status == dev_show_mnem) {
			if (button == BUTTON_LEFT) {
				if (page_select > 1) {
					page_select--;
					mnem_index = 1 + ((page_select - 1) * 4);
					mnem_index_page = mnem_number - mnem_index + 1;
					ShowMnemoicsPage(page_select, page_total,
							mnem_index, mnem_index_page,
							&mnem_list[mnem_index - 1]);
				}
			}

			if (button == BUTTON_RIGHT) {
				if (page_select < page_total) {
					page_select++;
					mnem_index = 1 + ((page_select - 1) * 4);
					mnem_index_page = mnem_number - mnem_index + 1;
					ShowMnemoicsPage(page_select, page_total,
							mnem_index, mnem_index_page,
							&mnem_list[mnem_index - 1]);
				} else {
					ShowKeepPage();
					g_status = dev_keep;
				}
			}
			return;
		}
	}

	if (button == BUTTON_OK && status == button_longpress) {
		if (g_status != dev_pwr_off && g_status != dev_charge) {
			DBG_LOG("key press trigger shutdown");

			ShowPowerOnoffPage();
			TS_DELAY(2000);

			OLED_ClearScreen();
			OLED_Off();

			ssd1306_SetDisplayOn(0);

			g_status = dev_pwr_off;
			while (IsButtonPress()) {
				continue;
			}
		}
	}
}

/* Private functions ---------------------------------------------------------*/

/**
* @brief Show dynamic effects when charging status
*
*/
static void ShowChargeDynamic(void) {
	static uint32_t ts_cycle, ts_step;

	ts_step = 1500 / g_bat_read_level;

	if (TS_IS_OVER(ts_cycle, ts_step)) {
		TS_INIT(ts_cycle);
		if (show_level < g_bat_read_level) {
			ShowChargePage(show_level, g_bat_read_level);
			show_level++;
		} else {
			ShowChargePage(show_level, g_bat_read_level);
			TS_DELAY(200);
			if (show_level < 100) {
				show_level = 0;
			}
		}
	}
}

/**
* @brief Button press select mnemonics words number
*
* @param dir 1 left, 2 right
*/
static void MnemonicsNumberSelect(uint8_t dir) {
	if (dir == 1) {
		switch (num_select) {
		case 15:
			num_select = 12;
			break;
		case 18:
			num_select = 15;
			break;
		case 21:
			num_select = 18;
			break;
		case 24:
			num_select = 21;
			break;
		default:
			num_select = 12;
			break;
		}
	} else {
		switch (num_select) {
		case 21:
			num_select = 24;
			break;
		case 18:
			num_select = 21;
			break;
		case 15:
			num_select = 18;
			break;
		case 12:
			num_select = 15;
			break;
		default:
			num_select = 24;
			break;
		}
	}
}

/**
* @brief Covert mmemonics format
*
* @param pmnem mmemonics string point
*/
static void MnemonicsCovt(char const *pmnem) {
	char *p;
	int i = 0;

	strcpy(mnem_buf, pmnem);

	p = mnem_buf;
	mnem_list[i++] = &mnem_buf[0];
	while (*p && i < 24) {
		if (*p == ' ') {
			*p = 0;
			p++;
			mnem_list[i++] = p;
		}
		p++;
	}
	mnem_number = i;
}

/**
* @brief service test command function
*
*/
static void CommandServ(int argc, char *argv[]) {
	argc--;
	argv++;

	if (ARGV_EQUAL("mngen")) {
		char *list;
		list = (char*) mnemonic_generate(atoi(argv[1]));
		DBG_LOG("mnemonic_generate():%s", list);
	}

	if (ARGV_EQUAL("batchk")) {
		uint8_t level;

		level = SystemReadBatLevel();

		DBG_LOG("Battery volt:%umV, level:%u", g_bat_adc_volt, level);
				DBG_LOG("USB connect status:%u, charge status:%u", IS_USB_IN(), IS_CHRG());
	}

	if (ARGV_EQUAL("sleep")) {
		DBG_LOG("device sleep.");
		DeviceSleep();
	}

	if (ARGV_EQUAL("status")) {
		DBG_LOG("g_status%d, g_status_save%d.", g_status, g_status_save);
	}
}

/**
 * @brief oled test command function
 *
 */
static void CommandOled(int argc, char *argv[]) {
	argc--;
	argv++;

	if (ARGV_EQUAL("colorturn")) {
		OLED_ColorTurn(atoi(argv[1]));
		DBG_LOG("OLED_ColorTurn()");
	}
	if (ARGV_EQUAL("disturn")) {
		OLED_DisplayTurn(atoi(argv[1]));
		DBG_LOG("OLED_DisplayTurn()");
	}
	if (ARGV_EQUAL("display")) {
		if (atoi(argv[1])) {
			OLED_DisPlay_On();
			DBG_LOG("OLED_DisPlay_On()");
		} else {
			OLED_DisPlay_Off();
			DBG_LOG("OLED_DisPlay_Off()");
		}
	}
	if (ARGV_EQUAL("refresh")) {
		OLED_Refresh();
		DBG_LOG("OLED_Refresh()");
	}
	if (ARGV_EQUAL("clear")) {
		OLED_ClearGram();
		OLED_ClearScreen();
		DBG_LOG("OLED_Clear()");
	}
	if (ARGV_EQUAL("contract")) {
		OLED_Contract((atoi(argv[1])));
		DBG_LOG("OLED_Contract()");
	}
	if (ARGV_EQUAL("draw")) {
		argc--;
		argv++;

		if (ARGV_EQUAL("point")) {
			OLED_DrawPoint(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]));
			DBG_LOG("OLED_DrawPoint()");
		}
		if (ARGV_EQUAL("cycle")) {
			OLED_DrawCircle(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]));
			DBG_LOG("OLED_DrawCircle()");
		}
		if (ARGV_EQUAL("line")) {
			OLED_DrawLine(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]), atoi(argv[4]),
					atoi(argv[5]));
			DBG_LOG("OLED_DrawPoint()");
		}
	}
	if (ARGV_EQUAL("show")) {
		argc--;
		argv++;

		if (ARGV_EQUAL("char")) {
			OLED_ShowChar(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]), atoi(argv[4]),
					atoi(argv[5]));
			DBG_LOG("OLED_ShowChar()");
		}
		if (ARGV_EQUAL("num")) {
			OLED_ShowNum(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]), atoi(argv[4]),
					atoi(argv[5]), atoi(argv[6]));
			DBG_LOG("OLED_ShowNum()");
		}
		if (ARGV_EQUAL("string")) {
			OLED_ShowString(atoi(argv[1]), atoi(argv[2]),
					(uint8_t*) argv[3], atoi(argv[4]),
					atoi(argv[5]));
			DBG_LOG("OLED_ShowString()");
		}
		if (ARGV_EQUAL("picture")) {
			uint8_t *pimage = NULL, xsize, ysize;

			argc--;
			argv++;

			if (ARGV_EQUAL("logo")) {
				pimage = (uint8_t*) &image_main_logo[0][0];
				xsize = image_main_logo_size[0];
				ysize = image_main_logo_size[1];
			}
			if (ARGV_EQUAL("logo2")) {
				pimage = (uint8_t*) &image_main_logo2[0][0];
				xsize = image_main_logo2_size[0];
				ysize = image_main_logo2_size[1];
			}
			if (ARGV_EQUAL("leftarrow")) {
				pimage = (uint8_t*) &image_left_arrow[0][0];
				xsize = image_left_arrow_size[0];
				ysize = image_left_arrow_size[1];
			}
			if (ARGV_EQUAL("rightarrow")) {
				pimage =
						(uint8_t*) &image_right_arrow[0][0];
				xsize = image_right_arrow_size[0];
				ysize = image_right_arrow_size[1];
			}
			if (ARGV_EQUAL("uparrow")) {
				pimage = (uint8_t*) &image_up_arrow[0][0];
				xsize = image_up_arrow_size[0];
				ysize = image_up_arrow_size[1];
			}
			if (ARGV_EQUAL("chargebat")) {
				pimage = (uint8_t*) &image_charge_bat[0][0];
				xsize = image_charge_bat_size[0];
				ysize = image_charge_bat_size[1];
			}
			if (ARGV_EQUAL("mainbat")) {
				pimage = (uint8_t*) &image_main_bat[0][0];
				xsize = image_main_bat_size[0];
				ysize = image_main_bat_size[1];
			}
			if (ARGV_EQUAL("ok")) {
				pimage = (uint8_t*) &image_ok[0][0];
				xsize = image_ok_size[0];
				ysize = image_ok_size[1];
			}
			if (ARGV_EQUAL("button")) {
				pimage = (uint8_t*) &image_button[0][0];
				xsize = image_button_size[0];
				ysize = image_button_size[1];
			}
			if (ARGV_EQUAL("buttondark")) {
				pimage =
						(uint8_t*) &image_button_dark[0][0];
				xsize = image_button_size[0];
				ysize = image_button_size[1];
			}
			if (ARGV_EQUAL("notice")) {
				pimage = (uint8_t*) &image_notice[0][0];
				xsize = image_notice_size[0];
				ysize = image_notice_size[1];
			}
			if (pimage) {
				OLED_ShowPicture(atoi(argv[1]),
						atoi(argv[2]), xsize, ysize, pimage,
						atoi(argv[3]));
				DBG_LOG("OLED_ShowPicture()");
			}
		}
	}

	if (ARGV_EQUAL("menu")) {
		argc--;
		argv++;

		if (ARGV_EQUAL("onoff")) {
			ShowPowerOnoffPage();
			DBG_LOG("ShowPowerOnoffPage()");
		}
		if (ARGV_EQUAL("generatenotice")) {
			ShowGenerateNoticePage();
			DBG_LOG("ShowGenerateNoticePage()");
		}
		if (ARGV_EQUAL("numberselect")) {
			ShowNumberSelectPage(atoi(argv[1]));
			DBG_LOG("ShowNumberSelectPage()");
		}
		if (ARGV_EQUAL("generated")) {
			ShowGeneratedPage();
			DBG_LOG("ShowGeneratedPage()");
		}
		if (ARGV_EQUAL("completed")) {
			ShowKeepPage();
			DBG_LOG("ShowKeepPage()");
		}
		if (ARGV_EQUAL("regenerate")) {
			ShowReGenerateNoticePage(atoi(argv[1]));
			DBG_LOG("ShowReGenerateNoticePage()");
		}
		if (ARGV_EQUAL("nooperation")) {
			ShowNoOperationPage(atoi(argv[1]));
			DBG_LOG("ShowNoOperationPage()");
		}
		if (ARGV_EQUAL("battery")) {
			ShowBattery(atoi(argv[1]));
			DBG_LOG("ShowBattery()");
		}
		if (ARGV_EQUAL("charge")) {
			ShowChargePage(atoi(argv[1]), atoi(argv[2]));
			DBG_LOG("ShowChargePage()");
		}
		if (ARGV_EQUAL("mnemonic")) {
			ShowMnemoicsPage(atoi(argv[1]), atoi(argv[2]),
					atoi(argv[3]), atoi(argv[4]), &argv[5]);
			DBG_LOG("ShowChargePage()");
		}
	}
}
