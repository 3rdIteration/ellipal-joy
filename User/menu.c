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

/* Private constants ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* External functions --------------------------------------------------------*/
/**
 * @brief Show Power on and Power off page
 *
 */
void ShowPowerOnoffPage(void) {
	OLED_ClearGram();
	OLED_ShowPicture(0, 0, image_main_logo_size[0], image_main_logo_size[1],
			(uint8_t*) image_main_logo, 1);
	OLED_Refresh();


    ssd1306_Fill(Black);
    ssd1306_DrawBitmap(31, 0, (uint8_t*) image_main_logo3, image_main_logo3_size[0], image_main_logo3_size[1], White);

    //ssd1306_WriteString("Crypto", Font_16x26, White);
    //ssd1306_SetCursor(40, 30);
    //ssd1306_WriteString("Guide", Font_16x26, White);
    ssd1306_UpdateScreen();
}

/**
 * @brief Show generation notice page
 *
 */
void ShowGenerateNoticePage(void) {
	OLED_ClearGram();
	ShowUpArrow();
	ShowBattery(g_bat_read_level);

	OLED_ShowString(10, 18, "Press ", 16, 1);

	OLED_ShowPicture(60, 18, image_ok_size[0], image_ok_size[1],
			(uint8_t*) image_ok, 1);
	OLED_ShowString(8, 40, "t", 16, 1);
	OLED_ShowString(15, 40, "o", 16, 1);
	OLED_ShowString(28, 40, "generat", 16, 1);
	OLED_ShowString(83, 40, "e", 16, 1);

	OLED_ShowString(12, 56, "m", 16, 1);
	OLED_ShowString(21, 56, "nemoni", 16, 1);
	OLED_ShowString(67, 56, "cs", 16, 1);

	OLED_ShowPicture(25, 75, image_main_logo2_size[0], image_main_logo2_size[1],
			(uint8_t*) image_main_logo2, 0);

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Press K3", Font_11x18, White);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("To Generate", Font_11x18, White);
    ssd1306_UpdateScreen();

}

/**
 * @brief Show number of mnemonic words select page
 * @param num select number
 *
 */
void ShowNumberSelectPage(uint8_t num) {
	uint8_t num_lr[4];

	OLED_ClearGram();

	ShowBattery(g_bat_read_level);

	OLED_ShowString(16, 16, "Number of", 12, 1);
	OLED_ShowString(7, 32, "m", 12, 1);
	OLED_ShowString(14, 32, "nemonic", 12, 1);
	OLED_ShowString(61, 32, "w", 12, 1);
	OLED_ShowString(66, 32, "or", 12, 1);
	OLED_ShowString(77, 32, "ds", 12, 1);

	OLED_DrawCircle(48, 68, 18);
	OLED_ShowNum(40, 60, num, 2, 16, 1);

	if (num == 12) {
		num_lr[0] = 0;
		num_lr[1] = 0;
		num_lr[2] = 15;
		num_lr[3] = 18;
	}
	if (num == 15) {
		num_lr[0] = 0;
		num_lr[1] = 12;
		num_lr[2] = 18;
		num_lr[3] = 21;
	}
	if (num == 18) {
		num_lr[0] = 12;
		num_lr[1] = 15;
		num_lr[2] = 21;
		num_lr[3] = 24;
	}
	if (num == 21) {
		num_lr[0] = 15;
		num_lr[1] = 18;
		num_lr[2] = 24;
		num_lr[3] = 0;
	}
	if (num == 24) {
		num_lr[0] = 18;
		num_lr[1] = 21;
		num_lr[2] = 0;
		num_lr[3] = 0;
	}
	if (num_lr[0]) {
		OLED_ShowNum(0, 62, num_lr[0], 2, 12, 1);
	}
	if (num_lr[1]) {
		OLED_ShowNum(15, 62, num_lr[1], 2, 12, 1);
	}
	if (num_lr[2]) {
		OLED_ShowNum(70, 62, num_lr[2], 2, 12, 1);
	}
	if (num_lr[3]) {
		OLED_ShowNum(84, 62, num_lr[3], 2, 12, 1);
	}

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Mnemonic", Font_11x18, White);
	if (num == 12) {
		ssd1306_SetCursor(2, 20);
		ssd1306_WriteString("Length: 12", Font_11x18, White);
		ssd1306_SetCursor(2, 45);
		ssd1306_WriteString("     | K2+", Font_11x18, White);
	}
	if (num == 15) {
		ssd1306_SetCursor(2, 20);
		ssd1306_WriteString("Length: 15", Font_11x18, White);
		ssd1306_SetCursor(2, 45);
		ssd1306_WriteString(" K1- | K2+", Font_11x18, White);
	}
	if (num == 18) {
		ssd1306_SetCursor(2, 20);
		ssd1306_WriteString("Length: 18", Font_11x18, White);
		ssd1306_SetCursor(2, 45);
		ssd1306_WriteString(" K1- | K2+", Font_11x18, White);
	}
	if (num == 21) {
		ssd1306_SetCursor(2, 20);
		ssd1306_WriteString("Length: 21", Font_11x18, White);
		ssd1306_SetCursor(2, 45);
		ssd1306_WriteString(" K1- | K2+", Font_11x18, White);
	}
	if (num == 24) {
		ssd1306_SetCursor(2, 20);
		ssd1306_WriteString("Length: 24", Font_11x18, White);
		ssd1306_SetCursor(2, 45);
		ssd1306_WriteString(" K1- | ", Font_11x18, White);
	}
    ssd1306_UpdateScreen();
}

