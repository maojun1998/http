//**********************************************//
//              WGET wget tools                 //
//                                              //
//      Base Windows                            //
//      $Author Hacking                         //
//                                              //
//**********************************************//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock2.h>
#include <windows.h>
#include <ctype.h>
#include <assert.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "dnsapi.lib")

#include "dns.h"
#include "wget.h"

char *defaultPort = "80";

void useage();
char *URL_AZ(char *URL);
char *host(char *URL);
char *filename(char *URL);
char *getPort(char *URL);
char *file(char *fullpath);
void do_save(char *URL);
void WriteFileO(HANDLE hFile, SOCKET clinentSocket, PParameterInfo pParam);
void WritePrice(char *name ,int number, float speed, int times);
char fileNameOk[256];

int main(int argc, char *argv[])
{
    char *az;

    if (argc != 3) {
        useage();
        return 0;
    }
    
    if (strcmp(argv[1], "-c")) {
        useage();
        return 0;
    }
    /*
    az = URL_AZ(argv[2]);
    printf("az: %s\n", az);
    printf("host : %s\n", host(az));
    printf("fileName : %s\n", filename(az));
    printf("port : %s\n", getPort(az));
    printf("file : %s\n", file(filename(az)));
    
    printf("ip:%s\n", DnsQueryIp(host(az)));
    */
    
    do_save(argv[2]);
    
    
    return 0;
}
void useage()
{
    printf( "wget(0.11) , stable version.                 \n"
            "Copyright (C) HACKING.  All rights reserved.\n\n"
            "   useage :                                  \n"
            "            wget -c URL                    \n\n");
}
char *URL_AZ(char *URL)
{
    int j, i;
    
    char *buffer = malloc(512);
    buffer[0] = 0;
    j = 0;
    if (URL[0] == 'h' && URL[1] == 't' && URL[2] == 't' &&
        URL[3] == 'p' && URL[4] == ':' && URL[5] == '/' &&
        URL[6] == '/') {
        i = 7;
    } else {
        i = 0;
    }
    for (; i < strlen(URL); i++) {
        if (URL[i] != ' ' && URL[i] != '&' && URL[i] != '=' && URL[i] != '\?') {
            buffer[j++] = URL[i];
        } else {
            break;
        }
   }

   buffer[j] = 0;

#ifdef DBG
    printf("URL : %s, size:%d\n", buffer, strlen(buffer));
#endif
   if (buffer[0] == 0) {
       return NULL;
   }
   
   return buffer;
}
char *host(char *URL)
{
    int i,j;
    char *hb;
    hb = malloc(512);
    hb[0] = 0;
    
    for (i = 0; i < strlen(URL); i++) {
        if (URL[i] == ':' || URL[i] == '/') {
            break;
        }
    }
    for (j = 0; j < i; j++) {
        hb[j] = URL[j];
    }
    hb[j] = 0;
    
    return hb;
    
}
char *filename(char *URL)
{
    char *buffer;
    int i,j, k;
    
    i = 0;
    j = 0;
    k = 0;
    
    buffer = (char *)malloc(512);
    buffer[0] = 0;
    
    for (i = 0; i < strlen(URL); i++) {
        if (URL[i] == '\?' || URL[i] == '=' || URL[i] == '&' || URL[i] == ' ') {
            if (j == 0) {
                j = i;
            }
        }
        if (URL[i] == '/') {
            if (k == 0) {
                k = i;
            }
        }
    }
    //printf("j:%d, k:%d\n", j, k);
    
    if (j == 0) {
        j = i;
    }
    if (k == 0) {
        k = i;
    }
    i = 0;
    while (k <= j) {
        buffer[i++] = URL[k++];
    }
    buffer[i] = 0;
    
    if (buffer[0] == 0) {
        strcat(buffer, "/index.html");
        return buffer;
    }
    if (buffer[strlen(buffer) - 1] == '/') {
        strcat(buffer, "index.html");
    }
    return buffer;
    
}
char *getPort(char *URL)
{
    int i,j;
    char flag;
    char *port = malloc(512);
    flag = 0;
    port[0] = 0;
    
    for (i = 0; i < strlen(URL); i++) {
        if (URL[i] == ':') {
            break;
        }
    }
    if (i == strlen(URL)) {
        return defaultPort;
    }
    
    i++;
    j = 0;
    for (; i < strlen(URL); i++) {
        if (isdigit(URL[i])) {
            port[j++] = URL[i];
        } else {
            break;
        }
    }
    port[j] = 0;
    return port;
}
char *file(char *fullpath)
{
    int i;
    int j;
    int k;
    char *buffer;
    
    buffer = malloc(512);
    assert(buffer);
    
    for (i = 0; i < strlen(fullpath); i++) {
        if (fullpath[i] == '/') {
            k = i;
        }
    }
    j = 0;
    k++;
    
    while(k < strlen(fullpath)) {
        buffer[j++] = fullpath[k++];
    }
    buffer[j] = 0;
    
    return buffer;
}
PHTTP_REQUEST
WINAPI
CreateHttpRquest (
    VOID
    )
{
    PHTTP_REQUEST pHttpRequst;
    
    pHttpRequst = (PHTTP_REQUEST )malloc(sizeof(HTTP_REQUEST));
    
    
    if (pHttpRequst == NULL) {
        return NULL;
    }
    pHttpRequst->sendBuff = malloc(MAX_BUFFER_SIZE);
    
    pHttpRequst->socket = INVALID_SOCKET;

    return pHttpRequst;
    
}
    
