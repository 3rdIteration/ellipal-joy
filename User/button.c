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
static uint8_t button_io_read[BUTTON_NUM_MAX], button_io[BUTTON_NUM_MAX];
static uint32_t ts_button[BUTTON_NUM_MAX];
static ButtonHandleCallBackDef ButtonCB = NULL;

/* External variables --------------------------------------------------------*/
ButtonStatusEnum button_status[BUTTON_NUM_MAX];

/* Private function prototypes -----------------------------------------------*/

/* External functions --------------------------------------------------------*/

/**
 * @brief Button scanning polling
 *
 */
void ButtonScanPolling(void) {
	uint8_t i, trigger = 0;

	button_io_read[0] = IO_READ(KEY1);
	button_io_read[1] = IO_READ(KEY2);
	button_io_read[2] = IO_READ(KEY3);

#if HARDWARE_NUCLEO == 1
	IO_WRITE(LD1,(button_io_read[0]==0));
	IO_WRITE(LD2,(button_io_read[1]==0));
	IO_WRITE(LD3,(button_io_read[2]==0));
#endif

	for (i = 0; i < BUTTON_NUM_MAX; i++) {
		if (button_io[i] != button_io_read[i]) {
			TS_INIT(ts_button[i]);
			button_io[i] = button_io_read[i];
		} else if (TS_IS_OVER(ts_button[i], 20)) {
			if (button_io[i] == 0) {
				if (button_status[i] == button_none) {
					button_status[i] = button_press;
					trigger = 1;
				}
				if (TS_IS_OVER(ts_button[i], 3000)
						&& button_status[i] == button_press) {
					button_status[i] = button_longpress;
					trigger = 1;
				}
			} else {
				if (button_status[i] == button_press) {
					button_status[i] = button_press_free;
					trigger = 1;
				}
				if (button_status[i] == button_longpress) {
					button_status[i] = button_longpress_free;
					trigger = 1;
				}
			}
		}
		if (trigger) {
			if (ButtonCB != NULL) {
				ButtonCB(i + 1, button_status[i]);
			}
			trigger = 0;
			if (button_status[i] == button_press_free
					|| button_status[i] == button_longpress_free) {
				button_status[i] = button_none;
			}
		}
	}
}

/**
 * @brief Query is any key press
 *
 * @return uint8_t any key press return 1
 */
uint8_t IsButtonPress(void) {
	uint8_t ret = 0;

	if (IS_BUTTON_LEFT() || IS_BUTTON_RIGHT() || IS_BUTTON_OK()) {
		ret = 1;
	} else {
		button_status[0] = button_none;
		button_status[1] = button_none;
		button_status[2] = button_none;
	}

	return ret;
}

/**
 * @brief Register key callback function
 *
 * @param cb callback function point
 */
void ButtonCallBackReg(ButtonHandleCallBackDef cb) {
	// Register
	ButtonCB = cb;
}

/* Private functions ---------------------------------------------------------*/
