// Copyright 2020-2021 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <os/os.h>
#include <os/mem.h>
#include <os/str.h>
#include "stdio.h"
#include <driver/audio_ring_buff.h>
#include "bk_aud_tras.h"
#include <driver/uart.h>
#include "gpio_driver.h"
#include <driver/timer.h>
#if (CONFIG_CACHE_ENABLE)
#include "cache.h"
#endif
#include "rtc_bk.h"

#define AUD_TRAS "aud_tras"

#define LOGI(...) BK_LOGI(AUD_TRAS, ##__VA_ARGS__)
#define LOGW(...) BK_LOGW(AUD_TRAS, ##__VA_ARGS__)
#define LOGE(...) BK_LOGE(AUD_TRAS, ##__VA_ARGS__)
#define LOGD(...) BK_LOGD(AUD_TRAS, ##__VA_ARGS__)

#define TU_QITEM_COUNT      (40)
#define AUD_TRAS_BUFF_SIZE    (320 * 5)

//#define AUD_TX_DEBUG

#define CONFIG_AUD_TX_COUNT_DEBUG
#ifdef CONFIG_AUD_TX_COUNT_DEBUG
#define AUD_TX_DEBUG_INTERVAL (1000 * 2)
#endif

#ifndef CONFIG_AUD_RING_BUFF_SAFE_INTERVAL
#define CONFIG_AUD_RING_BUFF_SAFE_INTERVAL    4
#endif

typedef struct
{
	beken_thread_t aud_tras_task_hdl;
	beken_queue_t aud_tras_int_msg_que;
	uint8_t *aud_tras_buff_addr;
	RingBufferContext aud_tras_rb;			//save mic data needed to send by aud_tras task
} aud_tras_info_t;

#ifdef CONFIG_AUD_TX_COUNT_DEBUG
typedef struct {
	beken_timer_t timer;
	uint32_t complete_size;
} aud_tx_count_debug_t;

static aud_tx_count_debug_t aud_tx_count = {0};
#endif

static aud_tras_setup_t aud_trs_setup_bk = {0};
static aud_tras_info_t *aud_tras_info = NULL;

#ifdef AUD_TX_DEBUG
static void uart_dump_mic_data(uart_id_t id, uint32_t baud_rate)
{
	uart_config_t config = {0};
	os_memset(&config, 0, sizeof(uart_config_t));
	if (id == 0) {
		gpio_dev_unmap(GPIO_10);
		gpio_dev_map(GPIO_10, GPIO_DEV_UART1_RXD);
		gpio_dev_unmap(GPIO_11);
		gpio_dev_map(GPIO_11, GPIO_DEV_UART1_TXD);
	} else if (id == 2) {
		gpio_dev_unmap(GPIO_40);
		gpio_dev_map(GPIO_40, GPIO_DEV_UART3_RXD);
		gpio_dev_unmap(GPIO_41);
		gpio_dev_map(GPIO_41, GPIO_DEV_UART3_TXD);
	} else {
		gpio_dev_unmap(GPIO_0);
		gpio_dev_map(GPIO_0, GPIO_DEV_UART2_TXD);
		gpio_dev_unmap(GPIO_1);
		gpio_dev_map(GPIO_1, GPIO_DEV_UART2_RXD);
	}

	config.baud_rate = baud_rate;
	config.data_bits = UART_DATA_8_BITS;
	config.parity = UART_PARITY_NONE;
	config.stop_bits = UART_STOP_BITS_1;
	config.flow_ctrl = UART_FLOWCTRL_DISABLE;
	config.src_clk = UART_SCLK_XTAL_26M;

	if (bk_uart_init(id, &config) != BK_OK) {
		LOGE("init uart fail \r\n");
	} else {
		LOGI("init uart ok \r\n");
	}
}
#endif

#ifdef CONFIG_AUD_TX_COUNT_DEBUG
static void aud_tx_lost_count_dump(void *param)
{
	aud_tx_count.complete_size = aud_tx_count.complete_size / 1024 / (AUD_TX_DEBUG_INTERVAL / 1000);

	LOGI("[AUD Tx] %uKB/s \r\n", aud_tx_count.complete_size);
	aud_tx_count.complete_size  = 0;
}
#endif

bk_err_t aud_tras_send_msg(aud_tras_op_t op, void *param)
{
	bk_err_t ret;
	aud_tras_msg_t msg;

	msg.op = op;
	msg.param = param;
	if (aud_tras_info->aud_tras_int_msg_que) {
		ret = rtos_push_to_queue(&aud_tras_info->aud_tras_int_msg_que, &msg, BEKEN_NO_WAIT);
		if (kNoErr != ret) {
			LOGE("aud_tras_send_int_msg fail \r\n");
			return kOverrunErr;
		}

		return ret;
	}
	return kNoResourcesErr;
}

