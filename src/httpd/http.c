//**********************************************//
//              HTTP SERVER                     //
//                                              //
//      Using Windows IOCP                      //
//      $Author Hacking                         //
//                                              //
//**********************************************//

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <winsock2.h>
#include <windows.h>



#include "http.h"
#include "hash.h"

//
// usage : http -p port -root path
//     example : http -p 80 -root C:\\Users\\htttp
//
char ROOT[256];

static BOOL CheckRootExist(char *root);
static void ShowUseage();
static BOOL Http_Server_Init(short port);
static void Http_Server_Stop();
static SOCKET listenSocket;
static void HttpServerWorker();
static DWORD RequestParmeterAnlyze(PKey_Struct pkey);
static DWORD BuildErrorPage(int erroCode, char *message, char *outBuff);
static VOID BuildPage(PKey_Struct pKey, char *body, int len, int erroCode);
static HANDLE GetFileHandle(char *path);
static VOID BuildRequestURL(char *base,char *URL);
static char *GetFileTypeDoc(char *type);
static VOID SendHttpHeader(PKey_Struct pKey, char *path);


static HANDLE ThreadList[64];
HANDLE hIocp;

int main(int argc, char *argv[])
{
    short port;
    char *path;
    int i;
    if (argc != 5) {
        ShowUseage();
        return 0;
    }
    if (strcmp(argv[1], "-p") || strcmp(argv[3],"-r")) {
        ShowUseage();
        return 0;
    }
    port = (short)atoi(argv[2]);
    
    if (port < 1) {
        ShowUseage();
        return 0;
    }
    if (!CheckRootExist(argv[4])) {
        printf("Dirctory %s is not exits\n\n", argv[4]);
        ShowUseage();
        return 0;
    }
    strcat(ROOT, argv[4]);
    
    if (ROOT[strlen(ROOT) - 1] == '\\')  {
        ROOT[strlen(ROOT) - 1] = 0;
    }
    
    HttpInit();
    
    Http_Server_Init(port);
    
    HttpServerWorker();
    
    return 0;
    
    
}
DWORD
WINAPI
GetSystemProcessNumber (
    VOID
    )
{
    SYSTEM_INFO systemInfo;
    
    (void)GetSystemInfo(&systemInfo);
    
    //printf("dwNumberOfProcessors : %d\n", systemInfo.dwNumberOfProcessors);
    
    return systemInfo.dwNumberOfProcessors * 2 + 2 > 64 ? 64 : systemInfo.dwNumberOfProcessors * 2 + 2;
    
}
static BOOL CheckRootExist(char *root)
{
    DWORD dwFile = GetFileAttributes(root);
    
    if (dwFile == -1) {
        return FALSE;
    }
    return (dwFile  &  FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
}

static void ShowUseage()
{
    
    printf( "\n    http -p port -r path\n\n"
            "     example : http -p 80 -r C:\\Users\\htttp\n");
    
}
static BOOL Http_Server_Init(short port)
{
    SOCKADDR_IN listenAddress;
    WORD wVersion;
    WSADATA wsaData;
    int err;
    
    
    listenAddress.sin_family = AF_INET;
    listenAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    listenAddress.sin_port = htons(port);
    
    wVersion = MAKEWORD(2, 2);
    err = WSAStartup(wVersion, &wsaData);
    
    if (err != 0) {
        printf("WSAStartup errno %d\n", err);
        return 0;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("cant's find version dll errno.\n");
        goto clean;
    }
    listenSocket = WSASocket(   AF_INET,
                                SOCK_STREAM,
                                IPPROTO_TCP,
                                NULL,
                                0,
                                WSA_FLAG_OVERLAPPED);
                                
    if (listenSocket == INVALID_SOCKET) {
        printf("INVALID_SOCKET , WSASocket error : %d\n", WSAGetLastError());
        goto clean;
    }
    err = bind(listenSocket, (SOCKADDR *)&listenAddress, sizeof(SOCKADDR));
    
    if (err != 0) {
        printf("bind error : %d\n", WSAGetLastError());
        goto cleanSocket;
    }
    err = listen(listenSocket, SOMAXCONN);
    if (err != 0) {
        printf("listen error :%d\n", err);
        goto cleanSocket;
    }
    hIocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE,
                                    NULL,
                                    (ULONG_PTR)NULL,
                                    GetSystemProcessNumber());
    if (hIocp == NULL) {
        printf("CreateIoCompletionPort GetLastError:%d\n", GetLastError());
        goto cleanSocket;
    }
    
    return TRUE;
    
cleanSocket:
    closesocket(listenSocket);
clean:
    WSACleanup();
    return FALSE;
}