/**
 * @brief Show Generated notice page
 *
 */
void ShowGeneratedPage(void) {
	OLED_ClearGram();
	ShowBattery(g_bat_read_level);

	OLED_ShowString(10, 45, "Generating...", 12, 1);

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("Generating", Font_11x18, White);
    ssd1306_UpdateScreen();

}

/**
 * @brief Show mnmoics words page
 *
 * @param mnem mnmoics string array
 * @param index mnmoics start index
 * @param mnem_num in current page need show number of mnmoics
 * @param page_num current page number
 * @param page_total total pages number
 */
void ShowMnemoicsPage(uint8_t page_num, uint8_t page_total, uint8_t index,
		uint8_t mnem_num, char *mnem[]) {
	uint8_t buf[16], i, y, y2;

	OLED_ClearGram();
	ssd1306_Fill(Black);

	ShowBattery(g_bat_read_level);
	ShowLeftRightArrow();

	sprintf((char*) buf, "%u/%u", page_num, page_total);
	OLED_ShowString(39, 4, buf, 12, 1);

	y = 16;
	y2 = 12;
	if (mnem_num > 4) {
		mnem_num = 4;
	}
	for (i = 0; i < mnem_num; i++) {
		OLED_ShowNum(6, y + 4, index + i, 2, 12, 1);
		sprintf((char*) buf, ".%s", mnem[i]);
		OLED_ShowString(18, y, buf, 16, 1);
		y += 20;

		ssd1306_SetCursor(10, y2);
		ssd1306_WriteNum(index + i, 2, Font_7x10, White);
	    ssd1306_SetCursor(24, y2);
	    ssd1306_WriteString(buf, Font_7x10, White);
	    y2 += 12;
	}

	ssd1306_Line(105, 0, 105, 64, White);
	ssd1306_SetCursor(113, 12);
	ssd1306_WriteString("K1", Font_7x10, White);
	ssd1306_SetCursor(115, 22);
	ssd1306_WriteString(">", Font_7x10, White);
	ssd1306_SetCursor(113, 42);
	ssd1306_WriteString("K2", Font_7x10, White);
	ssd1306_SetCursor(115, 52);
	ssd1306_WriteString("<", Font_7x10, White);

	ssd1306_UpdateScreen();
	OLED_Refresh();


}

