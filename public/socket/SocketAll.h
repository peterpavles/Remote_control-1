#pragma once
#include <WS2tcpip.h>
#include "Socket.h"
#pragma comment(lib,"Ws2_32.lib")

namespace MOUSE {
    enum {
        LBUTTONDOWN  = 0,  //鼠标左键按下
        LBUTTONUP    = 1,    //鼠标左键抬起
        RBUTTONDOWN  = 2,  //鼠标右键按下
        RBUTTONUP    = 3,    //鼠标右键抬起
        LBUTTONDOUBLE= 4,//鼠标左键双击
        RBUTTONDOUBLE= 5,//鼠标右键双击
        MOUSEMOVE    = 6,    //鼠标移动
    };
}

namespace KEY {
    enum {
        KEYDOWN = 0,  //键盘按下
        KEYUP   = 1,    //键盘抬起
    };
}

namespace CMD {
    enum {
        PRINTSCREEN     = 0,//注册命令
        STOPPRINTSCREEN = 1,//获取屏幕命令
        MOUSEEVENT      = 2,//鼠标事件命令
        KEYEVENT        = 3, //键盘事件命令
        STARTCMD        = 4,//启动命令行
        STOPCMD         = 5,//结束命令行
        CMDLINE         = 6,//命令行事件
        CMDCTRL         = 7,//命令行组合键
        FILEDIR         = 8, //获取文件目录命令
        DISKINFO        = 9,//获取磁盘信息
        DOWNFILE        = 10,//下载文件
        UPFILE          = 11,//上传文件
    };
}

typedef struct FILE_INFO {
    char type[16];
    char date[20];
    long long size;
    char name[1];
}File_Info;

typedef struct DOWN_FILE {
    int curSize;
    int dataSize;
    char data[1];
}Down_File;

typedef struct DISK_INFO {
    char disk[6];
    float total;
    float available;
    float free;
    char type[16];
}Disk_Info;

typedef struct CMD_LINE {
    char data[1];
}Cmd_Line;

typedef struct KEYS {
    int keyLeg;
    UINT data;
}KeysEvent;

typedef struct MOUSR {
    int mouseLeg;
    int x;
    int y;
}MouseEvent;

typedef struct SCREEN_INFO {
    int width;
    int height;
    int dataSize;   //压缩前的大小
}Screen_Info;

typedef struct SERVER_HEAD {
    uint16_t cmd;
    uint64_t dataLen;
}Server_Head;