#define MAX_TIMES   (10)

static void HttpServerWorker()
{
    PKey_Struct pKey;
    int err;
    SOCKET socket;
    HANDLE hTmp;
    BOOL status;
    DWORD times = 0;
    
    CreateThreadList(GetSystemProcessNumber());
    
    do {
        pKey = CreatePKS();
        if (pKey == NULL) {
            if (times < MAX_TIMES) {
                times++;
            } else {
                printf("Server Memory Out.\n\n");
                break;
            }
            continue;
        } else {
            times = 0;
        }
        socket = WSAAccept (listenSocket,
                            NULL,
                            NULL,
                            NULL,
                            (DWORD_PTR)NULL);
        if (socket == INVALID_SOCKET) {
            ClosePKS(pKey);
            continue;
        }
        pKey->socket = socket;
        
        hTmp = CreateIoCompletionPort ( (HANDLE)socket,
                                        hIocp,
                                        (ULONG_PTR)pKey,
                                        GetSystemProcessNumber());
        if (hTmp == INVALID_HANDLE_VALUE) {
            ClosePKS(pKey);
            ExitProcess(-1);
        }
        pKey->nextOp = NEXT_OPTYPE_INIT;
        
        status = PostQueuedCompletionStatus (   hIocp,
                                                0,
                                                (ULONG_PTR)pKey,
                                                &pKey->clientOp);
                                                
       if (status == FALSE) {
           ClosePKS(pKey);
           ExitProcess(-1);
       }
    } while (TRUE);
}
VOID
WINAPI
CreateThreadList (
    DWORD ThreadListNumber
    )
{
    int i;
    
    
    for (i = 0; i < ThreadListNumber; i++) {
        ThreadList[i] = CreateThread (  NULL,
                                        0,
                                        ThreadFunction,
                                        NULL,
                                        0,
                                        NULL);
        assert(ThreadList[i] != NULL);
    }
    
}

static
BOOL 
do_InitOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    );
static
BOOL 
do_SendOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    );
static
BOOL 
do_ReciveOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    );
static
BOOL 
do_ExitOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    );
static
BOOL 
do_ReadFileOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    );
    
DWORD 
WINAPI 
ThreadFunction (
    LPVOID lpParameter
    )
{
    int err;
    BOOL status;
    HANDLE htmp;
    DWORD ThreadId;
    DWORD size;
    PKey_Struct pKey;
    LPWSAOVERLAPPED lpOverlp;
    
    ThreadId = GetCurrentThreadId();
    do {
        status = GetQueuedCompletionStatus( hIocp,
                                            &size,
                                            (PULONG_PTR)&pKey,
                                            &lpOverlp,
                                            INFINITE);
    
        if (status == TRUE && HasOverlappedIoCompleted(lpOverlp)) {
            switch (pKey->nextOp) {
                case NEXT_OPTYPE_INIT : {
                    do_InitOp(pKey, size, lpOverlp);
                    break;
                }
                case NEXT_OPTYPE_SEND : {
                    do_SendOp(pKey, size, lpOverlp);
                    break;
                } 
                case NEXT_OPTYPE_RECIVE : {
                    do_ReciveOp(pKey, size, lpOverlp);
                    break;
                }
                case NEXT_OPTYPE_CLOSE : {
                    do_ExitOp(pKey, size, lpOverlp);
                    break;
                }
                case NEXT_OPTYPE_READ_FILE : {
                    do_ReadFileOp(pKey, size, lpOverlp);
                    break;
                }
                default : {
                    ClosePKS(pKey);
                    break;
                }
            }
        } else {
            if (WSAGetLastError() != WSA_IO_PENDING || GetLastError() != ERROR_IO_PENDING ) {
                ClosePKS(pKey);
            }
        }
    } while(TRUE);

    return 0;
}