/**
 * @brief Show Regenerated notice page
 * @param is_yes 1 show yes button selceted, 0 show no button selceted.
 *
 */
void ShowReGenerateNoticePage(uint8_t is_yes) {
	uint8_t *p_left, *p_right, m_left, m_right;

	OLED_ClearGram();

	ShowBattery(g_bat_read_level);

	OLED_ShowPicture(4, 20, image_notice_size[0], image_notice_size[1],
			(uint8_t*) image_notice, 1);

	OLED_ShowString(16, 24, "Regenerate", 12, 1);
	OLED_ShowString(16, 36, "mnemonics?", 12, 1);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 00);
    ssd1306_WriteString("Regenerate?", Font_11x18, White);


	if (is_yes) {
		p_left = (uint8_t*) image_button;
		p_right = (uint8_t*) image_button_dark;
		m_left = 1;
		m_right = 0;

	    ssd1306_SetCursor(2, 20);
	    ssd1306_WriteString("K3 Yes", Font_11x18, White);
	    ssd1306_SetCursor(2, 40);
	    ssd1306_WriteString("K1 No", Font_11x18, White);
	} else {
		p_left = (uint8_t*) image_button_dark;
		p_right = (uint8_t*) image_button;
		m_left = 0;
		m_right = 1;

	    ssd1306_SetCursor(2, 20);
	    ssd1306_WriteString("K3 No", Font_11x18, White);
	    ssd1306_SetCursor(2, 40);
	    ssd1306_WriteString("K2 Yes", Font_11x18, White);
	}

	ssd1306_UpdateScreen();

	OLED_ShowPicture(15, 58, image_button_size[0], image_button_size[1],
			(uint8_t*) p_left, 1);

	OLED_ShowPicture(51, 58, image_button_size[0], image_button_size[1],
			(uint8_t*) p_right, 1);

	OLED_ShowString(23, 62, "NO", 8, m_left);
	OLED_ShowString(55, 62, "YES", 8, m_right);


	OLED_Refresh();
}

/**
 * @brief Show no operation page
 * @param sec Shutdown countdown senconds
 *
 */
void ShowNoOperationPage(uint8_t sec) {
	uint8_t buf[16];

	OLED_ClearGram();
	ShowBattery(g_bat_read_level);

	OLED_ShowString(8, 24, "No operation", 12, 1);
	OLED_ShowString(8, 40, "It will shut", 12, 1);
	OLED_ShowString(8, 56, "down after", 12, 1);

	sprintf((char*) buf, "%us", sec);
	OLED_ShowString(36, 72, buf, 16, 1);

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("No operation", Font_11x18, White);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("Shutdown", Font_11x18, White);
    ssd1306_SetCursor(2, 40);
    ssd1306_WriteString("in", Font_11x18, White);
    ssd1306_SetCursor(80, 40);
    ssd1306_WriteString(buf, Font_11x18, White);
    ssd1306_UpdateScreen();

	OLED_Refresh();
}

/**
 * @brief Show "please keep etc. page"
 *
 */
void ShowKeepPage(void) {
	OLED_ClearGram();

	ShowUpArrow();
	ShowLeftRightArrow();
	ShowBattery(g_bat_read_level);

	OLED_ShowString(2, 30, "Congratulations!", 12, 1);
	OLED_ShowString(2, 45, "Please keep the", 12, 1);
	OLED_ShowString(2, 60, "m", 12, 1);
	OLED_ShowString(9, 60, "nemoni", 12, 1);
	OLED_ShowString(44, 60, "c phrase.", 12, 1);

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Congratulations!", Font_11x18, White);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("Keep phrase", Font_11x18, White);
    ssd1306_SetCursor(2, 40);
    ssd1306_WriteString("secure", Font_11x18, White);
    ssd1306_UpdateScreen();
}

