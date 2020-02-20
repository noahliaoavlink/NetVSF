#pragma once

#ifndef _NetCommon_H_
#define _NetCommon_H_

#define _MAX_BUFFER_LEN 65536 * 2
#define _MAX_RCV_BUFFER 65536 * 2
#define _MAX_SEND_BUFFER 65536 * 2
#define _MAX_LINE_BUFFER 2048

#define _SPLIT_SIZE 65000 * 2//40960//16384//4096
#define _VIDEO_BUFFER_SIZE 4096 * 2048 * 2

#define _ENABLE_GL 1

enum PacketType
{
	PT_CMD_ONLY = 0,
	PT_SIMPLE,
	PT_STRING,
	PT_STRING2,
	//PT_SHAPE_LIST,
	PT_LONG,
	//PT_UD_SHAPE_LIST,
	PT_VSF_VIDEO_PACKET,
	PT_VSF_SETUP,
	//PT_FILE,
};

enum NetCommandID
{
	//c -> s
	NID_LOGIN = 101,
	NID_SETUP,
	NID_PLAY,
	NID_STOP,
	NID_HELLO,
	NID_UPDATE_TIMESTAMP,

	//s -> c
	NID_LOGIN_RET = 1001,
	NID_SETUP_RET,
	NID_VIDEO_PACKET,
};

enum LoginResult
{
	LR_FAILED = 0,
	LR_SUCESSED,
};

enum VSFStatus
{
	VSFS_FAIL = -1,
	VSFS_STOP = 0,
	VSFS_SETUP,
	VSFS_PLAY,
};

enum CodecType
{
	CT_NONE = 0,
	CT_JPEG,
	//CT_H264,
};

enum ClientCallbackEvent
{
	CCE_DISPLAY_VIDEO_FRAME = 201,
};

typedef struct
{
	unsigned char ucVersion;
	unsigned char ucMarker;
	unsigned short usSequenceNumber;
	unsigned long ulLength;
	unsigned char* pBuf;
}VSFVideoPacket;

/*
<NP + packet_type(1) + cmd_id(4) + data_len(4) + data(?) + NP>

packet_type 0 - cmd only
packet_type 1 - cmd + simple data (ex. int )
packet_type 2 - cmd + string data(len + data)
packet_type 3 - cmd + string1(len + data) ¡Ï string2(len + data)

login , logout , start , stop , pause , update_time_code
*/

#endif