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
#include "sys_driver.h"
#include "bk_aud_intf_private.h"
#include "bk_aud_tras_drv.h"
//#include <driver/pwr_clk.h>
#if (CONFIG_CACHE_ENABLE)
#include "cache.h"
#endif
#include "bk_aud_tras.h"
#include "rtc_bk.h"


#define AUD_INTF_TAG "aud_intf"

#define LOGI(...) BK_LOGI(AUD_INTF_TAG, ##__VA_ARGS__)
#define LOGW(...) BK_LOGW(AUD_INTF_TAG, ##__VA_ARGS__)
#define LOGE(...) BK_LOGE(AUD_INTF_TAG, ##__VA_ARGS__)
#define LOGD(...) BK_LOGD(AUD_INTF_TAG, ##__VA_ARGS__)

/* check aud_intf busy status */
/*
#define CHECK_AUD_INTF_BUSY_STA() do {\
		if (aud_intf_info.api_info.busy_status) {\
			return BK_ERR_AUD_INTF_BUSY;\
		}\
		aud_intf_info.api_info.busy_status = true;\
	} while(0)
*/

#define CONFIG_AUD_RX_COUNT_DEBUG
#ifdef CONFIG_AUD_RX_COUNT_DEBUG
#define AUD_RX_DEBUG_INTERVAL (1000 * 2)
#endif

#ifdef CONFIG_AUD_RX_COUNT_DEBUG
typedef struct {
	beken_timer_t timer;
	uint32_t rx_size;
} aud_rx_count_debug_t;

static aud_rx_count_debug_t aud_rx_count = {0};
#endif


//aud_intf_all_setup_t aud_all_setup;
aud_intf_info_t aud_intf_info = DEFAULT_AUD_INTF_CONFIG();

/* extern api */
//static void audio_tras_demo_deconfig(void);
#if 0
beken_semaphore_t mailbox_media_app_aud_sem = NULL;
media_mailbox_msg_t *media_aud_mailbox_msg = NULL;
#endif

/*
static const unsigned int PCM_8000[] = {
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
	0x0001, 0x5A82, 0x7FFF, 0x5A82, 0x0000, 0xA57F, 0x8001, 0xA57E,
};
*/

static bk_err_t aud_intf_voc_write_spk_data(uint8_t *dac_buff, uint32_t size);