/**
 * @brief Show "please import etc. page"
 *
 */
void ShowImportPage(void) {
	OLED_ClearGram();

	ShowUpArrow();
	OLED_ShowPicture(19, 6, image_left_arrow_size[0], image_left_arrow_size[1],
			(uint8_t*) image_left_arrow, 1);
	ShowBattery(g_bat_read_level);

	OLED_ShowString(2, 30, "Please import", 12, 1);
	OLED_ShowString(2, 45, "m", 12, 1);
	OLED_ShowString(9, 45, "nemoni", 12, 1);
	OLED_ShowString(44, 45, "c phrase", 12, 1);
	OLED_ShowString(2, 60, "to your wallet.", 12, 1);

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Import in", Font_11x18, White);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("to your", Font_11x18, White);
    ssd1306_SetCursor(2, 40);
    ssd1306_WriteString("wallet", Font_11x18, White);
    ssd1306_UpdateScreen();
}

/**
 * @brief Show charge page
 *
 * @param grap Percentage of battery power displayed graphically
 * @param num Batter power level percent number,0-100
 */
void ShowChargePage(uint8_t grap, uint8_t num) {
	uint8_t buf[16];
	uint8_t i, n, x, y, l;

	OLED_ClearGram();
	OLED_ShowPicture(25, 35, image_charge_bat_size[0], image_charge_bat_size[1],
			(uint8_t*) image_charge_bat, 1);

	n = grap * 40 / 100;

	for (i = 0; i < n; i++) {
		x = 29 + i;

		if (i == 0 || i == 39) {
			l = 15;
			y = 40;
		} else {
			l = 17;
			y = 39;
		}
		OLED_DrawLine(x, y, x, y + l - 1, 1);
	}

	sprintf((char*) buf, "%u%%", num);
	if (num >= 100) {
		x = 32;
	} else {
		x = 40;
	}
	OLED_ShowString(x, 68, buf, 16, 1);

	OLED_Refresh();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 20);
    ssd1306_WriteString("Charging", Font_11x18, White);
    ssd1306_UpdateScreen();
}

/**
 * @brief Show up direction arrow ICON
 *
 */
void ShowUpArrow(void) {
	OLED_ShowPicture(43, 3, image_up_arrow_size[0], image_up_arrow_size[1],
			(uint8_t*) image_up_arrow, 1);
}

/**
 * @brief Show left and right direction arrow ICON
 *
 */
void ShowLeftRightArrow(void) {
	OLED_ShowPicture(19, 6, image_left_arrow_size[0], image_left_arrow_size[1],
			(uint8_t*) image_left_arrow, 1);

	OLED_ShowPicture(68, 6, image_right_arrow_size[0],
			image_right_arrow_size[1], (uint8_t*) image_right_arrow, 1);
}

/**
 * @brief Show battery level ICON
 *
 * @param perc 0-100
 */
void ShowBattery(uint8_t perc) {
	OLED_ShowPicture(82, 3, image_main_bat_size[0], image_main_bat_size[1],
			(uint8_t*) image_main_bat, 1);

	if (perc > 5) {
		OLED_DrawPoint(84, 5, 1);
		OLED_DrawPoint(84, 6, 1);
	}
	if (perc > 20) {
		OLED_DrawPoint(85, 5, 1);
		OLED_DrawPoint(85, 6, 1);
	}
	if (perc > 40) {
		OLED_DrawPoint(86, 5, 1);
		OLED_DrawPoint(86, 6, 1);
	}
	if (perc > 60) {
		OLED_DrawPoint(87, 5, 1);
		OLED_DrawPoint(87, 6, 1);
	}
	if (perc > 80) {
		OLED_DrawPoint(88, 5, 1);
		OLED_DrawPoint(88, 6, 1);
	}
}

/* Private functions ---------------------------------------------------------*/
