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
#include "usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t cmd_rxbuf[COMMAND_BUF_MAX], printf_buf[COMMAND_PRINT_BUF_MAX];
static uint16_t cmd_rxcnt = 0;
static CmdEntrance_t *firstEnt = NULL;
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void CommandSystem(int argc, char *argv[]);

/* External functions --------------------------------------------------------*/

/**
 * @brief command line init.
 *
 */
void CommandInit(void) {
	CMD_ENT_DEF(system, CommandSystem);
	Cmd_AddEntrance(CMD_ENT(system));
	DBG_LOG("Command Init.");
}

/**
 * @brief command receive new byte
 *
 * @param rd receive byte
 */
void CommandNewRec(uint8_t rd) {
	cmd_rxbuf[cmd_rxcnt++] = rd;
	if (cmd_rxcnt >= COMMAND_BUF_MAX) {
		cmd_rxcnt = 0;
	}
}

/**
 * command receive data polling
 */
void CommandReceivePolling(void) {
	char *p = NULL;

	if (cmd_rxcnt > 0
			&& (cmd_rxbuf[cmd_rxcnt - 1] == '\n'
					|| cmd_rxbuf[cmd_rxcnt - 1] == '\r')) {
		DBG_SEND(cmd_rxbuf, cmd_rxcnt - 1); DBG_SEND("\r\n", 2);

		cmd_rxbuf[cmd_rxcnt - 1] = '\0';
		p = (char*) &cmd_rxbuf[0];

		while (!isgraph(*p)) {
			p++;
		}
		Cmd_Handle(p);
		cmd_rxcnt = 0;
	}
}

/**
 * @brief command send data to uart
 *
 * @param data data array point
 * @param len data len
 */
void CommandSend(uint8_t *data, uint16_t len) {
#if HARDWARE_ELLIPAL == 1
	HAL_UART_Transmit(&huart1, data, len, 1000);
#endif

#if HARDWARE_NUCLEO == 1
	HAL_UART_Transmit(&huart3, data, len, 1000);
#endif
}

/**
 * @brief command format print function
 *
 */
void CommandPrintf(char *format, ...) {
	va_list args;

	va_start(args, format);
	vsnprintf((char*) printf_buf, COMMAND_PRINT_BUF_MAX, format, args);
	va_end(args);
	CommandSend(printf_buf, strlen((char*) printf_buf));
}

/**
 * @brief virtual command execute
 *
 * @param cmd command string
 */
void CommandVirtual(char *cmd) {
	strcpy((char*) cmd_rxbuf, cmd);
	Cmd_Handle((char*) cmd_rxbuf);
}

/**
 * @brief add a command entrance
 *
 * @param pEnt command entrance string
 * @return uint8_t add sucess return 1
 */
uint8_t Cmd_AddEntrance(CmdEntrance_t *pEnt) {
	uint8_t r = 0;
	CmdEntrance_t *p = NULL;

	if (pEnt != NULL && pEnt->cmd != NULL) {
		if (firstEnt == NULL) {
			firstEnt = pEnt;
		} else {
			p = firstEnt;
			while (p->next != NULL && p->next != pEnt) {
				p = p->next;
			}
			p->next = pEnt;
		}
		r = 1;
	}
	return r;
}

/**
 * @brief command execute
 *
 * @param cmd command string
 */
void Cmd_Handle(char *cmd) {
	uint8_t argc = 0;
	char *argv[CMD_ARGC_MAX + 1], *p = NULL;
	CmdEntrance_t *pEnt;

	pEnt = firstEnt;
	for (argc = 0; argc < CMD_ARGC_MAX; argc++) {
		argv[argc] = NULL;
	}
	while (*cmd && !isgraph(*cmd)) {
		cmd++;
	}
	argc = 1;
	argv[0] = cmd;
	p = cmd;
	while (*p && argc < CMD_ARGC_MAX) {
		while (*p && isgraph(*p)) {
			p++;
		}
		while (*p && !isgraph(*p)) {
			*p = 0;
			p++;
		}
		if (*p) {
			argv[argc] = p;
			argc++;
		}
	}
	argv[argc] = NULL;
	while (pEnt != NULL) {
		if (argc > 0 && strcmp(cmd, pEnt->cmd) == 0) {
			pEnt->pCB(argc, argv);
			break;
		}
		pEnt = pEnt->next;
	}
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief system command function
 *
 */
static void CommandSystem(int argc, char *argv[]) {
	argv++;
	argc--;

	if (ARGV_EQUAL("reset")) {
		DBG_LOG("System will Reset.");
		HAL_NVIC_SystemReset();
	} else if (ARGV_EQUAL("version")) {
		DBG_LOG("Firmware version:%s", VERSION);
	}
}