bk_err_t aud_tras_deinit(void)
{
	bk_err_t ret;
	aud_tras_msg_t msg;

	msg.op = AUD_TRAS_EXIT;
	msg.param = NULL;
	if (aud_tras_info->aud_tras_int_msg_que) {
		ret = rtos_push_to_queue_front(&aud_tras_info->aud_tras_int_msg_que, &msg, BEKEN_NO_WAIT);
		if (kNoErr != ret) {
			LOGE("audio send msg: AUD_TRAS_EXIT fail \r\n");
			return kOverrunErr;
		}

		return ret;
	}
	return kNoResourcesErr;
}

static void aud_tras_main(beken_thread_arg_t param_data)
{
	bk_err_t ret = BK_OK;
	uint32_t fill_size = 0;
	aud_tras_setup_t *aud_trs_setup = NULL;
	aud_trs_setup = (aud_tras_setup_t *)param_data;
	uint8_t *aud_temp_data = NULL;
	int tx_size = 0;


#ifdef AUD_TX_DEBUG
	uart_dump_mic_data(1, 2000000);
#endif

#ifdef CONFIG_AUD_TX_COUNT_DEBUG
	if (aud_tx_count.timer.handle != NULL)
	{
		ret = rtos_deinit_timer(&aud_tx_count.timer);
		if (BK_OK != ret)
		{
			LOGE("deinit aud_tx_count time fail\r\n");
			goto aud_tras_exit;
		}
		aud_tx_count.timer.handle = NULL;
	}

	aud_tx_count.complete_size = 0;

	ret = rtos_init_timer(&aud_tx_count.timer, AUD_TX_DEBUG_INTERVAL, aud_tx_lost_count_dump, NULL);
	if (ret != BK_OK) {
		LOGE("rtos_init_timer fail \n");
	}
	ret = rtos_start_timer(&aud_tx_count.timer);
	if (ret != BK_OK) {
		LOGE("rtos_start_timer fail \n");
	}
	LOGI("start audio tx count timer complete \r\n");
#endif

	GLOBAL_INT_DECLARATION();
	aud_temp_data = os_malloc(320);
	if (!aud_temp_data)
	{
		LOGE("malloc aud_temp_data\n");
		goto aud_tras_exit;
	}
	os_memset(aud_temp_data, 0, 320);

	aud_tras_send_msg(AUD_TRAS_TX, NULL);

	aud_tras_msg_t msg;
	while (1) {
		ret = rtos_pop_from_queue(&aud_tras_info->aud_tras_int_msg_que, &msg, BEKEN_WAIT_FOREVER);
		if (kNoErr == ret) {
			switch (msg.op) {
				case AUD_TRAS_IDLE:
					break;

				case AUD_TRAS_EXIT:
					LOGD("goto: AUD_TRAS_EXIT \r\n");
					goto aud_tras_exit;
					break;

				case AUD_TRAS_TX:
					fill_size = ring_buffer_get_fill_size(&aud_tras_info->aud_tras_rb);
					for (int n = 0; n < fill_size/320; n++) {
//						GPIO_UP(6);
#if (CONFIG_CACHE_ENABLE)
						flush_all_dcache();
#endif
						GLOBAL_INT_DISABLE();
						ring_buffer_read(&aud_tras_info->aud_tras_rb, aud_temp_data, 320);
						GLOBAL_INT_RESTORE();
#ifdef AUD_TX_DEBUG
						bk_uart_write_bytes(UART_ID_1, aud_temp_data, 320);
#endif
						tx_size = aud_trs_setup->aud_tras_send_data_cb(aud_temp_data, 320);
						if (tx_size > 0) {
#ifdef CONFIG_AUD_TX_COUNT_DEBUG
							aud_tx_count.complete_size+=tx_size;
#endif
						}
//						GPIO_DOWN(6);
					}

					rtos_delay_milliseconds(5);
					aud_tras_send_msg(AUD_TRAS_TX, NULL);
					break;

				default:
					break;
			}
		}
	}

aud_tras_exit:
	if (aud_temp_data) {
		os_free(aud_temp_data);
		aud_temp_data = NULL;
	}

#ifdef CONFIG_AUD_TX_COUNT_DEBUG
	if (aud_tx_count.timer.handle) {
		ret = rtos_stop_timer(&aud_tx_count.timer);
		if (ret != BK_OK) {
			LOGE("stop aud_tx_count timer fail \n");
		}
		ret = rtos_deinit_timer(&aud_tx_count.timer);
		if (ret != BK_OK) {
			LOGE("deinit aud_tx_count timer fail \n");
		}
		aud_tx_count.timer.handle = NULL;
	}
	aud_tx_count.complete_size = 0;
#endif

	if (aud_tras_info->aud_tras_buff_addr) {
		ring_buffer_clear(&aud_tras_info->aud_tras_rb);
		os_free(aud_tras_info->aud_tras_buff_addr);
		aud_tras_info->aud_tras_buff_addr = NULL;
	}

	/* delete msg queue */
	ret = rtos_deinit_queue(&aud_tras_info->aud_tras_int_msg_que);
	if (ret != kNoErr) {
		LOGE("delete message queue fail \r\n");
	}
	aud_tras_info->aud_tras_int_msg_que = NULL;
	LOGI("delete aud_tras_int_msg_que \r\n");

	os_memset(&aud_trs_setup_bk, 0, sizeof(aud_tras_setup_t));

	/* delete task */
	aud_tras_info->aud_tras_task_hdl = NULL;

	if (aud_tras_info)
	{
		os_free(aud_tras_info);
		aud_tras_info = NULL;
	}

	rtos_delete_thread(NULL);
}

