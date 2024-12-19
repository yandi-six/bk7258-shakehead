/*!
*****************************************************************************

*****************************************************************************
*/

#ifndef _SDK_CFG_SHELTER_H__
#define _SDK_CFG_SHELTER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"


/***********************************/
/***        shelter            ***/
/***********************************/
//�ڵ���������
typedef struct {
    SDK_S32    enable;     /* �ڵ�ʹ�� 0-����, 1-ʹ�� */
    float     x;          /* �ڵ��������Ͻǵ�x���� */
    SDK_S8    x_opt[MAX_STR_LEN_128];
    float     y;          /* �ڵ��������Ͻǵ�y���� */
    SDK_S8    y_opt[MAX_STR_LEN_128];
    float     width;      /* �ڵ������� */
    SDK_S8    width_opt[MAX_STR_LEN_128];
    float     height;     /* �ڵ�����߶� */
    SDK_S8    height_opt[MAX_STR_LEN_128];
    SDK_U32    color;      /* �ڵ���ɫ, Ĭ�� 0:��ɫ ��RGB��ʽ*/
} SDK_NET_SHELTER_RECT;

/***********************************/
/***         osd                 ***/
/***********************************/
typedef struct {
    SDK_S32   enable;            /* 0-����, 1-ʹ�� */
    SDK_S8    text[MAX_STR_LEN_128];  //ͨ������
    float   x;
    SDK_S8    x_opt[MAX_STR_LEN_128];
    float   y;
    SDK_S8    y_opt[MAX_STR_LEN_128];
} SDK_NET_OSD_CHANNEL_NAME;

typedef struct {
    SDK_S32    enable; /* 0-����, 1-ʹ�� */
    SDK_U8     dateFormat;
                /*���ڸ�ʽ(�����ո�ʽ)��
				0��YYYY-MM-DD		������
				1��MM-DD-YYYY		������
				2��YYYY/MM/DD		������
				3��MM/DD/YYYY		������
				4��DD-MM-YYYY		������
				5��DD/MM/YYYY		������

                */
    SDK_U8     dateSprtr; //0��":"��1��"-"��2��"/" 3��"."
    SDK_U8     timeFmt; //ʱ���ʽ (0-24Сʱ��1��12Сʱ).
    float    x;
    SDK_S8    x_opt[MAX_STR_LEN_128];
    float    y;
    SDK_S8    y_opt[MAX_STR_LEN_128];
    SDK_U8		displayWeek; //�Ƿ���ʾ���ڼ�
} SDK_NET_OSD_DATETIME;



/***********************************/
/***    channel                  ***/
/***********************************/

typedef struct {
    SDK_NET_OSD_CHANNEL_NAME osdChannelName;
    SDK_NET_OSD_DATETIME     osdDatetime;

} SDK_NET_CHANNEL_INFO, *LPSDK_NET_CHANNEL_INFO;

typedef struct {
    SDK_NET_CHANNEL_INFO channelInfo;
    SDK_NET_SHELTER_RECT shelterRect; //֧��4���ڵ�����
} SDK_NET_CHANNEL_CFG, *LPSDK_NET_CHANNEL_CFG;

extern int ChannelCfgSave();
extern int ChannelCfgLoad();
extern void ChannelCfgPrint();
extern int ChannelCfgLoadDefValue();
extern cJSON* ChannelCfgLoadJson();
extern int ChannelJsonSaveCfg(cJSON *json);

#define CHANNEL_CFG_FILE "channel_cfg.cjson"


extern SDK_NET_CHANNEL_CFG runChannelCfg;
extern SDK_CFG_MAP osdMap[];
extern SDK_CFG_MAP shelterRectMap[];

#ifdef __cplusplus
}
#endif
#endif