static
BOOL 
do_InitOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    )
{
    int err;
    pKey->clinetBuff.len = MAX_BUFFER_SIZE;
    pKey->nextOp = NEXT_OPTYPE_RECIVE;
    
    err = WSARecv ( pKey->socket,
                    &pKey->clinetBuff,
                    1,
                    &pKey->sizeRecv,
                    &pKey->flag,
                    &pKey->clientOp,
                    NULL);
    if (err == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        ClosePKS(pKey);
        return FALSE;
    }
    return TRUE;
}
static
BOOL 
do_SendOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    )
{
    int err;
    HANDLE hTmp;
    int summ;
    int hasRead;
    
    summ = pKey->fileSize;
    hasRead = pKey->sizeHasRead;
    

    if (hasRead == summ) {
            ClosePKS(pKey);
            return FALSE;
    } else {
       pKey->sizeHasRead = pKey->sizeHasRead + size;
    }
#ifdef  DBG
    printf("pKey->sizeHasRead : %d, size: %d\n", pKey->sizeHasRead, size);
#endif 
    pKey->nextOp = NEXT_OPTYPE_READ_FILE;
    
    pKey->clinetBuff.len = size;
    
    err = WSASend ( pKey->socket,
                    &pKey->clinetBuff,
                    1,
                    &pKey->sizeSend,
                    pKey->flag,
                    &pKey->clientOp,
                    NULL);
    if (err == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        ClosePKS(pKey);
        return FALSE;
    }
    
    return TRUE;

}
static
BOOL 
do_ReadFileOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    )
{
    int err;
    HANDLE hTmp;
    int summ;
    int hasRead;
    BOOL status;
    HANDLE hFile;
    
    if (pKey->sendHeader = 1) {
        pKey->sendHeader = 0;
    }
    summ = pKey->fileSize;
    hasRead = pKey->sizeHasRead;
    
    if (hasRead == summ) {
        ClosePKS(pKey);
        return FALSE;
    }
    pKey->nextOp = NEXT_OPTYPE_SEND;
    hFile = pKey->hFile;
    
    pKey->fileOp.Offset = pKey->sizeHasRead;
#ifdef  DBG
    printf("pKey->fileOp.Offset : %d\n", pKey->fileOp.Offset);
#endif 
    status = ReadFile(  hFile,
                        pKey->clinetBuff.buf,
                        MAX_BUFFER_SIZE,
                        &pKey->sizeSend,
                        &pKey->fileOp);
        
    if (status == FALSE  && GetLastError() != ERROR_IO_PENDING) {
        ClosePKS(pKey);
        return FALSE;
    }
    return TRUE;
}
static
BOOL 
do_ReciveOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    )
{
    DWORD op;
    char buffer[1024];
    int len;
    int err;
    char buffer1[1024];
    HANDLE hFile;
    BOOL status;
    HANDLE hTmp;
    DWORD highOffset;
    DWORD lowOffset;
    
    buffer[0] = 0;
    buffer1[0] = 0;
    
    pKey->sizeRecv = size;
    op = RequestParmeterAnlyze(pKey);
    if (op == 1) {
        BuildRequestURL(pKey->httpRequest.http_request_line.RequestURL, buffer);
        strcat(buffer1, ROOT);
        strcat(buffer1, buffer);
#ifdef  DBG
        printf("File Path:%s\n", buffer1);
#endif 

        
        hFile = GetFileHandle(buffer1);
        
        if (hFile == INVALID_HANDLE_VALUE) {
            
            printf("Not Found %s, !!!\n", buffer1);
        
            len = BuildErrorPage(404, "404 NOT FOUND", buffer);
            BuildPage(pKey, buffer, len, op);
            err = WSASend ( pKey->socket,
                            &pKey->fileBuff,
                            1,
                            &pKey->sizeSend,
                            pKey->flag,
                            &pKey->fileOp,
                            NULL);
            if (err == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                ClosePKS(pKey);
            }
            return FALSE;
        }
        printf("Found %s, ok!!!\n", buffer1);
        
        pKey->hFile = hFile;
        pKey->nextOp = NEXT_OPTYPE_READ_FILE;
    
        lowOffset = GetFileSize(hFile ,&highOffset);
        
#ifdef  DBG
        printf("file : %s, size : %d\n", buffer1, lowOffset);
#endif 
        pKey->fileSize = lowOffset;

        hTmp = CreateIoCompletionPort ( (HANDLE)hFile,
                                        hIocp,
                                        (ULONG_PTR)pKey,
                                        GetSystemProcessNumber());
        if (hTmp == INVALID_HANDLE_VALUE) {
            ClosePKS(pKey);
            return FALSE;
        }
        SendHttpHeader(pKey, buffer1);

        return TRUE;
    }
    
    len = BuildErrorPage(op, "Server Error", buffer);
    
    BuildPage(pKey, buffer, len, op);

    err = WSASend ( pKey->socket,
                    &pKey->fileBuff,
                    1,
                    &pKey->sizeSend,
                    pKey->flag,
                    &pKey->fileOp,
                    NULL);
    if (err == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        ClosePKS(pKey);
        return FALSE;
    }
    return TRUE;
}
static
BOOL 
do_ExitOp (
    PKey_Struct pKey,
    int size,
    LPWSAOVERLAPPED pOverLap
    )
{
    return TRUE;
}


