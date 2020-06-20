#pragma once
#include <WS2tcpip.h>
#include "Socket.h"
#pragma comment(lib,"Ws2_32.lib")

namespace MOUSE {
    enum {
        LBUTTONDOWN  = 0,  //����������
        LBUTTONUP    = 1,    //������̧��
        RBUTTONDOWN  = 2,  //����Ҽ�����
        RBUTTONUP    = 3,    //����Ҽ�̧��
        LBUTTONDOUBLE= 4,//������˫��
        RBUTTONDOUBLE= 5,//����Ҽ�˫��
        MOUSEMOVE    = 6,    //����ƶ�
    };
}

namespace KEY {
    enum {
        KEYDOWN = 0,  //���̰���
        KEYUP   = 1,    //����̧��
    };
}

namespace CMD {
    enum {
        PRINTSCREEN     = 0,//ע������
        STOPPRINTSCREEN = 1,//��ȡ��Ļ����
        MOUSEEVENT      = 2,//����¼�����
        KEYEVENT        = 3, //�����¼�����
        STARTCMD        = 4,//����������
        STOPCMD         = 5,//����������
        CMDLINE         = 6,//�������¼�
        CMDCTRL         = 7,//��������ϼ�
        FILEDIR         = 8, //��ȡ�ļ�Ŀ¼����
        DISKINFO        = 9,//��ȡ������Ϣ
        DOWNFILE        = 10,//�����ļ�
        UPFILE          = 11,//�ϴ��ļ�
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
    int dataSize;   //ѹ��ǰ�Ĵ�С
}Screen_Info;

typedef struct SERVER_HEAD {
    uint16_t cmd;
    uint64_t dataLen;
}Server_Head;