PHTTP_RESPONS
WINAPI
CreateHttpRespons (
    VOID
    )
{
    PHTTP_RESPONS pHttpRespons;
    pHttpRespons = (PHTTP_RESPONS)malloc(sizeof(HTTP_RESPONS));
    
    if (pHttpRespons == NULL) {
        return NULL;
    }
    pHttpRespons->recvBuff = malloc(MAX_BUFFER_SIZE);
    pHttpRespons->size = 0;
    
    return pHttpRespons;
}
void do_save(char *URL)
{
    PHTTP_REQUEST pHttpRequst;
    PHTTP_RESPONS pHttpRespons;
    char *az;
    BOOL status;
    short portNo;
    char *ip;
    char *fileN;
    char *port;
    HANDLE hFile;
    SOCKET clinentSocket;
    WORD wVersionRequested;
    WSADATA wsaData;
    int error;
    SOCKADDR_IN sockaddr;
    OVERLAPPED overlapped;
    WSAOVERLAPPED wsaOverlapped;
    WSABUF wsabuff;
    char *buffer;
    char *recvBuff;
    PParameterInfo pParam;
    DWORD nWrite;
    
    wVersionRequested = MAKEWORD(1, 1);
    
    error = WSAStartup(wVersionRequested, &wsaData);
    if (error != 0) {
        return ;
    }
    if (LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1) {
        WSACleanup();
        return;
    }
    memset(&overlapped, 0, sizeof(overlapped));
    memset(&wsaOverlapped, 0, sizeof(wsaOverlapped));

    

    az = URL_AZ(URL);

#ifdef DBG
    printf("az : %s\n", az);
    printf("fileName : %s\n", filename(az));
    printf("file : %s\n", file(filename(az)));
#endif 
    fileN = file(filename(az));
    port = getPort(az);
    portNo = (short)atoi(port);
    
    if (portNo < 1) {
        printf("Port %s is error\n", portNo);
        return ;
    }
    ip = DnsQueryIp(host(az));

    if (ip == NULL) {
        printf("Host %s DNS ERROR\n", host(az));
        return;
    }
    
    pHttpRequst = CreateHttpRquest();
    pHttpRespons = CreateHttpRespons();
    
    Construct(  host(az),
                filename(az),
                getPort(az),
                pHttpRequst,
                pHttpRespons);
    
    clinentSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (clinentSocket == INVALID_SOCKET) {
        printf("Create Socket Error : %d\n", WSAGetLastError());
        return;
    }
    sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(portNo);
    
    error =  connect(clinentSocket, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));
    
    if (error == SOCKET_ERROR) {
        printf("connect error:%d\n", WSAGetLastError());
        return;
    }
    buffer = pHttpRequst->sendBuff;
    recvBuff = pHttpRespons->recvBuff;
    
    error = send(clinentSocket, buffer, pHttpRequst->size, 0);
    if (SOCKET_ERROR == error) {
        printf("send request error:%d\n", WSAGetLastError());
        return;
    }
    printf("Connetting  to %s:%s... connected.\n", host(az), getPort(az));
    printf("HTTP request sent, awaiting response...");
    
    error = recv(clinentSocket, buffer, MAX_BUFFER_SIZE, 0);
    if (error == SOCKET_ERROR) {
        printf("recv request error:%d\n", WSAGetLastError());
        return;
    }
    pParam = AnlyzeThePack(buffer, error);
    if (pParam == NULL) {
        printf("pParam == NULL\n");
        return;
    }
    if (pParam->statusCode == 0) {
        printf("%s\n", ERROR);
        return ;
    } else if (pParam->statusCode != 200) {
        printf("%d\n", pParam->statusCode);
        return ;
    } else {
        printf("200\n");
    }
    printf("Length: %d\n\n\n", pParam->size);
