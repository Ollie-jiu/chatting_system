
#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <graphics.h>//Easy.x
SOCKET connect_sock;
HWND draw;
int count_num;
int count;
#define DEFAULT_PORT	5019
void recevieData() {
	char recvBuff[1024];
	int temp_msg;
	while (1) {
		temp_msg = recv(connect_sock, recvBuff, 1023, 0);
		if (temp_msg == SOCKET_ERROR) {
			fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
			closesocket(connect_sock);
			WSACleanup();
			return;
		}
		if (temp_msg == 0) {
			printf("server closed connection\n");
			closesocket(connect_sock);
			WSACleanup();
			return;
		}
		if (temp_msg > 0) {
			recvBuff[temp_msg] = 0;
			outtextxy(0, count_num * 20, recvBuff);
			count_num++;
		}
	}
}
int main(int argc, char **argv){
	draw = initgraph(300, 400, SHOWCONSOLE);
	char szBuff[1024];
	int msg_len;
	//int addr_len;
	struct sockaddr_in server_addr;
	struct hostent *hp;

	WSADATA wsaData;

	char			*server_name = "localhost";
	unsigned short	port = DEFAULT_PORT;
	unsigned int	addr;

	if (argc != 3){
		printf("echoscln [server name] [port number]\n");
		return -1;
	}
	else{
		server_name = argv[1];
		port = atoi(argv[2]);
	}

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR){
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}

	if (isalpha(server_name[0]))// if the first is a letter;
		hp = gethostbyname(server_name);
	else{
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	if (hp==NULL)
	{
		fprintf(stderr, "Cannot resolve address: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//copy the resolved information into the sockaddr_in structure
	memset(&server_addr, 0, sizeof(server_addr));
	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = hp->h_addrtype;
	server_addr.sin_port = htons(port);

	
	connect_sock = socket(AF_INET,SOCK_STREAM, 0);	//TCp socket


	if (connect_sock == INVALID_SOCKET){
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Client connecting to: %s\n", hp->h_name);
	if (connect(connect_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		fprintf(stderr, "connect() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	CreateThread(NULL, NULL,(LPTHREAD_START_ROUTINE)recevieData,NULL, NULL, NULL);
	while (1) {
		/************************************************/
		printf("input character string:\n");
		gets_s(szBuff);
		/*for (int i = 0;i < strlen(szBuff);i++) {
			if (szBuff[i] == '\n') {

				szBuff[i] == ' ';
			}
		}*/
		msg_len = send(connect_sock,szBuff, 1023, 0);
		if (msg_len == SOCKET_ERROR) {
			fprintf(stderr, "send() failed with error........ %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		if (msg_len == 0) {
			printf("server closed connection\n");
			closesocket(connect_sock);
			WSACleanup();
			return -1;
		}
	}//it needs sent messages all the time;
	closesocket(connect_sock);
	WSACleanup();
}