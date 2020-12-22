// LiveClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Created on Sat Dec 21 01:26:59 2020

//@author: 尹立庆
//@wechat： 13521526165

// include "pch.h"//VS2019删掉改行
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

// using namespace std;    // 不能使用std命名空间，与opencv2模块中有同名函数bind，代码中不能使用cout，使用printf代替
using namespace cv;

void initialization();

int main()
{
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	const int BUFFER_SIZE = 1228802;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	char *recv_buf= new char[BUFFER_SIZE];
	int text_len = 921600;
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(5027);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("套接字绑定失败！\n");
		WSACleanup();
	}
	else {
		printf("套接字绑定成功！\n");
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		printf("设置监听状态失败！\n");
		WSACleanup();
	}
	else {
		printf("设置监听状态成功！\n");
	}
	printf("服务端正在监听连接，请稍候....\n");
	//接受连接请求
	len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &len);
	if (s_accept == SOCKET_ERROR) {
		printf("连接失败！\n");
		WSACleanup();
		return 0;
	}
	printf("连接建立，准备接受数据！\n");

	// 显示视频
	//从摄像头读取视频
	// VideoCapture capture(0);
	//循环显示每一帧
	Mat frame(480, 640, CV_8UC3);//定义一个Mat变量，用于存储每一帧的图像，8个二进制位无符号，3个通道
	// frame.rows = 480;
	// frame.cols = 640;
	// frame.channels = 3;
	
	// capture >> frame;//读取当前帧
	// capture.release();

	uint8 *decode = new uint8[text_len];

	// 回复接收状态
	send_buf[0] = 'O'; send_buf[1] = 'K'; send_buf[2] = 0;
	//接收数据
	while (1) {
		recv_len = recv(s_accept, recv_buf, BUFFER_SIZE, 0);
		/*
		while (recv_len > 0) {
			cout << "客户端信息:" << recv_buf << endl;
			// recv_len = recv(s_accept, recv_buf, BUFFER_SIZE, 0);
		}
		*/
		if (recv_len < 0) {
			printf("接收失败！\n");
			break;
		}
		else {
			// cout << "客户端信息:" << recv_buf << endl;

			// printf("客户端信息:%s\n", recv_buf);
			long size = base64_decode((uint8 *)recv_buf, recv_len, decode);
			decode[size] = 0;
			printf("size: %d\n", size);
			frame.data = decode;

			imshow("读取视频帧LiveClient", frame);//显示当前帧
			waitKey(300);//延时300ms
		}
		
		// cout << "请输入回复信息:";
		// cin >> send_buf;
		
		send_len = send(s_accept, send_buf, 100, 0);
		if (send_len < 0) {
			printf("发送失败！\n");
			break;
		}
		else {
			printf("发送成功！\n");
		}
	}
	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
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
		printf("初始化套接字库失败！\n");
	}
	else {
		printf("初始化套接字库成功！\n");
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		printf("套接字库版本号不符！\n");
		WSACleanup();
	}
	else {
		printf("套接字库版本正确！\n");
	}
	//填充服务端地址信息

}