PKey_Struct
WINAPI
CreatePKS (
    VOID
    )
{
    PKey_Struct pKey = malloc(sizeof(Key_Struct));
    
    if (pKey) {
        memset(pKey, 0, sizeof(Key_Struct));
        pKey->socket = INVALID_SOCKET;
        pKey->hFile = INVALID_HANDLE_VALUE;
        pKey->bufferRecv = malloc(MAX_BUFFER_SIZE);
        pKey->bufferSend = malloc(MAX_BUFFER_SIZE);
        pKey->clinetBuff.buf = pKey->bufferRecv;
        pKey->fileBuff.buf = pKey->bufferSend;
        pKey->clinetBuff.len = MAX_BUFFER_SIZE;
        pKey->fileBuff.len = MAX_BUFFER_SIZE;
        pKey->httpRequest.pHashTable = CreateHashTable(128, NULL);
        pKey->httpRespons.pHashTable = CreateHashTable(128, NULL);
    }
#ifdef  DBG
    printf("*****Malloc :%p\n", pKey);
#endif 
    assert(pKey->bufferRecv);
    assert(pKey->bufferSend);
    
    
    return pKey;
}

VOID KeyCloseHash(PNODE pNode)
{
    free(pNode->key);
    free((char *)pNode->value);
}
VOID
WINAPI
ClosePKS (
    PKey_Struct pKey
    )
{
    if (pKey) {
#ifdef  DBG
        printf("****Free %p\n\n", pKey);
#endif 
        closesocket(pKey->socket);
        CloseHandle(pKey->hFile);
        DestoryHashTable(pKey->httpRequest.pHashTable, KeyCloseHash);
        DestoryHashTable(pKey->httpRespons.pHashTable, KeyCloseHash);
        free(pKey->bufferRecv);
        free(pKey->bufferSend);
        free(pKey);
    }
}
PMIME
QueryKey (
    MIME *mimeArray,
    int size,
    char *key
    )
{
    int middle;
    int left;
    int right;
    int result;

    left = 0;
    result = 0;
    right = size - 1;
    
    while (left <= right) {
        middle = (left + right) / 2;
        result = strcmp(key, mimeArray[middle].key1);
        if (result == 0) {
            return &mimeArray[middle];
        } else if (result < 0) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    return NULL;
}
void InserSortHeader (
    HEADER_NODE *pHNODE_Array,
    int Size
    )
{
    int i, j;
    HEADER_NODE hNode;
    
    for (i = 0; i < Size; i++) {
        hNode.HEADER = pHNODE_Array[i].HEADER;
        hNode.HEADER_TYPE = pHNODE_Array[i].HEADER_TYPE;
        for (j = i; j > 0; j--) {
            if (strcmp(hNode.HEADER, pHNODE_Array[j - 1].HEADER) < 0) {
                pHNODE_Array[j].HEADER = pHNODE_Array[j - 1].HEADER;
                pHNODE_Array[j].HEADER_TYPE = pHNODE_Array[j - 1].HEADER_TYPE;
            } else {
                break;
            }
        }
        pHNODE_Array[j].HEADER = hNode.HEADER;
        pHNODE_Array[j].HEADER_TYPE = hNode.HEADER_TYPE;
    }
}
PHEADER_NODE
QueryHeaderNode (
    HEADER_NODE *pHNODE_Array,
    int Size,
    char *Key
    )
{
    int middle;
    int left;
    int right;
    int result;

    left = 0;
    result = 0;
    right = Size - 1;
    
    while (left <= right) {
        middle = (left + right) / 2;
        result = strcmp(Key, pHNODE_Array[middle].HEADER);
        if (result == 0) {
            return &pHNODE_Array[middle];
        } else if (result < 0) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    return NULL;
}
BOOL
HttpInit (
    VOID
    )
{
    //
    // init HEADER.
    //
    InserSortHeader(Headers, sizeof(Headers) / sizeof(HEADER_NODE));

    return TRUE;
}
static DWORD RequestParmeterAnlyze(PKey_Struct pkey)
{
    int size;
    int i;
    int j;
    int k;
    int index;
    char *buffer_recv;
    char buffer1[512];
    char buffer2[512];
    
    buffer1[0] = 0;
    buffer2[0] = 0;
    j = 0;
    k = 0;
    
    size = pkey->sizeRecv;
    buffer_recv = pkey->bufferRecv;
    
    
    for (i = 0; i < size; i++) {
        if (i > 255) {
            return 500;
        }
        if (buffer_recv[i] != ' ') {
            buffer1[j++] = buffer_recv[i];
        } else {
            break;
        }

    }
    
    if (buffer_recv[i] != ' ') {
        return 400;
    }
    i++;
    
    buffer1[j] = 0;

#ifdef  DBG
    printf("Method :%s\n", buffer1);
#endif
    for (index = 0; index < sizeof(Mthod_Array) / sizeof(char *); index++) {
        if (!strcmp(Mthod_Array[index], buffer1)) {
            pkey->httpRequest.http_request_line.MethodType = index;
            break;
        }
        pkey->httpRequest.http_request_line.MethodType = -1;
    }
    if (pkey->httpRequest.http_request_line.MethodType == -1) {
        return 401;
    }
    j = 0;
    
    for (; i < size; i++) {
        if (buffer_recv[i] != ' ') {
            buffer1[j++] = buffer_recv[i];
        } else {
            break;
        }
    }
    if (buffer_recv[i] != ' ') {
        return 402;
    }
    i++;
    buffer1[j] = 0;
    
    pkey->httpRequest.http_request_line.RequestURL[0] = 0;
    
    if (buffer1[0] != '/') {
        return 404;
    }
#ifdef DBG
    printf("RequestURL:%s\n", buffer1);
#endif 
    strcat(pkey->httpRequest.http_request_line.RequestURL, buffer1);
    
    if (i + 8 > size - 1) {
        return 403;
    }
    /*
    if (buffer_recv[i + 1] != 'H' || buffer_recv[i + 2] != 'T' || buffer_recv[i + 3] != 'T' ||
        buffer_recv[i + 4] != 'P' || buffer_recv[i + 5] != '/' || isdigit(buffer_recv[i + 6])  == 0 ||
        buffer_recv[i + 7] !=  '.'|| isdigit(buffer_recv[i + 8])  == 0) {
        return 405;
    }
    */
    return 1;
}
static DWORD BuildErrorPage(int erroCode, char *message, char *outBuff)
{
    char page[] =   "<html>\n"
                    "    <head>\n"
                    "        <title>%d</title>\n"
                    "    </head>\n"
                    "    <body>\n"
                    "        <h1 align=\"center\">%s</h1>\n"
                    "        <p>%s</p>\n"
                    "        <hr>\n"
                    "        <address align=\"center\">Httpd Server 0.01</address>\n"
                    "    </body>\n"
                    "</html>\n";
    
    return sprintf(outBuff, page, erroCode, message, message);

}
static VOID BuildPage(PKey_Struct pKey, char *body, int len, int erroCode)
{
    char *buffer;
    char TEMP[3] = {0x0d, 0x0a, 0x00};
    
    buffer = pKey->bufferSend;
    
    pKey->nextOp = NEXT_OPTYPE_SEND_ERRPAGE;

    pKey->fileBuff.len = sprintf(   pKey->fileBuff.buf,
                                    "%s%d%s%s%s%s%s%s%d%s%s%s%s",
                                    "HTTP/1.1 ",
                                    erroCode,
                                    TEMP,
                                    "Content-Type: text/html; charset=us-ascii",
                                    TEMP,
                                    "Connection: close",
                                    TEMP,
                                    "Content-Length: ",
                                    len,
                                    TEMP,
                                    TEMP,
                                    body,
                                    TEMP
                                );

}
static HANDLE GetFileHandle(char *path)
{
    DWORD status;
    HANDLE file;
    int len;
    
    if ((status  =  GetFileAttributes(path)) == INVALID_FILE_ATTRIBUTES) {
#ifdef DBG
        printf("%s, line : %d\n", __FUNCTION__, __LINE__);
#endif
        return INVALID_HANDLE_VALUE;
    }
    len = strlen(path);


    if (status & FILE_ATTRIBUTE_DIRECTORY) {
        if (path[len - 1] == '\\') {
            path[len - 1] = 0;
        }


        strcat(path, "\\index.html");
        if ((status  =  GetFileAttributes(path)) == INVALID_FILE_ATTRIBUTES) {
            return INVALID_HANDLE_VALUE;
        }
    }
    
#ifdef DBG
    printf("%s, line : %d\n", __FUNCTION__, __LINE__);
#endif
    
    file = CreateFile(  path,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("path : %s, GetLastError : %d\n", path, GetLastError());
    }
    return file;
}
static VOID BuildRequestURL(char *base, char *URL)
{
    int i;
    int j = 0;
    
    for (i = 0; i < strlen(base); i++) {
        if (base[i] == '/') {
            URL[j++] = '\\';
        } else if (base[i] == ' ' || base[i] == '&' || base[i] == '=' || base[i] == '\?') {
            URL[j++] = 0;
            break;
        } else {
            URL[j++] = base[i];
        }
    }

    
    URL[j] = 0;
    printf("Request for : %s\n", URL);
}
static char *GetFileTypeDoc(char *type)
{
    int i;
    int j;
    char buffer[256];
    PMIME pMime;
    
    int flag = 0;
    j = 0;
    for(i = strlen(type) - 1; i > -1; i--) {
        if (type[i] == '\\') {
            break;
        }
        if (type[i] == '.') {
            flag = i;
            break;
        }
    }        
   
    if (flag == 0) {
        return DEFAULT_STREAM;
    }
    for (i = flag + 1; i < strlen(type); i++) {
        buffer[j++] = type[i];
    }
    buffer[j] = 0;
#ifdef   DBG
    printf("type : %s\n", buffer);

#endif 
    pMime = QueryKey(MIME_Array, sizeof(MIME_Array) / sizeof(MIME), buffer);
    
    
    if (pMime == NULL) {
#ifdef  DBG
    printf("line : %d, %s\n", __LINE__, DEFAULT_STREAM);
#endif 
        return DEFAULT_STREAM;
    }
#ifdef  DBG
    printf("line : %d, %s\n", __LINE__, pMime->key2);
#endif 
    return pMime->key2;
}
static VOID SendHttpHeader(PKey_Struct pKey, char *path)
{
    int err;
    char *buffer;
    int len;
    char TEMP[3] = {0x0d, 0x0a, 0x00};
    char buf[20];

    buffer = pKey->clinetBuff.buf;
    pKey->sendHeader = 1;
#ifdef  DBG
    printf("%s, %d\n", __FUNCTION__, __LINE__);
    printf("buffer : %x\n, buffer");
#endif

#ifdef  DBG
    printf("%s, %d\n", __FUNCTION__, __LINE__);
#endif 
    buffer[0] = 0;
    /*
    strcat(buffer, "HTTP/1.1 200");
    strcat(buffer, TEMP);
    strcat(buffer, "Content-Length: ");
    sprintf(buf, "%d", pKey->fileSize);
    strcat(buffer, buf);
    strcat(buffer, TEMP);
    strcat(buffer, "Content-Type: ");
    strcat(buffer, GetFileTypeDoc(path));
    strcat(buffer, TEMP);
    strcat(buffer, TEMP);
    len =strlen(buffer);
    */
    
    len = sprintf(  buffer, 
                    "%s%s%s%d%s%s%s%s%s",
                    "HTTP/1.1 200",
                    TEMP,
                    "Content-Length: ",
                    pKey->fileSize,
                    TEMP,
                    "Content-Type: ",
                    GetFileTypeDoc(path),
                    TEMP,
                    TEMP);
    
#ifdef  DBG
     printf("len : %d\n", len);
#endif 
    pKey->clinetBuff.len = len;
    
    err = WSASend ( pKey->socket,
                    &pKey->clinetBuff,
                    1,
                    &pKey->sizeSend,
                    pKey->flag,
                    &pKey->clientOp,
                    NULL);
    if (err == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        ClosePKS(pKey);
        return ;
    }
    return ;
}
