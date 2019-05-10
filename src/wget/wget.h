#ifndef __WGET_H__  // wget.h
#define __WGET_H__

#include <winsock2.h>
#include <windows.h>

#define MAX_BUFFER_SIZE     (4096)


typedef struct _ParameterInfo {
    int size;
    int index;
    int statusCode;
    int hasWrite;
} ParameterInfo, *PParameterInfo;


PParameterInfo
AnlyzeThePack (
    char *buffer,
    int size
    );
    
typedef struct _LINKLIST {
    void *node;
    struct _LINKLIST *next;
} LINKLIST, *PLINKLIST;

typedef struct _HTTP_REQUEST {
    SOCKET socket;
    int Method_Type;
    char *sendBuff;
    int size;
} HTTP_REQUEST, *PHTTP_REQUEST;

typedef struct _HTTP_RESPONS {
    SOCKET socket;
    int ErrorCode;
    char *recvBuff;
    int size;
} HTTP_RESPONS, *PHTTP_RESPONS;

PLINKLIST
CreateLinkList (
    void 
    );

void 
CloseLINKLIST (
    PLINKLIST *pLinkList
    );

void
Insert (
    PLINKLIST *parent,
    PLINKLIST *child
    );

void 
DestoryAll(
    PLINKLIST *head
    );

PHTTP_REQUEST
WINAPI
CreateHttpRquest (
    VOID
    );
    
PHTTP_RESPONS
WINAPI
CreateHttpRespons (
    VOID
    );
    
BOOL
Construct (
    char *host, 
    char *fullpath, 
    char *port,
    PHTTP_REQUEST pHttpRequst,
    PHTTP_RESPONS pHttpRespons
    );

int
isConnetLen (
    char *buffer
    );


#endif 