#ifdef DBG
    printf("recv size :%d, index:%d\n", error, pParam->index);
#endif 

    printf("Saving to: \'%s\'\n", file(filename(az)));
    fileNameOk[0] = 0;
    
    strcat(fileNameOk , fileN);
    hFile = CreateFile (fileN,
                        GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        0,
                        NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Open File %s FALSE, ErrorNo %d\n", fileN, GetLastError());
        return ;
    }
    if (pParam->index < error) {
        status = WriteFile (hFile,
                            &buffer[pParam->index],
                            error - pParam->index,
                            &nWrite,
                            NULL);
        if (status == 0) {
            printf("Write File FALSE :%d", GetLastError());
            return ;
        }
        pParam->hasWrite = nWrite;
    }
#ifdef  DBG
    printf("len :%d\n", pParam->size);

#endif 
    WriteFileO(hFile, clinentSocket, pParam);
    
    return ;
}
BOOL
Construct (
    char *host, 
    char *fullpath, 
    char *port,
    PHTTP_REQUEST pHttpRequst,
    PHTTP_RESPONS pHttpRespons
    )
{
    char *ip;
    short portNo;
    char TEMP[3] = {0x0d, 0x0a, 0x00};
    int len;

    //ip = DnsQueryIp(host);
    /*
    if (ip == NULL) {
        printf("Host : %s is Exits!!!\n", host);
        return FALSE;
    }*/

    len = sprintf(  pHttpRequst->sendBuff,
                    "GET %s HTTP/1.1%s"
                    "Host: %s:%s%s"
                    "Connection: close%s"
                    "User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.0; Trident/5.0)%s"
                    "Accept: text/html, application/xhtml+xml, */*%s%s",
                    fullpath,
                    TEMP,
                    host,
                    port,
                    TEMP,
                    TEMP,
                    TEMP,
                    TEMP,
                    TEMP
                );
    pHttpRequst->size = len;
    
#ifdef  DBG
    printf("%s\n", pHttpRequst->sendBuff);
#endif
    return TRUE;
}

