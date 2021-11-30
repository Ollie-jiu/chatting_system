
#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<mysql.h>
#include<process.h>
#define DEFAULT_PORT	5019

int addr_len;
int count_num;
struct sockaddr_in client_addr;
SOCKET sock, msg_sock[100];
void accept_conn(int c) {
	char szBuff[1024];
	int temp_msg;
	while (1) {
		temp_msg = recv(msg_sock[c], szBuff, 1023, NULL);//the range of 1023 is to make sure each its size can't be too small; 
		if (temp_msg == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return ;
		}
		if (temp_msg == 0) {
			printf("Client closed connection\n");
			closesocket(msg_sock[c]);
			return ;
		}
		if (temp_msg > 0) {
			szBuff[temp_msg] = 0;
			printf("user£º%d:Bytes Received: %d, message: %s from %s\n", c,temp_msg, szBuff, inet_ntoa(client_addr.sin_addr));
			for (int i = 0; i < count_num; i++) {
				int msg_len = send(msg_sock[i], szBuff, strlen(szBuff), NULL);
				if (msg_len == 0) {
					printf("Client closed connection\n");
					closesocket(msg_sock[i]);
					return;
				}
			}

		}
		
	}
	closesocket(msg_sock[c]);
}
int main(int argc, char **argv){
	struct sockaddr_in  local;
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR){
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}
	// Fill in the address structure
	local.sin_family		= AF_INET;
	local.sin_addr.s_addr	= INADDR_ANY;
	local.sin_port		= htons(DEFAULT_PORT);

	sock = socket(AF_INET,SOCK_STREAM, 0);	//TCp socket


	if (sock == INVALID_SOCKET){
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR){
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//waiting for the connections
	if (listen(sock, 5) == SOCKET_ERROR){
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	
	printf("Waiting for the connections ........\n");		
	while(true){
		addr_len = sizeof(client_addr);
		msg_sock[count_num] = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		if (msg_sock[count_num] == INVALID_SOCKET) {
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		else {
			printf("accepted connection from %s, port %d\n",
				inet_ntoa(client_addr.sin_addr),
				htons(client_addr.sin_port));
		}
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)accept_conn,(char*)count_num, NULL, NULL);
		count_num++;
		}
	WSACleanup();
}
		