bk_err_t aud_tras_init(aud_tras_setup_t *setup_cfg)
{
	bk_err_t ret = BK_OK;

	aud_tras_info = os_malloc(sizeof(aud_tras_info_t));

	if (aud_tras_info == NULL)
	{
		LOGE("malloc aud_tras_info\n");
		return BK_FAIL;
	}
	os_memset(aud_tras_info, 0, sizeof(aud_tras_info_t));

	aud_tras_info->aud_tras_buff_addr = os_malloc(AUD_TRAS_BUFF_SIZE + CONFIG_AUD_RING_BUFF_SAFE_INTERVAL);
	if (!aud_tras_info->aud_tras_buff_addr) {
		LOGE("malloc aud_tras_buff_addr\n");
		goto out;
	}
	ring_buffer_init(&aud_tras_info->aud_tras_rb, aud_tras_info->aud_tras_buff_addr, AUD_TRAS_BUFF_SIZE + CONFIG_AUD_RING_BUFF_SAFE_INTERVAL, DMA_ID_MAX, RB_DMA_TYPE_NULL);
	LOGD("aud_tras_info->aud_tras_rb: %p \n", &aud_tras_info->aud_tras_rb);

	os_memcpy(&aud_trs_setup_bk, setup_cfg, sizeof(aud_tras_setup_t));

	if ((!aud_tras_info->aud_tras_task_hdl) && (!aud_tras_info->aud_tras_int_msg_que))
	{
		ret = rtos_init_queue(&aud_tras_info->aud_tras_int_msg_que,
							  "aud_tras_int_que",
							  sizeof(aud_tras_msg_t),
							  TU_QITEM_COUNT);
		if (ret != kNoErr)
		{
			LOGE("ceate audio transfer internal message queue fail\n");
			goto out;
		}
		LOGI("ceate audio transfer internal message queue complete\n");

		ret = rtos_create_psram_thread(&aud_tras_info->aud_tras_task_hdl,
								 4,
								 "aud_tras",
								 (beken_thread_function_t)aud_tras_main,
								 1024 * 8,
								 (beken_thread_arg_t)&aud_trs_setup_bk);
		if (ret != kNoErr)
		{
			LOGE("Error: Failed to create aud_tras task \n");
			return kGeneralErr;
		}

		LOGI("init aud_tras task complete \n");
	}
	else
	{
		goto out;
	}

	return BK_OK;

out:
	if (aud_tras_info->aud_tras_int_msg_que)
	{
		ret = rtos_deinit_queue(&aud_tras_info->aud_tras_int_msg_que);
		if (ret != kNoErr) {
			LOGE("delete message queue fail \r\n");
		}
		aud_tras_info->aud_tras_int_msg_que = NULL;
	}

	if (aud_tras_info->aud_tras_buff_addr) {
		ring_buffer_clear(&aud_tras_info->aud_tras_rb);
		os_free(aud_tras_info->aud_tras_buff_addr);
		aud_tras_info->aud_tras_buff_addr = NULL;
	}

	if (aud_tras_info)
	{
		os_free(aud_tras_info);
		aud_tras_info = NULL;
	}

	return BK_FAIL;
}

RingBufferContext *aud_tras_get_tx_rb(void)
{
	return &aud_tras_info->aud_tras_rb;
}