PParameterInfo
AnlyzeThePack (
    char *buffer,
    int size
    )
{
    PParameterInfo pParam;
    char buffer1[1024];
    char buffer2[512];
    char buffer3[512];
    int index;
    int number;
    int flag;
    int j;
    int i;
    int k;
    int l;
    int contentLen;
    
    
    pParam = (PParameterInfo)malloc(sizeof(ParameterInfo));
    
    memset(pParam, 0, sizeof(ParameterInfo));
    
    
    if (size < 13) {
        free(pParam);
        return NULL;
    }
    if (buffer[0] == 'H' && buffer[1] == 'T' && buffer[2] == 'T' && buffer[3] == 'P' &&
        buffer[4] == '/' && buffer[5] == '1' && buffer[6] == '.' && buffer[7] == '1' &&
        buffer[8] == ' ') {
        if (isdigit(buffer[9]) && isdigit(buffer[10]) && isdigit(buffer[11])) {
            number = (buffer[9] - '0') * 100 + (buffer[10] - '0') * 10 + (buffer[11] - '0');
            
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    pParam->statusCode = number;
    if (number != 200) {
        return pParam;
    }
#ifdef  DBG
    printf("line : %d, function : %s\n", __LINE__, __FUNCTION__);
#endif 
    for (i = 12; i < size; i++) {
        if (buffer[i] == 0x0d && buffer[i + 1] == 0x0a) {
            i += 2;
            break;
        }
    }
#ifdef  DBG
    printf("line : %d, function : %s\n", __LINE__, __FUNCTION__);
#endif 
    j = i;
    buffer1[0] = 0;
    
    for (; i < size; i++) {
        if (buffer[i] == 0x0d && buffer[i + 1] == 0x0a) {
            buffer1[0] = 0;
            for (l = 0, k = j; k < i; k++) {
                buffer1[l++] = buffer[k];
            }
            j = i + 2;
            buffer1[l] = 0;
#ifdef  DBG
            printf("*** buferr1:%s\n", buffer1);
#endif 
            i += 1;
            if (buffer1[0] == 0) {
                i += 1;
                break;
            }
            if ((contentLen = isConnetLen(buffer1)) != -1) {
                pParam->size = contentLen;
                
            }
        }

    }
    //printf("i : %d\n", i);
    pParam->index = j;
    
    assert(j <= size);
    
    return pParam;
    
}

int
isConnetLen (
    char *buffer
    )
{
    int i,j;
    int number;
    char buffer1[1024];
    j = 0;
#ifdef  DBG
    printf("int buffer : %s\n", buffer);
#endif 
    for (i = 0; i < strlen(buffer); i++) {
        if (buffer[i] != ':') {
            buffer1[j++] = buffer[i];
        } else {
            break;
        }
    }
    if (i == strlen(buffer)) {
        return -1;
    }
    buffer1[j] = 0;
    if (strcmp(buffer1, "Content-Length")) {
        return -1;
    }
#ifdef DBG
        printf("key :%s\n", buffer1);
#endif 
/*
    if (buffer1[i + 1] != ' ') {
#ifdef  DBG
        printf("line : %d, function : %s\n", __LINE__, __FUNCTION__);
#endif
        return -1;
    }*/
    
    i += 2;
    number = 0;
    number = atoi(&buffer[i]);
    
    
    if (number > 0) {
        return number;
    } else {
        return -1;
    }
}
void WriteFileO(HANDLE hFile, SOCKET clinentSocket, PParameterInfo pParam)
{
    char *buffer;
    int hasWrite;
    int sum;
    int needWrite;
    int len;
    int status;
    DWORD nwrite; 
    int old;
    int now;
    DWORD timeBegin;
    DWORD timeOld;
    DWORD timeNow;
    DWORD timeEnd;
    DWORD oldHasRead;
    float speed;
    
    buffer = malloc(MAX_BUFFER_SIZE);
    
    assert(buffer);
    
    sum = pParam->size;
    hasWrite = pParam->hasWrite;
    needWrite = sum - hasWrite;
    old = 0;
    
    timeBegin = GetTickCount();
    timeNow = timeBegin;
    timeOld = timeBegin; 
    oldHasRead = hasWrite;
 
    
    
    while (needWrite > 0) {
        len = recv(clinentSocket, buffer, MAX_BUFFER_SIZE, 0);
        if (len == SOCKET_ERROR) {
            printf("recv error:%d\n", WSAGetLastError());
            return ;
        }
        status = WriteFile (hFile,
                            buffer,
                            len,
                            &nwrite,
                            NULL);
        if (status == 0) {
            printf("write file error :%d\n", GetLastError());
            return ;
        }
 #ifdef DBG
        printf("number write ,len :%d,nWrite: %d\n", len, nwrite);
 #endif 
        
        hasWrite += len;
        now = (int)((double)hasWrite / (double)sum * 100);
        timeNow = GetTickCount();
        //printf("write : %d\n", hasWrite * 100 / sum);
        if (now - old > 0) {
            old = now;
            speed = (float)(hasWrite - oldHasRead) / (float)(timeNow - timeOld) * 1000.0 / 1024.0; 
            WritePrice(fileNameOk, now , speed, timeNow - timeBegin);
            oldHasRead = hasWrite;
            timeOld = timeNow;
        }
        
        needWrite = sum - hasWrite;
    }
    timeEnd = GetTickCount();
    
    printf( "\n\nAll using  %.1fs, average speed %.1f kb/s\n", 
            ((float)timeEnd - (float)timeBegin) / 1000, 
            (float)(sum) / ((float)timeEnd - (float)timeBegin) * 1000.0 / 1024.0 );
            
    CloseHandle(hFile);
    closesocket(clinentSocket);
    
}
void WritePrice(char *name ,int number, float speed, int times) 
{
    HANDLE hStdin;
    CONSOLE_SCREEN_BUFFER_INFO conBufferInfo;
    COORD pos;
    
    
    int i, j;
    short sizeWindow;
    int s = 30;
    
    hStdin = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (hStdin == INVALID_HANDLE_VALUE) {
        ExitProcess(0);
    }
    GetConsoleScreenBufferInfo(hStdin, &conBufferInfo);
    pos.X = conBufferInfo.dwCursorPosition.X;
    pos.Y = conBufferInfo.dwCursorPosition.Y;
    sizeWindow = conBufferInfo.dwSize.Y;
    
    pos.X = 0;
    
    pos.Y = pos.Y - 1;
    SetConsoleCursorPosition(hStdin, pos);
    
    
    printf( "                    "
            "                    "
            "                    "
            "                    ");
 

    SetConsoleCursorPosition(hStdin, pos);
    
    printf("%-20s[", name);
    
    for (i = 0; i < number * 30 / 100 ;i++) {
        printf("=");
    }
    printf(">");
    
    while (i < 30) {
        printf(" ");
        i++;
    }
    printf("]");
    
    fprintf(stdout ,"  %4d%s  speed: %10.1f kb/s, in  %8.3f s\n", number, "%", speed ,(float)times / (float) 1000);
    
    
}

