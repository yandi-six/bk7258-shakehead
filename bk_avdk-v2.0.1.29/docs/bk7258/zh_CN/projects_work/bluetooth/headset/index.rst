耳机(Headset)
======================================

:link_to_translation:`en:[English]`

1 功能概述
-------------------------------------

	用于耳机等设备场景，开启a2dp sink、avrcp ct/tg、ble等feature。

2 代码路径
-------------------------------------

    Demo路径：`./projects/bluetooth/headset <https://gitlab.bekencorp.com/wifi/armino/-/tree/main/projects/bluetooth/headset>`_

	编译命令：``make bk7258 PROJECT=bluetooth/headset``

3 cli 命令
-------------------------------------
    连接后可以使用cli命令测试avrcp等功能。注意，某些命令是否生效视手机、手机app而定)

    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=play                       | 播放                |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=pause                      | 暂停                |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=next                       | 下一曲              |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=prev                       | 上一曲              |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=rewind[,msec]              | 快退，可指定时间    |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=fast_forward[,msec]        | 快进，可指定时间    |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=vol_up                     | 音量增加            |
    +-------------------------------------------+---------------------+
    | AT+BTAVRCPCTRL=vol_down                   | 音量减少            |
    +-------------------------------------------+---------------------+
    | AT+BTENTERPAIRINGMODE                     | 进入配对模式        |
    +-------------------------------------------+---------------------+