#if 0
bk_err_t mailbox_media_aud_send_msg(media_event_t event, void *param)
{
	bk_err_t ret = BK_OK;

	if (!media_aud_mailbox_msg) {
		media_aud_mailbox_msg = rtc_bk_malloc(sizeof(media_mailbox_msg_t));
		if (!media_aud_mailbox_msg) {
			LOGE("%s, malloc media_aud_mailbox_msg fail\n", __func__);
			aud_intf_info.api_info.busy_status = false;
			return BK_ERR_NO_MEM;
		}
	}

	media_aud_mailbox_msg->event = event;
	media_aud_mailbox_msg->param = (uint32_t)param;
	media_aud_mailbox_msg->sem = mailbox_media_app_aud_sem;
	ret = msg_send_req_to_media_app_mailbox_sync(media_aud_mailbox_msg);
	if (ret != kNoErr)
	{
		LOGE("%s failed 0x%x\n", __func__, ret);
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}
#endif

bk_err_t aud_intf_send_msg(aud_intf_event_t op, uint32_t data, uint32_t size)
{
	bk_err_t ret;
	aud_intf_msg_t msg;

	msg.op = op;
	msg.data = data;
	msg.size = size;
	if (aud_intf_info.aud_intf_msg_que) {
		ret = rtos_push_to_queue(&aud_intf_info.aud_intf_msg_que, &msg, BEKEN_NO_WAIT);
		if (kNoErr != ret) {
			LOGE("aud_tras_send_int_msg fail \r\n");
			return kOverrunErr;
		}

		return ret;
	}
	return kNoResourcesErr;
}

static bk_err_t aud_intf_deinit(void)
{
	bk_err_t ret;
	aud_intf_msg_t msg;

	msg.op = AUD_INTF_EVENT_EXIT;
	msg.data = 0;
	msg.size = 0;
	if (aud_intf_info.aud_intf_msg_que) {
		ret = rtos_push_to_queue_front(&aud_intf_info.aud_intf_msg_que, &msg, BEKEN_NO_WAIT);
		if (kNoErr != ret) {
			LOGE("audio send msg: AUD_INTF_EVENT_EXIT fail \r\n");
			return kOverrunErr;
		}

		return ret;
	}
	return kNoResourcesErr;
}

static void aud_intf_main(beken_thread_arg_t param_data)
{
	bk_err_t ret = BK_OK;

	aud_intf_msg_t msg;
	while (1) {
		ret = rtos_pop_from_queue(&aud_intf_info.aud_intf_msg_que, &msg, BEKEN_WAIT_FOREVER);
		if (kNoErr == ret) {
			switch (msg.op) {
				case AUD_INTF_EVENT_IDLE:
					break;

				case AUD_INTF_EVENT_MIC_TX:
					if (aud_intf_info.drv_info.setup.aud_intf_tx_mic_data) {
						aud_intf_info.drv_info.setup.aud_intf_tx_mic_data((unsigned char *)msg.data, msg.size);
					}
					break;

				case AUD_INTF_EVENT_SPK_RX:
					if (aud_intf_info.drv_info.setup.aud_intf_rx_spk_data) {
						aud_intf_info.drv_info.setup.aud_intf_rx_spk_data(msg.size);
					}
					break;

				case AUD_INTF_EVENT_UAC_STATE:
					if (aud_intf_info.aud_intf_uac_connect_state_cb) {
						aud_intf_info.aud_intf_uac_connect_state_cb((uint8_t)msg.data);
					}
					break;

				case AUD_INTF_EVENT_EXIT:
					goto aud_intf_exit;
					break;

				default:
					break;
			}
		}
	}

aud_intf_exit:

	/* delete msg queue */
	ret = rtos_deinit_queue(&aud_intf_info.aud_intf_msg_que);
	if (ret != kNoErr) {
		LOGE("%s, %d, delete message queue fail\n", __func__, __LINE__);
	}
	aud_intf_info.aud_intf_msg_que = NULL;
	LOGI("delete aud_intf_int_msg_que \r\n");

	/* delete task */
	aud_intf_info.aud_intf_thread_hdl = NULL;

	rtos_delete_thread(NULL);
}


bk_err_t bk_aud_intf_set_mode(aud_intf_work_mode_t work_mode)
{
	bk_err_t ret = BK_OK;
        LOGD("%s %d \n", __func__, __LINE__);
	if (aud_intf_info.drv_info.setup.work_mode == work_mode)
		return BK_ERR_AUD_INTF_OK;

	//CHECK_AUD_INTF_BUSY_STA();
	aud_intf_work_mode_t temp = aud_intf_info.drv_info.setup.work_mode;

	aud_intf_info.drv_info.setup.work_mode = work_mode;
	aud_intf_info.drv_status = AUD_INTF_DRV_STA_WORK;
	ret = aud_tras_drv_set_work_mode(aud_intf_info.drv_info.setup.work_mode);
	if (ret != BK_OK) {
		LOGE("aud_tras_drv_set_work_mode fail 0x%x\n",ret);
		aud_intf_info.drv_info.setup.work_mode = temp;
	} else {
		aud_intf_info.drv_status = AUD_INTF_DRV_STA_WORK;
	}

	/* create task to handle tx and rx message */
	if (ret == BK_OK) {
		if (work_mode == AUD_INTF_WORK_MODE_GENERAL || work_mode == AUD_INTF_WORK_MODE_VOICE) {
			if (!aud_intf_info.aud_intf_msg_que) {
				ret = rtos_init_queue(&aud_intf_info.aud_intf_msg_que,
									  "aud_intf_int_que",
									  sizeof(aud_intf_msg_t),
									  10);
				if (ret != kNoErr) {
					LOGE("ceate aud_intf_msg_que fail 0x%x\n",ret);
					return BK_FAIL;
				}
				LOGI("ceate aud_intf_msg_que complete \r\n");
			}

			if (!aud_intf_info.aud_intf_thread_hdl) {
				ret = rtos_create_psram_thread(&aud_intf_info.aud_intf_thread_hdl,
									 6,
									 "aud_intf",
									 (beken_thread_function_t)aud_intf_main,
									 4096,
									 NULL);
				if (ret != kNoErr) {
					LOGE("create audio transfer driver task fail 0x%x\r\n",ret);
					rtos_deinit_queue(&aud_intf_info.aud_intf_msg_que);
					aud_intf_info.aud_intf_msg_que = NULL;
					aud_intf_info.aud_intf_thread_hdl = NULL;
				}
				LOGI("create audio transfer driver task complete \r\n");
			}

			if (work_mode == AUD_INTF_WORK_MODE_VOICE) {
				/* init audio transfer task */
				aud_tras_setup_t aud_tras_setup_cfg = {0};
				aud_tras_setup_cfg.aud_tras_send_data_cb = aud_intf_info.drv_info.setup.aud_intf_tx_mic_data;
				ret = aud_tras_init(&aud_tras_setup_cfg);
				if (ret != BK_OK) {
					LOGE("aud_tras_init fail 0x%x\n",ret);
					return BK_FAIL;
				}
			}
		} else if (work_mode == AUD_INTF_WORK_MODE_NULL) {
			aud_intf_deinit();
			if (temp == AUD_INTF_WORK_MODE_VOICE) {
				aud_tras_deinit();
			}
		} else {
			//TODO nothing
		}
	}
	LOGD("%s %d \n", __func__, __LINE__);
	return ret;
}

bk_err_t bk_aud_intf_set_mic_gain(uint8_t value)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	/* check value range */
	if (value > 0x3F)
		return BK_ERR_AUD_INTF_PARAM;

	//CHECK_AUD_INTF_BUSY_STA();

	uint8_t temp = 0;

	/*check mic status */
	if (aud_intf_info.mic_status != AUD_INTF_MIC_STA_NULL || aud_intf_info.voc_status != AUD_INTF_VOC_STA_NULL) {
		if (aud_intf_info.mic_status != AUD_INTF_MIC_STA_NULL) {
			temp = aud_intf_info.mic_info.mic_gain;
			aud_intf_info.mic_info.mic_gain = value;
			ret =aud_tras_drv_set_mic_gain(aud_intf_info.mic_info.mic_gain);
			if (ret != BK_OK){
				aud_intf_info.mic_info.mic_gain = temp;
			}
		}

		if (aud_intf_info.voc_status != AUD_INTF_VOC_STA_NULL) {
			temp = aud_intf_info.voc_info.aud_setup.adc_gain;
			aud_intf_info.voc_info.aud_setup.adc_gain = value;
			ret = aud_tras_drv_set_mic_gain(aud_intf_info.voc_info.aud_setup.adc_gain);
			if (ret != BK_OK){
				aud_intf_info.voc_info.aud_setup.adc_gain = temp;
			}
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_set_mic_samp_rate(uint32_t samp_rate)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();

	/*check mic status */
	if (aud_intf_info.mic_status != AUD_INTF_MIC_STA_NULL) {
		uint32_t temp = aud_intf_info.mic_info.samp_rate;
		aud_intf_info.mic_info.samp_rate = samp_rate;
		ret = aud_tras_drv_mic_set_samp_rate(aud_intf_info.mic_info.samp_rate);
		if (ret != BK_OK){
			aud_intf_info.mic_info.samp_rate = temp;
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_set_mic_chl(aud_intf_mic_chl_t mic_chl)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();

	/*check mic status */
	if (aud_intf_info.mic_status != AUD_INTF_MIC_STA_NULL) {
		aud_intf_mic_chl_t temp = aud_intf_info.mic_info.mic_chl;
		aud_intf_info.mic_info.mic_chl = mic_chl;
		ret = aud_tras_drv_mic_set_chl(aud_intf_info.mic_info.mic_chl);
		if (ret != BK_OK){
			aud_intf_info.mic_info.mic_chl = temp;
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_set_spk_gain(uint32_t value)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	uint16_t temp = 0;

	/*check mic status */
	if (aud_intf_info.spk_status != AUD_INTF_SPK_STA_NULL || aud_intf_info.voc_status != AUD_INTF_VOC_STA_NULL) {
		if (aud_intf_info.spk_status != AUD_INTF_SPK_STA_NULL) {
			/* check value range */
			if (aud_intf_info.spk_info.spk_type == AUD_INTF_SPK_TYPE_BOARD) {
				if (value > 0x3F) {
					LOGE("the spk_gain out of range:0x00-0x3F \n");
					aud_intf_info.api_info.busy_status = false;
					return BK_ERR_AUD_INTF_PARAM;
				}
			} else {
				if (value > 100) {
					LOGE("the spk_gain out of range:0-100 \n");
					aud_intf_info.api_info.busy_status = false;
					return BK_ERR_AUD_INTF_PARAM;
				}
			}
			temp = aud_intf_info.spk_info.spk_gain;
			aud_intf_info.spk_info.spk_gain = value;
		}

		if (aud_intf_info.voc_status != AUD_INTF_VOC_STA_NULL) {
			/* check value range */
			if (aud_intf_info.voc_info.spk_type == AUD_INTF_SPK_TYPE_BOARD) {
				if (value > 0x3F) {
					LOGE("the spk_gain out of range:0x00-0x3F \n");
					aud_intf_info.api_info.busy_status = false;
					return BK_ERR_AUD_INTF_PARAM;
				}
			} else {
				if (value > 100) {
					LOGE("the spk_gain out of range:0-100 \n");
					aud_intf_info.api_info.busy_status = false;
					return BK_ERR_AUD_INTF_PARAM;
				}
			}
			temp = aud_intf_info.voc_info.aud_setup.dac_gain;
			aud_intf_info.voc_info.aud_setup.dac_gain = value;
			LOGI("set spk_gain: %d \n", aud_intf_info.voc_info.aud_setup.dac_gain);
			//return aud_tras_drv_send_msg(AUD_TRAS_DRV_VOC_SET_SPK_GAIN, &aud_intf_info.voc_info.aud_setup.dac_gain);
			//return aud_intf_send_msg_with_sem(AUD_TRAS_DRV_VOC_SET_SPK_GAIN, &aud_intf_info.voc_info.aud_setup.dac_gain, BEKEN_WAIT_FOREVER);

			ret = aud_tras_drv_set_spk_gain(aud_intf_info.voc_info.aud_setup.dac_gain);
			if (ret != BK_OK){
				aud_intf_info.voc_info.aud_setup.dac_gain = temp;
			}
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_set_spk_samp_rate(uint32_t samp_rate)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();

	/*check spk status */
	if (aud_intf_info.spk_status != AUD_INTF_SPK_STA_NULL) {
		uint32_t temp = aud_intf_info.spk_info.samp_rate;
		aud_intf_info.spk_info.samp_rate = samp_rate;
		ret = aud_tras_drv_spk_set_samp_rate(aud_intf_info.spk_info.samp_rate);
		if (ret != BK_OK){
			aud_intf_info.spk_info.samp_rate = temp;
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_set_spk_chl(aud_intf_spk_chl_t spk_chl)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();

	/*check spk status */
	if (aud_intf_info.spk_status != AUD_INTF_SPK_STA_NULL) {
		aud_intf_spk_chl_t temp = aud_intf_info.spk_info.spk_chl;
		aud_intf_info.spk_info.spk_chl = spk_chl;
		ret = aud_tras_drv_spk_set_chl(spk_chl);
		if (ret != BK_OK){
			aud_intf_info.spk_info.spk_chl = temp;
		}
	}

	aud_intf_info.api_info.busy_status = false;
	return ret;
}

bk_err_t bk_aud_intf_register_uac_connect_state_cb(void *cb)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_uac_register_connect_state_cb(cb);
	if (ret == BK_OK) {
		aud_intf_info.aud_intf_uac_connect_state_cb = cb;
	}

	return ret;
}

bk_err_t bk_aud_intf_uac_auto_connect_ctrl(bool enable)
{
	bk_err_t ret = BK_ERR_AUD_INTF_STA;

	if (aud_intf_info.uac_auto_connect == enable) {
		return BK_ERR_AUD_INTF_OK;
	}

	/* check if aud_intf driver init */
	if (aud_intf_info.drv_status != AUD_INTF_DRV_STA_IDLE && aud_intf_info.drv_status != AUD_INTF_DRV_STA_WORK) {
		return BK_ERR_AUD_INTF_STA;
	}

	//CHECK_AUD_INTF_BUSY_STA();
	bool temp = aud_intf_info.uac_auto_connect;
	aud_intf_info.uac_auto_connect = enable;
	ret = aud_tras_uac_auto_connect_ctrl(aud_intf_info.uac_auto_connect);
	if (ret != BK_OK) {
		aud_intf_info.uac_auto_connect = temp;
	}
	return ret;
}

bk_err_t bk_aud_intf_set_aec_para(aud_intf_voc_aec_para_t aec_para, uint32_t value)
{
	bk_err_t ret = BK_OK;
//	bk_err_t err = BK_ERR_AUD_INTF_FAIL;
	aud_intf_voc_aec_ctl_t *aec_ctl = NULL;

	/*check aec status */
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();

	aec_ctl = rtc_bk_malloc(sizeof(aud_intf_voc_aec_ctl_t));
	if (aec_ctl == NULL) {
		aud_intf_info.api_info.busy_status = false;
		return BK_ERR_AUD_INTF_MEMY;
	}
	aec_ctl->op = aec_para;
	aec_ctl->value = value;

	//TODO
	//cpy value before set

	switch (aec_para) {
		case AUD_INTF_VOC_AEC_MIC_DELAY:
			aud_intf_info.voc_info.aec_setup->mic_delay = value;
			break;

		case AUD_INTF_VOC_AEC_EC_DEPTH:
			aud_intf_info.voc_info.aec_setup->ec_depth = value;
			break;

		case AUD_INTF_VOC_AEC_REF_SCALE:
			aud_intf_info.voc_info.aec_setup->ref_scale = value;
			break;

		case AUD_INTF_VOC_AEC_VOICE_VOL:
			aud_intf_info.voc_info.aec_setup->voice_vol = value;
			break;

		case AUD_INTF_VOC_AEC_TXRX_THR:
			aud_intf_info.voc_info.aec_setup->TxRxThr = value;
			break;

		case AUD_INTF_VOC_AEC_TXRX_FLR:
			aud_intf_info.voc_info.aec_setup->TxRxFlr = value;
			break;

		case AUD_INTF_VOC_AEC_NS_LEVEL:
			aud_intf_info.voc_info.aec_setup->ns_level = value;
			break;

		case AUD_INTF_VOC_AEC_NS_PARA:
			aud_intf_info.voc_info.aec_setup->ns_para = value;
			break;

		case AUD_INTF_VOC_AEC_DRC:
			aud_intf_info.voc_info.aec_setup->drc = value;
			break;

		case AUD_INTF_VOC_AEC_INIT_FLAG:
			aud_intf_info.voc_info.aec_setup->init_flags = value;
			break;

		default:
			break;
	}
	ret = aud_tras_drv_set_aec_para(aec_ctl);

	rtc_bk_free(aec_ctl);
	return ret;
}

bk_err_t bk_aud_intf_get_aec_para(void)
{
	/*check aec status */
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	aud_tras_drv_get_aec_para();
     return BK_OK;
}

bk_err_t bk_aud_intf_mic_init(aud_intf_mic_setup_t *setup)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();

	aud_intf_info.mic_info.mic_chl = setup->mic_chl;
	aud_intf_info.mic_info.samp_rate = setup->samp_rate;
	aud_intf_info.mic_info.frame_size = setup->frame_size;
	aud_intf_info.mic_info.mic_gain = setup->mic_gain;
	aud_intf_info.mic_info.mic_type = setup->mic_type;
	ret = aud_tras_drv_mic_init(&aud_intf_info.mic_info);
	if (ret == BK_OK){
		aud_intf_info.mic_status = AUD_INTF_MIC_STA_IDLE;
	}
	return ret;
}

bk_err_t bk_aud_intf_mic_deinit(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_mic_deinit();
	if (ret == BK_OK){
		aud_intf_info.mic_status = AUD_INTF_MIC_STA_NULL;
	}
	return ret;
}

bk_err_t bk_aud_intf_mic_start(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_mic_start();
	if (ret == BK_OK){
		aud_intf_info.mic_status = AUD_INTF_MIC_STA_START;
	}
	return ret;
}

bk_err_t bk_aud_intf_mic_pause(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_mic_pause();
	if (ret == BK_OK){
		aud_intf_info.mic_status = AUD_INTF_MIC_STA_STOP;
	}
	return ret;
}

bk_err_t bk_aud_intf_mic_stop(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_mic_stop();
	if (ret == BK_OK){
		aud_intf_info.mic_status = AUD_INTF_MIC_STA_STOP;
	}
	return ret;
}

static void aud_intf_spk_deconfig(void)
{
	aud_intf_info.spk_info.spk_chl = AUD_INTF_SPK_CHL_LEFT;
	aud_intf_info.spk_info.samp_rate = 8000;
	aud_intf_info.spk_info.frame_size = 0;
	aud_intf_info.spk_info.spk_gain = 0;
	ring_buffer_clear(aud_intf_info.spk_info.spk_rx_rb);
	if (aud_intf_info.spk_info.spk_rx_ring_buff) {
		rtc_bk_free(aud_intf_info.spk_info.spk_rx_ring_buff);
		aud_intf_info.spk_info.spk_rx_ring_buff = NULL;
	}

	if (aud_intf_info.spk_info.spk_rx_rb) {
		rtc_bk_free(aud_intf_info.spk_info.spk_rx_rb);
		aud_intf_info.spk_info.spk_rx_rb = NULL;
	}
}

bk_err_t bk_aud_intf_spk_init(aud_intf_spk_setup_t *setup)
{
	bk_err_t ret = BK_ERR_AUD_INTF_OK;
	bk_err_t err = BK_ERR_AUD_INTF_FAIL;
	//CHECK_AUD_INTF_BUSY_STA();

	aud_intf_info.spk_info.spk_chl = setup->spk_chl;
	aud_intf_info.spk_info.samp_rate = setup->samp_rate;
	aud_intf_info.spk_info.frame_size = setup->frame_size;
	aud_intf_info.spk_info.spk_gain = setup->spk_gain;
	aud_intf_info.spk_info.work_mode = setup->work_mode;
	aud_intf_info.spk_info.spk_type = setup->spk_type;
	aud_intf_info.spk_info.fifo_frame_num = 4;			//default: 4

	aud_intf_info.spk_info.spk_rx_ring_buff = rtc_bk_malloc(aud_intf_info.spk_info.frame_size * aud_intf_info.spk_info.fifo_frame_num);
	if (aud_intf_info.spk_info.spk_rx_ring_buff == NULL) {
		LOGE("malloc spk_rx_ring_buff fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_spk_init_exit;
	}

	aud_intf_info.spk_info.spk_rx_rb = rtc_bk_malloc(sizeof(RingBufferContext));
	if (aud_intf_info.spk_info.spk_rx_rb == NULL) {
		LOGE("malloc spk_rx_rb fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_spk_init_exit;
	}

	ring_buffer_init(aud_intf_info.spk_info.spk_rx_rb, (uint8_t *)aud_intf_info.spk_info.spk_rx_ring_buff, aud_intf_info.spk_info.frame_size * aud_intf_info.spk_info.fifo_frame_num, DMA_ID_MAX, RB_DMA_TYPE_NULL);
	ret = aud_tras_drv_spk_init(&aud_intf_info.spk_info);
	if(ret!= BK_OK){
		err = ret;
		goto aud_intf_spk_init_exit;
	}
	aud_intf_info.spk_status = AUD_INTF_SPK_STA_IDLE;

	return BK_ERR_AUD_INTF_OK;

aud_intf_spk_init_exit:
	if (aud_intf_info.spk_info.spk_rx_ring_buff != NULL)
		rtc_bk_free(aud_intf_info.spk_info.spk_rx_ring_buff);
	if (aud_intf_info.spk_info.spk_rx_rb != NULL)
		rtc_bk_free(aud_intf_info.spk_info.spk_rx_rb);
	aud_intf_info.api_info.busy_status = false;
	return err;
}

bk_err_t bk_aud_intf_spk_deinit(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_spk_deinit();
	if (ret == BK_OK){
		aud_intf_info.spk_status = AUD_INTF_SPK_STA_NULL;
	}
	return ret;
}

bk_err_t bk_aud_intf_spk_start(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_spk_start();
	if (ret == BK_OK){
		aud_intf_info.spk_status = AUD_INTF_SPK_STA_START;
	}
	return ret;
}

bk_err_t bk_aud_intf_spk_pause(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_spk_pause();
	if (ret == BK_OK){
		aud_intf_info.spk_status = AUD_INTF_SPK_STA_STOP;
	}
	return ret;
}

bk_err_t bk_aud_intf_spk_stop(void)
{
	bk_err_t ret = BK_OK;
	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_spk_stop();
	if (ret == BK_OK){
		aud_intf_info.spk_status = AUD_INTF_SPK_STA_STOP;
	}
	return ret;
}

static void aud_intf_voc_deconfig(void)
{
	/* audio deconfig */
	aud_intf_info.voc_info.samp_rate = 8000;	//default value
	aud_intf_info.voc_info.aud_setup.adc_gain = 0;
	aud_intf_info.voc_info.aud_setup.dac_gain = 0;
	aud_intf_info.voc_info.aud_setup.mic_frame_number = 0;
	aud_intf_info.voc_info.aud_setup.mic_samp_rate_points = 0;
	aud_intf_info.voc_info.aud_setup.speaker_frame_number = 0;
	aud_intf_info.voc_info.aud_setup.speaker_samp_rate_points = 0;

	/* aec deconfig */
	if (aud_intf_info.voc_info.aec_enable && aud_intf_info.voc_info.aec_setup) {
		rtc_bk_free(aud_intf_info.voc_info.aec_setup);
	}
	aud_intf_info.voc_info.aec_setup = NULL;
	aud_intf_info.voc_info.aec_enable = false;

	/* tx deconfig */
	aud_intf_info.voc_info.tx_info.buff_length = 0;
	aud_intf_info.voc_info.tx_info.tx_buff_status = false;
	aud_intf_info.voc_info.tx_info.ping.busy_status = false;
	if (aud_intf_info.voc_info.tx_info.ping.buff_addr) {
		rtc_bk_free(aud_intf_info.voc_info.tx_info.ping.buff_addr);
		aud_intf_info.voc_info.tx_info.ping.buff_addr = NULL;
	}

	aud_intf_info.voc_info.tx_info.pang.busy_status = false;
	if (aud_intf_info.voc_info.tx_info.pang.buff_addr) {
		rtc_bk_free(aud_intf_info.voc_info.tx_info.pang.buff_addr);
		aud_intf_info.voc_info.tx_info.pang.buff_addr = NULL;
	}

	/* rx deconfig */
	aud_intf_info.voc_info.rx_info.rx_buff_status = false;
	if (aud_intf_info.voc_info.rx_info.decoder_ring_buff) {
		rtc_bk_free(aud_intf_info.voc_info.rx_info.decoder_ring_buff);
		aud_intf_info.voc_info.rx_info.decoder_ring_buff = NULL;
	}

	if (aud_intf_info.voc_info.rx_info.decoder_rb) {
		rtc_bk_free(aud_intf_info.voc_info.rx_info.decoder_rb);
		aud_intf_info.voc_info.rx_info.decoder_rb = NULL;
	}

	aud_intf_info.voc_info.rx_info.frame_num = 0;
	aud_intf_info.voc_info.rx_info.frame_size = 0;
	aud_intf_info.voc_info.rx_info.fifo_frame_num = 0;
	aud_intf_info.voc_info.rx_info.rx_buff_seq_tail = 0;
	aud_intf_info.voc_info.rx_info.aud_trs_read_seq = 0;
}

#ifdef CONFIG_AUD_RX_COUNT_DEBUG
static void aud_rx_lost_count_dump(void *param)
{
	aud_rx_count.rx_size = aud_rx_count.rx_size / 1024 / (AUD_RX_DEBUG_INTERVAL / 1000);

	LOGI("[AUD Rx] %uKB/s \r\n", aud_rx_count.rx_size);
	aud_rx_count.rx_size = 0;
}
#endif

bk_err_t bk_aud_intf_voc_init(aud_intf_voc_setup_t setup)
{
	bk_err_t ret = BK_OK;
	bk_err_t err = BK_ERR_AUD_INTF_FAIL;
	//CHECK_AUD_INTF_BUSY_STA();

#ifdef CONFIG_AUD_RX_COUNT_DEBUG
	if (aud_rx_count.timer.handle != NULL)
	{
		ret = rtos_deinit_timer(&aud_rx_count.timer);
		if (BK_OK != ret)
		{
			LOGE("deinit aud_tx_count time fail\r\n");
			err = BK_ERR_AUD_INTF_TIMER;
			goto aud_intf_voc_init_exit;
		}
		aud_rx_count.timer.handle = NULL;
	}

	aud_rx_count.rx_size = 0;

	ret = rtos_init_timer(&aud_rx_count.timer, AUD_RX_DEBUG_INTERVAL, aud_rx_lost_count_dump, NULL);
	if (ret != BK_OK) {
		LOGE("rtos_init_timer fail \n");
		err = BK_ERR_AUD_INTF_TIMER;
		goto aud_intf_voc_init_exit;
	}
	ret = rtos_start_timer(&aud_rx_count.timer);
	if (ret != BK_OK) {
		LOGE("rtos_start_timer fail \n");
	}
	LOGI("start audio rx count timer complete \r\n");
#endif

	//aud_tras_drv_setup.aud_trs_mode = demo_setup.mode;
	aud_intf_info.voc_info.samp_rate = setup.samp_rate;
	aud_intf_info.voc_info.aec_enable = setup.aec_enable;
	aud_intf_info.voc_info.data_type = setup.data_type;
	/* audio config */
	aud_intf_info.voc_info.aud_setup.adc_gain = setup.mic_gain;	//default: 0x2d
	aud_intf_info.voc_info.aud_setup.dac_gain = setup.spk_gain;	//default: 0x2d
	if (aud_intf_info.voc_info.samp_rate == 16000) {
		aud_intf_info.voc_info.aud_setup.mic_samp_rate_points = 320;	//if AEC enable , the value is equal to aec_samp_rate_points, and the value not need to set
		aud_intf_info.voc_info.aud_setup.speaker_samp_rate_points = 320;	//if AEC enable , the value is equal to aec_samp_rate_points, and the value not need to set
	} else {
		aud_intf_info.voc_info.aud_setup.mic_samp_rate_points = 160;	//if AEC enable , the value is equal to aec_samp_rate_points, and the value not need to set
		aud_intf_info.voc_info.aud_setup.speaker_samp_rate_points = 160;	//if AEC enable , the value is equal to aec_samp_rate_points, and the value not need to set
	}
	aud_intf_info.voc_info.aud_setup.mic_frame_number = 2;
	aud_intf_info.voc_info.aud_setup.speaker_frame_number = 2;
	aud_intf_info.voc_info.aud_setup.spk_mode = setup.spk_mode;
	aud_intf_info.voc_info.mic_en = setup.mic_en;
	aud_intf_info.voc_info.spk_en = setup.spk_en;
	aud_intf_info.voc_info.mic_type = setup.mic_type;
	aud_intf_info.voc_info.spk_type = setup.spk_type;

	/* aec config */
	if (aud_intf_info.voc_info.aec_enable) {
		aud_intf_info.voc_info.aec_setup = rtc_bk_malloc(sizeof(aec_config_t));
		if (aud_intf_info.voc_info.aec_setup == NULL) {
			LOGE("malloc aec_setup fail \r\n");
			err = BK_ERR_AUD_INTF_MEMY;
			goto aud_intf_voc_init_exit;
		}
		aud_intf_info.voc_info.aec_setup->init_flags = 0x1f;
		aud_intf_info.voc_info.aec_setup->mic_delay = 0;
		aud_intf_info.voc_info.aec_setup->ec_depth = setup.aec_cfg.ec_depth;
		aud_intf_info.voc_info.aec_setup->ref_scale = setup.aec_cfg.ref_scale;
		aud_intf_info.voc_info.aec_setup->TxRxThr = setup.aec_cfg.TxRxThr;
		aud_intf_info.voc_info.aec_setup->TxRxFlr = setup.aec_cfg.TxRxFlr;
		aud_intf_info.voc_info.aec_setup->voice_vol = 14;
		aud_intf_info.voc_info.aec_setup->ns_level = setup.aec_cfg.ns_level;
		aud_intf_info.voc_info.aec_setup->ns_para = setup.aec_cfg.ns_para;
		aud_intf_info.voc_info.aec_setup->drc = 15;
	} else {
		aud_intf_info.voc_info.aec_setup = NULL;
	}

	/* tx config */
	switch (aud_intf_info.voc_info.data_type) {
		case AUD_INTF_VOC_DATA_TYPE_G711A:
		case AUD_INTF_VOC_DATA_TYPE_G711U:
			aud_intf_info.voc_info.tx_info.buff_length = aud_intf_info.voc_info.aud_setup.mic_samp_rate_points;
			break;

		case AUD_INTF_VOC_DATA_TYPE_PCM:
			aud_intf_info.voc_info.tx_info.buff_length = aud_intf_info.voc_info.aud_setup.mic_samp_rate_points * 2;
			break;

		default:
			break;
	}
	aud_intf_info.voc_info.tx_info.ping.busy_status = false;
	aud_intf_info.voc_info.tx_info.ping.buff_addr = rtc_bk_malloc(aud_intf_info.voc_info.tx_info.buff_length);
	if (aud_intf_info.voc_info.tx_info.ping.buff_addr == NULL) {
		LOGE("malloc pingpang buffer of tx fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_voc_init_exit;
	}
	aud_intf_info.voc_info.tx_info.pang.busy_status = false;
	aud_intf_info.voc_info.tx_info.pang.buff_addr = rtc_bk_malloc(aud_intf_info.voc_info.tx_info.buff_length);
	if (aud_intf_info.voc_info.tx_info.pang.buff_addr == NULL) {
		LOGE("malloc pang buffer of tx fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_voc_init_exit;
	}
	aud_intf_info.voc_info.tx_info.tx_buff_status = true;

	/* rx config */
	aud_intf_info.voc_info.rx_info.aud_trs_read_seq = 0;
	switch (aud_intf_info.voc_info.data_type) {
		case AUD_INTF_VOC_DATA_TYPE_G711A:
		case AUD_INTF_VOC_DATA_TYPE_G711U:
			aud_intf_info.voc_info.rx_info.frame_size = 320;		//apk receive one frame 40ms
			//aud_intf_info.voc_info.rx_info.frame_size = aud_intf_info.voc_info.aud_setup.mic_samp_rate_points;
			break;

		case AUD_INTF_VOC_DATA_TYPE_PCM:
			aud_intf_info.voc_info.rx_info.frame_size = 320 * 2;		//apk receive one frame 40ms
			//aud_intf_info.voc_info.rx_info.frame_size = aud_intf_info.voc_info.aud_setup.mic_samp_rate_points * 2;
			break;

		default:
			break;
	}
	aud_intf_info.voc_info.rx_info.frame_num = setup.frame_num;
	aud_intf_info.voc_info.rx_info.rx_buff_seq_tail = 0;
	aud_intf_info.voc_info.rx_info.fifo_frame_num = setup.fifo_frame_num;
	aud_intf_info.voc_info.rx_info.decoder_ring_buff = rtc_bk_malloc(aud_intf_info.voc_info.rx_info.frame_size * aud_intf_info.voc_info.rx_info.frame_num + CONFIG_AUD_RING_BUFF_SAFE_INTERVAL);
	if (aud_intf_info.voc_info.rx_info.decoder_ring_buff == NULL) {
		LOGE("malloc decoder ring buffer of rx fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_voc_init_exit;
	}
	LOGI("malloc decoder_ring_buff:%p, size:%d \r\n", aud_intf_info.voc_info.rx_info.decoder_ring_buff, aud_intf_info.voc_info.rx_info.frame_size * aud_intf_info.voc_info.rx_info.frame_num);
	aud_intf_info.voc_info.rx_info.decoder_rb = rtc_bk_malloc(sizeof(RingBufferContext));
	if (aud_intf_info.voc_info.rx_info.decoder_rb == NULL) {
		LOGE("malloc decoder_rb fail \r\n");
		err = BK_ERR_AUD_INTF_MEMY;
		goto aud_intf_voc_init_exit;
	}
	ring_buffer_init(aud_intf_info.voc_info.rx_info.decoder_rb, (uint8_t *)aud_intf_info.voc_info.rx_info.decoder_ring_buff, aud_intf_info.voc_info.rx_info.frame_size * aud_intf_info.voc_info.rx_info.frame_num + CONFIG_AUD_RING_BUFF_SAFE_INTERVAL, DMA_ID_MAX, RB_DMA_TYPE_NULL);
	aud_intf_info.voc_info.rx_info.rx_buff_status = true;

	LOGI("decoder_rb:%p \r\n",aud_intf_info.voc_info.rx_info.decoder_rb);

	aud_intf_info.voc_info.aud_tx_rb = aud_tras_get_tx_rb();
	ret = aud_tras_drv_voc_init(&aud_intf_info.voc_info);
	if (ret != BK_OK) {
		goto aud_intf_voc_init_exit;
	}

	aud_intf_info.voc_status = AUD_INTF_VOC_STA_IDLE;

	return BK_ERR_AUD_INTF_OK;

aud_intf_voc_init_exit:
#ifdef CONFIG_AUD_RX_COUNT_DEBUG
	if (aud_rx_count.timer.handle) {
		bk_err_t ret = rtos_stop_timer(&aud_rx_count.timer);
		if (ret != BK_OK) {
			LOGE("stop aud_rx_count timer fail \n");
		}
		ret = rtos_deinit_timer(&aud_rx_count.timer);
		if (ret != BK_OK) {
			LOGE("deinit aud_rx_count timer fail \n");
		}
		aud_rx_count.timer.handle = NULL;
		aud_rx_count.rx_size = 0;
	}
#endif

	if (aud_intf_info.voc_info.aec_setup != NULL) {
		rtc_bk_free(aud_intf_info.voc_info.aec_setup);
		aud_intf_info.voc_info.aec_setup = NULL;
	}

	if (aud_intf_info.voc_info.tx_info.ping.buff_addr != NULL) {
		rtc_bk_free(aud_intf_info.voc_info.tx_info.ping.buff_addr);
		aud_intf_info.voc_info.tx_info.ping.buff_addr = NULL;
	}

	if (aud_intf_info.voc_info.tx_info.pang.buff_addr != NULL) {
		rtc_bk_free(aud_intf_info.voc_info.tx_info.pang.buff_addr);
		aud_intf_info.voc_info.tx_info.pang.buff_addr = NULL;
	}

	if (aud_intf_info.voc_info.rx_info.decoder_ring_buff != NULL) {
		rtc_bk_free(aud_intf_info.voc_info.rx_info.decoder_ring_buff);
		aud_intf_info.voc_info.rx_info.decoder_ring_buff = NULL;
	}

	if (aud_intf_info.voc_info.rx_info.decoder_rb != NULL) {
		rtc_bk_free(aud_intf_info.voc_info.rx_info.decoder_rb);
		aud_intf_info.voc_info.rx_info.decoder_rb = NULL;
	}

	aud_intf_info.api_info.busy_status = false;
	return err;
}

bk_err_t bk_aud_intf_voc_deinit(void)
{
	bk_err_t ret = BK_OK;

	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL) {
		LOGI("voice is alreay deinit \r\n");
		return BK_ERR_AUD_INTF_OK;
	}

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_deinit();
	if (ret != BK_OK) {
		LOGE("%s fail, result: %d \r\n", __func__, ret);
	} else {
		aud_intf_voc_deconfig();
		aud_intf_info.voc_status = AUD_INTF_VOC_STA_NULL;
	}

#ifdef CONFIG_AUD_RX_COUNT_DEBUG
	if (aud_rx_count.timer.handle) {
		ret = rtos_stop_timer(&aud_rx_count.timer);
		if (ret != BK_OK) {
			LOGE("stop aud_rx_count timer fail \n");
		}
		ret = rtos_deinit_timer(&aud_rx_count.timer);
		if (ret != BK_OK) {
			LOGE("deinit aud_rx_count timer fail \n");
		}
		aud_rx_count.timer.handle = NULL;
		aud_rx_count.rx_size = 0;
	}
#endif

	return ret;
}

bk_err_t bk_aud_intf_voc_start(void)
{
	bk_err_t ret = BK_OK;

	switch (aud_intf_info.voc_status) {
		case AUD_INTF_VOC_STA_NULL:
		case AUD_INTF_VOC_STA_START:
			return BK_ERR_AUD_INTF_OK;

		case AUD_INTF_VOC_STA_IDLE:
		case AUD_INTF_VOC_STA_STOP:
			if (ring_buffer_get_fill_size(aud_intf_info.voc_info.rx_info.decoder_rb)/aud_intf_info.voc_info.rx_info.frame_size < aud_intf_info.voc_info.rx_info.fifo_frame_num) {
				uint8_t *temp_buff = NULL;
				uint32_t temp_size = aud_intf_info.voc_info.rx_info.frame_size * aud_intf_info.voc_info.rx_info.fifo_frame_num - ring_buffer_get_fill_size(aud_intf_info.voc_info.rx_info.decoder_rb);
				temp_buff = rtc_bk_malloc(temp_size);
				if (temp_buff == NULL) {
					return BK_ERR_AUD_INTF_MEMY;
				} else {
					switch (aud_intf_info.voc_info.data_type) {
						case AUD_INTF_VOC_DATA_TYPE_G711A:
							rtc_bk_memset(temp_buff, 0xD5, temp_size);
							break;

						case AUD_INTF_VOC_DATA_TYPE_PCM:
							rtc_bk_memset(temp_buff, 0x00, temp_size);
							break;

						case AUD_INTF_VOC_DATA_TYPE_G711U:
							rtc_bk_memset(temp_buff, 0xFF, temp_size);
							break;

						default:
							break;
					}

					aud_intf_voc_write_spk_data(temp_buff, temp_size);
					rtc_bk_free(temp_buff);
				}
			}
			break;

		default:
			return BK_ERR_AUD_INTF_FAIL;
	}

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_start();
	if (ret != BK_OK) {
		LOGE("%s fail, result: %d \r\n", __func__, ret);
	} else {
		aud_intf_info.voc_status = AUD_INTF_VOC_STA_START;
	}
	return ret;
}

bk_err_t bk_aud_intf_voc_stop(void)
{
	bk_err_t ret = BK_OK;

	switch (aud_intf_info.voc_status) {
		case AUD_INTF_VOC_STA_NULL:
		case AUD_INTF_VOC_STA_IDLE:
		case AUD_INTF_VOC_STA_STOP:
			return BK_ERR_AUD_INTF_STA;
			break;

		case AUD_INTF_VOC_STA_START:
			break;

		default:
			return BK_ERR_AUD_INTF_FAIL;
	}

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_stop();
	if (ret != BK_OK) {
		LOGE("%s fail, result: %d \r\n", __func__, ret);
	} else {
		aud_intf_info.voc_status = AUD_INTF_VOC_STA_STOP;
	}

	return ret;
}

bk_err_t bk_aud_intf_voc_mic_ctrl(aud_intf_voc_mic_ctrl_t mic_en)
{
	bk_err_t ret = BK_OK;
	if (aud_intf_info.voc_info.mic_en == mic_en) {
		return BK_OK;
	}

	aud_intf_voc_mic_ctrl_t temp = aud_intf_info.voc_info.mic_en;
	aud_intf_info.voc_info.mic_en = mic_en;

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_ctrl_mic(aud_intf_info.voc_info.mic_en);
	if (ret != BK_OK){
		aud_intf_info.voc_info.mic_en = temp;
	}

	return ret;
}

bk_err_t bk_aud_intf_voc_spk_ctrl(aud_intf_voc_spk_ctrl_t spk_en)
{
	bk_err_t ret = BK_OK;
	if (aud_intf_info.voc_info.spk_en == spk_en) {
		return BK_OK;
	}

	aud_intf_voc_spk_ctrl_t temp = aud_intf_info.voc_info.spk_en;
	aud_intf_info.voc_info.spk_en = spk_en;

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_ctrl_spk(aud_intf_info.voc_info.spk_en);
	if (ret != BK_OK){
		aud_intf_info.voc_info.spk_en = temp;
	}
	return ret;
}

bk_err_t bk_aud_intf_voc_aec_ctrl(bool aec_en)
{
	bk_err_t ret = BK_OK;
	if (aud_intf_info.voc_info.aec_enable == aec_en) {
		return BK_OK;
	}

	bool temp = aud_intf_info.voc_info.aec_enable;
	aud_intf_info.voc_info.aec_enable = aec_en;

	//CHECK_AUD_INTF_BUSY_STA();
	ret = aud_tras_drv_voc_ctrl_aec(aud_intf_info.voc_info.aec_enable);
	if (ret != BK_OK){
		aud_intf_info.voc_info.aec_enable = temp;
	}
	return ret;
}

bk_err_t bk_aud_intf_drv_init(aud_intf_drv_setup_t *setup)
{
	bk_err_t ret = BK_OK;
	bk_err_t err = BK_ERR_AUD_INTF_FAIL;


	if (aud_intf_info.drv_status != AUD_INTF_DRV_STA_NULL) {
		LOGI("aud_intf driver already init \r\n");
		return BK_ERR_AUD_INTF_OK;
	}

	/* save drv_info */
	aud_intf_info.drv_info.setup = *setup;
//	aud_intf_info.drv_info.aud_tras_drv_com_event_cb = aud_tras_drv_com_event_cb_handle;


	/* init audio interface driver */
	LOGI("init aud_intf driver in CPU1 mode \r\n");
	ret = aud_tras_drv_init(&aud_intf_info.drv_info);

	return ret;
}

bk_err_t bk_aud_intf_drv_deinit(void)
{
	bk_err_t ret = BK_OK;

	if (aud_intf_info.drv_status == AUD_INTF_DRV_STA_NULL) {
		LOGI("aud_intf already deinit \r\n");
		return BK_OK;
	}

	/* reset uac_auto_connect */
	aud_intf_info.uac_auto_connect = true;

	ret = aud_tras_drv_deinit();
	aud_intf_info.drv_status = AUD_INTF_DRV_STA_NULL;



	return BK_ERR_AUD_INTF_OK;
}

/* write speaker data in voice work mode */
static bk_err_t aud_intf_voc_write_spk_data(uint8_t *dac_buff, uint32_t size)
{
	uint32_t write_size = 0;

//	LOGI("enter: %s \r\n", __func__);

	/* check aud_intf status */
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

#if (CONFIG_CACHE_ENABLE)
	flush_all_dcache();
#endif

	if (ring_buffer_get_free_size(aud_intf_info.voc_info.rx_info.decoder_rb) >= size) {
		write_size = ring_buffer_write(aud_intf_info.voc_info.rx_info.decoder_rb, dac_buff, size);
		if (write_size != size) {
			LOGE("write decoder_ring_buff fail, size:%d \r\n", size);
			return BK_FAIL;
		}
		aud_intf_info.voc_info.rx_info.rx_buff_seq_tail += size/(aud_intf_info.voc_info.rx_info.frame_size);
	}

	return BK_OK;
}

/* write speaker data in general work mode */
static bk_err_t aud_intf_genl_write_spk_data(uint8_t *dac_buff, uint32_t size)
{
	uint32_t write_size = 0;

	//LOGI("enter: %s \r\n", __func__);

	if (aud_intf_info.spk_status == AUD_INTF_SPK_STA_START || aud_intf_info.spk_status == AUD_INTF_SPK_STA_IDLE) {
		if (ring_buffer_get_free_size(aud_intf_info.spk_info.spk_rx_rb) >= size) {
			write_size = ring_buffer_write(aud_intf_info.spk_info.spk_rx_rb, dac_buff, size);
			if (write_size != size) {
				LOGE("write spk_rx_ring_buff fail, write_size:%d \r\n", write_size);
				return BK_FAIL;
			}
		}
	}

	return BK_OK;
}

bk_err_t bk_aud_intf_write_spk_data(uint8_t *dac_buff, uint32_t size)
{
	bk_err_t ret = BK_OK;

	//LOGI("enter: %s \r\n", __func__);

	switch (aud_intf_info.drv_info.setup.work_mode) {
		case AUD_INTF_WORK_MODE_GENERAL:
			ret = aud_intf_genl_write_spk_data(dac_buff, size);
			break;

		case AUD_INTF_WORK_MODE_VOICE:
#ifdef CONFIG_AUD_RX_COUNT_DEBUG
			aud_rx_count.rx_size += size;
#endif
			ret = aud_intf_voc_write_spk_data(dac_buff, size);
			break;

		default:
			ret = BK_FAIL;
			break;
	}

	return ret;
}


bk_err_t bk_aud_intf_voc_tx_debug(aud_intf_dump_data_callback dump_callback)
{
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	aud_tras_drv_voc_tx_debug(dump_callback);
	return BK_OK;
}

bk_err_t bk_aud_intf_voc_rx_debug(aud_intf_dump_data_callback dump_callback)
{
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	aud_tras_drv_voc_rx_debug(dump_callback);
	return BK_OK;
}

bk_err_t bk_aud_intf_voc_aec_debug(aud_intf_dump_data_callback dump_callback)
{
	if (aud_intf_info.voc_status == AUD_INTF_VOC_STA_NULL)
		return BK_ERR_AUD_INTF_STA;

	//CHECK_AUD_INTF_BUSY_STA();
	aud_tras_drv_voc_aec_debug(dump_callback);
	return BK_OK;
}
