/*!
*****************************************************************************

*****************************************************************************
*/

#ifndef _SDK_CFG_SNAP_H__
#define _SDK_CFG_SNAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

/***********************************/
/***         snap                ***/
/***********************************/
/* ��ʱץͼ */
typedef struct {
    SDK_S32     enable;
    SDK_SCHEDTIME scheduleTime[7][12];/**��ͨ����videoloss�Ĳ���ʱ��*/
    SDK_S32     interval;           // ��msһ��
    SDK_S32     nums;               // һ�μ���
    SDK_S32     pictureQuality;     // ���:0 ��:1  һ��:2  ����:3��:4
    SDK_S32     imageSize;          // �����С��0: 1080p 1: 720p 2 D1 3��CIF��4��QCIF
    SDK_S32     snapShotImageType;  // 0 JPEG��1 bmp
    SDK_S32     storagerMode;       //�洢ģʽ(0:����, 1:FTP, 2: FTP|LOCAL(����FTP,FTPʧ�ܺ�¼�񱾵�))
    SDK_S32     channelID;          //ͨ����
} SDK_TIMER_SNAP;

/*�¼�ץͼ*/
typedef struct {
	SDK_S32     enable;
	SDK_S32     interval;           //ץ�ļ��ms
	SDK_S32     nums;               // һ�μ���
	SDK_S32     pictureQuality;     // ����ֵ 0- 100 ��100% ���
	SDK_S32     imageSize;          // �����С
	SDK_S32     snapShotImageType;  // 0 JPEG��1 bmp
	SDK_S32     storagerMode;       //�洢ģʽ(0:����
	SDK_S32     channelID;          //ͨ����
}SDK_EVENT_SNAP;

typedef struct {
	SDK_TIMER_SNAP timer_snap;
	SDK_EVENT_SNAP event_snap;
} SDK_NET_SNAP_CFG, *LPSDK_NET_SNAP_CFG;

extern int SnapCfgSave();
extern int SnapCfgLoad();
extern void SnapCfgPrint();
extern int SnapCfgLoadDefValue();
extern cJSON * SnapCfgLoadJson();
extern int SnapJsonSaveCfg(cJSON *json);

#define SNAP_CFG_FILE "snap_cfg.cjson"


extern SDK_NET_SNAP_CFG runSnapCfg;
extern SDK_CFG_MAP snapTimerMap[];
extern SDK_CFG_MAP snapEventMap[];

#ifdef __cplusplus
}
#endif
#endif

