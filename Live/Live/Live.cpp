// Live.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Created on Sat Dec 21 01:26:59 2020

//@author: 尹立庆
//@wechat： 13521526165

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

#include "base64.h"
// DLL实现
// CMakefile
// 抓屏软件
// 摄像头视频存储
// 语音录制与传输
// 视频直播软件，一人直播，多人观看，连麦，视频会议
// 视频中显示字
// 弹幕
// 配置文件

using namespace std;
using namespace cv;

void initialization();

int main(void)
{


	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	char recv_buf[100];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(5027);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "服务器连接失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "服务器连接成功！" << endl;
	}


	// ----------------------------------------------

	//从摄像头读取视频
	VideoCapture capture(0);
	//循环显示每一帧
	Mat frame;//定义一个Mat变量，用于存储每一帧的图像

	capture >> frame;//读取当前帧
	char* pos = (char*)frame.data;
	printf("channels: %d\n", frame.channels());
	int colByte = frame.cols*frame.channels() * sizeof(uchar);
	uint8 *text = (uint8 *)pos;
	// uint32 text_len = (uint32)strlen((char *)text);
	uint32 text_len = (uint32)(frame.rows * colByte);

	uint32 buffer_size = (uint32)(text_len + 3) / 3 * 4; // 加6保持申请的内存大于三分之四倍
	printf("text_len: %d\n", text_len);
	printf("buffer_size: %d\n", buffer_size);

	uint8 *decode = new uint8[text_len];
	uint8 *encode = new uint8[buffer_size];
	while (1)
	{
		capture >> frame;//读取当前帧
		char* pos = (char*)frame.data;
		text = (uint8 *)pos;

		uint32 size = base64_encode(text, text_len, encode);
		encode[size] = 0;
		// printf("%s\n", encode);
		printf("size: %d\n", size);
		// 上传摄像头数据

		send_len = send(s_server, (char*)encode, size, 0);
		if (send_len < 0) {
			cout << "发送失败！" << endl;
			break;
		}
		recv_len = recv(s_server, recv_buf, 100, 0);
		if (recv_len < 0) {
			cout << "接收失败！" << endl;
			break;
		}
		else {
			cout << "服务端信息:" << recv_buf << endl;
		}

		size = base64_decode(encode, size, decode);
		decode[size] = 0;
		printf("size: %d\n", size);
		frame.data = decode;

		imshow("读取视频帧Live", frame);//显示当前帧
		waitKey(30);//延时30ms
	}

	system("pause");
	//关闭套接字
	closesocket(s_server);
	//释放DLL资源
	WSACleanup();
	return 0;
}

void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}
	//填充服务端地址信息

}

