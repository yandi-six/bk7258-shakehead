/*!
*****************************************************************************

*****************************************************************************
*/

#ifndef _SDK_CFG_PTZ_H__
#define _SDK_CFG_PTZ_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"


/***********************************/
/***         ptz                 ***/
/***********************************/
/*��̨������485���ڲ���*/
typedef struct {
    SDK_U32 channel;         /* ��Ӧͨ�� */
    SDK_U32 baudRate;        /* ������(bps)��600bps -- 115200bps */
    SDK_U8  dataBit;         /* �����м�λ 0��5λ��1��6λ��2��7λ��3��8λ */
    SDK_U8  stopBit;         /* ֹͣλ 0��1λ��1��2λ  */
    SDK_U8  parity;          /* У�� 0����У�飬1����У�飬2��żУ��; */
    SDK_U8  flowcontrol;     /* 0���ޣ�1��������,2-Ӳ���� */
    SDK_S8  decoderType[32]; /* ����������, ���±�*/
    SDK_S32 workMode;        /* ����ģʽ��0-խ������(232��������PPP����),  1-����̨(232�������ڲ�������),  2-͸��ͨ�� */
    SDK_U16 decoderAddress;  /* ��������ַ:0 - 255*/
    SDK_U16 speedH;          /* ��̨H�ٶ�*/
    SDK_U16 speedV;          /* ��̨V�ٶ�*/
    SDK_U16 watchPos;        /* ����λ*/
    SDK_U32 res;
} SDK_NET_DECODERCFG,*LPSDK_NET_DECODERCFG;

#define SDK_PTZ_PROTOCOL_NUM        200     /* ���֧�ֵ���̨Э���� */
//��̨Э���ṹ����
typedef struct {
    SDK_U32 type;         /*����������ֵ����1��ʼ��������*/
    SDK_S8  describe[32]; /*��������������*/
} SDK_PTZ_PROTOCOL;


// ��̨����
typedef struct {
    SDK_U8  type;//���SDK_PTZ_LINK_TYPE��0:��Ч��1:Ԥ�õ㣬2:���Ѳ����3:���ù켣
    SDK_U8  value;
    SDK_U8  reserve[2];
} SDK_PTZ_LINK, *LPSDK_PTZ_LINK;

/*232���ڲ���*/
typedef struct {
    SDK_U32 baudRate; /* ������(bps) */
    SDK_U8  dataBit; /* �����м�λ 0��5λ��1��6λ��2��7λ��3��8λ; */
    SDK_U8  stopBit; /* ֹͣλ 0��1λ��1��2λ; */
    SDK_U8  parity; /* У�� 0����У�飬1����У�飬2��żУ��; */
    SDK_U8  flowcontrol; /* 0���ޣ�1��������,2-Ӳ���� */
    SDK_U32 workMode; /* ����ģʽ��0��խ�����䣨232��������PPP���ţ���1������̨��232�������ڲ������ƣ���2��͸��ͨ�� */
} SDK_NET_RS232CFG, *LPSDK_NET_RS232CFG;

typedef struct {
    SDK_NET_DECODERCFG decoderCfg;
    SDK_S32          ptzNum;                        /*��Ч��ptzЭ����Ŀ*/
    SDK_S8           describe[32];                  /*��������������*/
    SDK_PTZ_PROTOCOL   protocol[SDK_PTZ_PROTOCOL_NUM]; /*���200��PTZЭ��*/
} SDK_NET_PTZ_PROTOCOL_CFG;

typedef struct {
    SDK_S32     id;
    SDK_S32     serialPortID;
    SDK_S32     videoInputID;
    SDK_S32     duplexMode;   // 0-half, 1-full
    SDK_S32     controlType;  // 0- controlType, 1- external
    SDK_S32     protocol; // 0 pelco-d, 1 pelco-p
    SDK_S32     address;
} SDK_NET_PTZ_CFG;



#define PTZ_MAX_PRESET             255
#define PTZ_MAX_CRUISE_POINT_NUM   32
#define PTZ_MAX_CRUISE_GROUP_NUM   5

/* PTZԤ�Ƶ㹦�ܽṹ�� */
typedef struct tagSDK_NET_PRESET_INFO
{
    SDK_U16   nChannel;
    SDK_U16   nPresetNum;                   //Ԥ�õ����
    SDK_U32   no[PTZ_MAX_PRESET];           //Ԥ�õ����
    SDK_S8    csName[PTZ_MAX_PRESET][64];   //Ԥ�õ�����
}SDK_NET_PRESET_INFO;


/* PTZ Ѳ�����ܽṹ�� */
typedef struct tagSDK_NET_CRUISE_POINT
{
	SDK_S8 	byPointIndex;   //Ѳ�����е��±�,���ֵ����PTZ_MAX_CRUISE_POINT_NUM ��ʾ��ӵ�ĩβ
	SDK_S8 	byPresetNo;     //Ԥ�õ���
	SDK_S8 	byRemainTime;   //Ԥ�õ�����ʱ��
	SDK_S8 	bySpeed;        //��Ԥ�õ��ٶ�
}SDK_NET_CRUISE_POINT;

typedef struct tagSDK_NET_CRUISE_GROUP
{
	SDK_S8 byPointNum;        //Ԥ�õ�����
	SDK_S8 byCruiseIndex;     //��Ѳ�������
	SDK_S8 byRes[2];
	SDK_NET_CRUISE_POINT struCruisePoint[PTZ_MAX_CRUISE_POINT_NUM];
}SDK_NET_CRUISE_GROUP;

typedef struct tagSDK_NET_CRUISE_CFG
{
	SDK_S32  nChannel;
	SDK_S8   byIsCruising;		//�Ƿ���Ѳ��
	SDK_S8   byCruisingIndex;	//����Ѳ����Ѳ�����
	SDK_S8   byPointIndex;		//����Ѳ����Ԥ�õ����(�����±�)
	SDK_S8   byEnableCruise;;	//�Ƿ���Ѳ��
	SDK_NET_CRUISE_GROUP struCruise[PTZ_MAX_CRUISE_GROUP_NUM];
}SDK_NET_CRUISE_CFG;



extern int PtzCfgSave();
extern int PtzCfgLoad();
extern void PtzCfgPrint();
extern int PtzCfgLoadDefValue();
extern cJSON * PtzCfgLoadJson();
extern int PtzJsonSaveCfg(cJSON *json);


extern int PresetCruiseCfgSave();
extern int PresetCruiseCfgLoad();
extern int PresetCruiseCfgDefault();


#define PTZ_CFG_FILE    "ptz_cfg.cjson"
#define PTZ_CRUISE_FILE "ptz_cruise.cfg"

extern SDK_NET_PTZ_CFG     runPtzCfg;
extern SDK_NET_DECODERCFG  runPtzDecCfg;

extern SDK_NET_PRESET_INFO runPresetCfg;
extern SDK_NET_CRUISE_CFG  runCruiseCfg;

extern SDK_CFG_MAP ptzMap[];

#ifdef __cplusplus
}
#endif
#endif

