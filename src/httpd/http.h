#ifndef __HTTP_H__ // http.h
#define __HTTP_H__

#include <winsock2.h>
#include <windows.h>


#include "hash.h"


#define METHOD_TYPE_OPTIONS     (0)
#define METHOD_TYPE_GET         (1)
#define METHOD_TYPE_HEAD        (2)
#define METHOD_TYPE_POST        (3)
#define METHOD_TYPE_PUT         (4)
#define METHOD_TYPE_DELETE      (5)
#define METHOD_TYPE_TRACE       (6)
#define METHOD_TYPE_CONNECTION  (7)
#define METHOD_TYPE_EXTENSION   (8)

char *Mthod_Array[] = {
    "OPTIONS", 
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECTION",
};


#define HEADER_TYPE_GENERAL     (0)
#define HEADER_TYPE_REQUEST     (1)
#define HEADER_TYPE_RESPONS     (2)
#define HEADER_TYPE_ENTITY      (3)


typedef struct _HEADER_NODE {
    char *HEADER;
    int HEADER_TYPE;
} HEADER_NODE, *PHEADER_NODE;

HEADER_NODE Headers[] = {
//
//  General Header Fields
//
    {"Cache-Control", HEADER_TYPE_GENERAL},
    {"Connection", HEADER_TYPE_GENERAL},
    {"Date", HEADER_TYPE_GENERAL},
    {"Pragma", HEADER_TYPE_GENERAL},
    {"Trailer", HEADER_TYPE_GENERAL},
    {"Transfer-Encoding", HEADER_TYPE_GENERAL},
    {"Upgrade", HEADER_TYPE_GENERAL},
    {"Via", HEADER_TYPE_GENERAL},
    {"Warning", HEADER_TYPE_GENERAL},
//
//  Request Header Fields
// 
    {"Accept", HEADER_TYPE_REQUEST},
    {"Accept-Charset", HEADER_TYPE_REQUEST},
    {"Accept-Encoding", HEADER_TYPE_REQUEST},
    {"Accept-Language", HEADER_TYPE_REQUEST},
    {"Authorization", HEADER_TYPE_REQUEST},
    {"Expect", HEADER_TYPE_REQUEST},
    {"From", HEADER_TYPE_REQUEST},
    {"Host", HEADER_TYPE_REQUEST},
    {"If-Match", HEADER_TYPE_REQUEST},
    {"If-Modified-Since", HEADER_TYPE_REQUEST},
    {"If-None-Match", HEADER_TYPE_REQUEST},
    {"If-Range", HEADER_TYPE_REQUEST},
    {"If-Unmodified-Since", HEADER_TYPE_REQUEST},
    {"Max-Forwards", HEADER_TYPE_REQUEST},
    {"Proxy-Authorization", HEADER_TYPE_REQUEST},
    {"Range", HEADER_TYPE_REQUEST},
    {"Referer", HEADER_TYPE_REQUEST},
    {"TE", HEADER_TYPE_REQUEST},
    {"User-Agent", HEADER_TYPE_REQUEST},

//
//  Response Header Fields
//
    {"Accept-Ranges",HEADER_TYPE_RESPONS},
    {"Age", HEADER_TYPE_RESPONS},
    {"ETag", HEADER_TYPE_RESPONS},
    {"Location", HEADER_TYPE_RESPONS},
    {"Proxy-Authenticate", HEADER_TYPE_RESPONS},
    {"Retry-After", HEADER_TYPE_RESPONS},
    {"Server", HEADER_TYPE_RESPONS},

//
//  Entity Header Fields
//
    {"Allow", HEADER_TYPE_ENTITY},
    {"Content-Encoding", HEADER_TYPE_ENTITY},
    {"Content-Language", HEADER_TYPE_ENTITY},
    {"Content-Length", HEADER_TYPE_ENTITY},
    {"Content-Location", HEADER_TYPE_ENTITY},
    {"Content-MD5", HEADER_TYPE_ENTITY},
    {"Content-Range", HEADER_TYPE_ENTITY},
    {"Content-Type", HEADER_TYPE_ENTITY},
    {"Expires", HEADER_TYPE_ENTITY},
    {"Last-Modified", HEADER_TYPE_ENTITY},
};
typedef struct _HTTP_VERSION {
    int max;
    int min;
} HTTP_VERSION;

typedef struct _HTTP_REQUEST_LINE {
    HTTP_VERSION HttpVersion;
    int MethodType;
    char RequestURL[256];
} HTTP_REQUEST_LINE;


typedef struct _HTTP_STATUS_LINE {
    HTTP_VERSION HttpVersion;
    DWORD StatusCode;
} HTTP_STATUS_LINE;

typedef struct _HttpRequest {
    HTTP_REQUEST_LINE http_request_line;
    PHASH_TABLE pHashTable;
} HttpRequest, *PHttpRequest;


typedef struct _HttpRespons {
    HTTP_STATUS_LINE http_respons_line;
    PHASH_TABLE pHashTable;
} HttpRespons, *PHttpRespons;

typedef struct _MIME {
    char *key1;
    char *key2;
} MIME, *PMIME;

MIME MIME_Array[] = {
    {"","application/octet-stream"},
    {"323","text/h323"},
    {"acx","application/internet-property-stream"},
    {"ai", "application/postscript"},
    {"aif",	"audio/x-aiff"},
    {"aifc", "audio/x-aiff"},
    {"aiff", "audio/x-aiff"},
    {"asf", "video/x-ms-asf"},
    {"asr", "video/x-ms-asf"},
    {"asx", "video/x-ms-asf"},
    {"au", "audio/basic"},
    {"avi", "video/x-msvideo"},
    {"axs", "application/olescript"},
    {"bas", "text/plain"},
    {"bcpio", "application/x-bcpio"},
    {"bin", "application/octet-stream"},
    {"bmp", "image/bmp"},
    {"c", "text/plain"},
    {"cat", "application/vnd.ms-pkiseccat"},
    {"cdf", "application/x-cdf"},
    {"cer", "application/x-x509-ca-cert"},
    {"class", "application/octet-stream"},
    {"clp", "application/x-msclip"},
    {"cmx", "image/x-cmx"},
    {"cod", "image/cis-cod"},
    {"cpio", "application/x-cpio"},
    {"crd", "application/x-mscardfile"},
    {"crl", "application/pkix-crl"},
    {"crt", "application/x-x509-ca-cert"},
    {"csh", "application/x-csh"},
    {"css", "text/css"},
    {"dcr", "application/x-director"},
    {"der", "application/x-x509-ca-cert"},
    {"dir", "application/x-director"},
    {"dll", "application/x-msdownload"},
    {"dms", "application/octet-stream"},
    {"doc", "application/msword"},
    {"dot", "application/msword"},
    {"dvi", "application/x-dvi"},
    {"dxr", "application/x-director"},
    {"eps", "application/postscript"},
    {"etx", "text/x-setext"},
    {"evy", "application/envoy"},
    {"exe", "application/octet-stream"},
    {"fif", "application/fractals"},
    {"flr", "x-world/x-vrml"},
    {"gif", "image/gif"},
    {"gtar", "application/x-gtar"},
    {"gz", "application/x-gzip"},
    {"h", "text/plain"},
    {"hdf", "application/x-hdf"},
    {"hlp", "application/winhlp"},
    {"hqx", "application/mac-binhex40"},
    {"hta", "application/hta"},
    {"htc", "text/x-component"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"htt", "text/webviewhtml"},
    {"ico", "image/x-icon"},
    {"ief", "image/ief"},
    {"iii", "application/x-iphone"},
    {"ins", "application/x-internet-signup"},
    {"isp", "application/x-internet-signup"},
    {"jfif", "image/pipeg"},
    {"jpe", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "application/x-javascript"},
    {"latex", "application/x-latex"},
    {"lha", "application/octet-stream"},
    {"lsf", "video/x-la-asf"},
    {"lsx", "video/x-la-asf"},
    {"lzh", "application/octet-stream"},
    {"m13", "application/x-msmediaview"},
    {"m14", "application/x-msmediaview"},
    {"m3u", "audio/x-mpegurl"} ,
    {"man", "application/x-troff-man"},
    {"mdb", "application/x-msaccess"},
    {"me", "application/x-troff-me"},
    {"mht", "message/rfc822"},
    {"mhtml", "message/rfc822"},
    {"mid", "audio/mid"},
    {"mny", "application/x-msmoney"},
    {"mov", "video/quicktime"},
    {"movie", "video/x-sgi-movie"},
    {"mp2", "video/mpeg"},
    {"mp3", "audio/mpeg"},
    {"mpa", "video/mpeg"},
    {"mpe", "video/mpeg"},
    {"mpeg", "video/mpeg"},
    {"mpg", "video/mpeg"},
    {"mpp", "application/vnd.ms-project"},
    {"mpv2", "video/mpeg"},
    {"ms", "application/x-troff-ms"},
    {"mvb", "application/x-msmediaview"},
    {"nws", "message/rfc822"},
    {"oda", "application/oda"},
    {"p10", "application/pkcs10"},
    {"p12", "application/x-pkcs12"},
    {"p7b", "application/x-pkcs7-certificates"},
    {"p7c", "application/x-pkcs7-mime"},
    {"p7m", "application/x-pkcs7-mime"},
    {"p7r", "application/x-pkcs7-certreqresp"},
    {"p7s", "application/x-pkcs7-signature"},
    {"pbm", "image/x-portable-bitmap"},
    {"pdf", "application/pdf"},
    {"pfx", "application/x-pkcs12"},
    {"pgm", "image/x-portable-graymap"},
    {"pko", "application/ynd.ms-pkipko"},
    {"pma", "application/x-perfmon"},
    {"pmc","application/x-perfmon"},
    {"pml", "application/x-perfmon"},
    {"pmr", "application/x-perfmon"},
    {"pmw", "application/x-perfmon"},
    {"pnm", "image/x-portable-anymap"},
    {"pot", "application/vnd.ms-powerpoint"},
    {"ppm", "image/x-portable-pixmap"},
    {"pps", "application/vnd.ms-powerpoint"},
    {"ppt", "application/vnd.ms-powerpoint"},
    {"prf", "application/pics-rules"},
    {"ps", "application/postscript"},
    {"pub", "application/x-mspublisher"},
    {"qt", "video/quicktime"},
    {"ra", "audio/x-pn-realaudio"},
    {"ram", "audio/x-pn-realaudio"},
    {"ras", "image/x-cmu-raster"},
    {"rgb", "image/x-rgb"},
    {"rmi", "audio/mid"},
    {"roff", "application/x-troff"},
    {"rtf", "application/rtf"},
    {"rtx", "text/richtext"},
    {"scd", "application/x-msschedule"},
    {"sct", "text/scriptlet"},
    {"setpay", "application/set-payment-initiation"},
    {"setreg", "application/set-registration-initiation"},
    {"sh", "application/x-sh"},
    {"shar", "application/x-shar"},
    {"sit", "application/x-stuffit"},
    {"snd", "audio/basic"},
    {"spc", "application/x-pkcs7-certificates"},
    {"spl", "application/futuresplash"},
    {"src", "application/x-wais-source"},
    {"sst", "application/vnd.ms-pkicertstore"},
    {"stl", "application/vnd.ms-pkistl"},
    {"stm", "text/html"},
    {"svg", "image/svg+xml"},
    {"sv4cpio", "application/x-sv4cpio"},
    {"sv4crc", "application/x-sv4crc"},
    {"swf", "application/x-shockwave-flash"},
    {"t", "application/x-troff"},
    {"tar", "application/x-tar"},
    {"tcl", "application/x-tcl"},
    {"tex", "application/x-tex"},
    {"texi", "application/x-texinfo"},
    {"texinfo", "application/x-texinfo"},
    {"tgz", "application/x-compressed"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"tr", "application/x-troff"},
    {"trm", "application/x-msterminal"},
    {"tsv", "text/tab-separated-values"},
    {"txt", "text/plain"},
    {"uls", "text/iuls"},
    {"ustar", "application/x-ustar"},
    {"vcf", "text/x-vcard"},
    {"vrml", "x-world/x-vrml"},
    {"wav", "audio/x-wav"},
    {"wcm", "application/vnd.ms-works"},
    {"wdb", "application/vnd.ms-works"},
    {"wks", "application/vnd.ms-works"},
    {"wmf", "application/x-msmetafile"},
    {"wps", "application/vnd.ms-works"},
    {"wri", "application/x-mswrite"},
    {"wrl", "x-world/x-vrml"},
    {"wrz", "x-world/x-vrml"},
    {"xaf", "x-world/x-vrml"},
    {"xbm", "image/x-xbitmap"},
    {"xla", "application/vnd.ms-excel"},
    {"xlc", "application/vnd.ms-excel"},
    {"xlm", "application/vnd.ms-excel"},
    {"xls", "application/vnd.ms-excel"},
    {"xlt", "application/vnd.ms-excel"},
    {"xlw", "application/vnd.ms-excel"},
    {"xof", "x-world/x-vrml"},
    {"xpm", "image/x-xpixmap"},
    {"xwd", "image/x-xwindowdump"},
    {"z", "application/x-compress"},
    {"zip","application/zip"},
};

char *DEFAULT_STREAM = "application/octet-stream";



PMIME
QueryKey (
    MIME *mimeArray,
    int size,
    char *key
    );

void InserSortHeader (
    HEADER_NODE *pHNODE_Array,
    int Size
    );
    
PHEADER_NODE
QueryHeaderNode (
    HEADER_NODE *pHNODE_Array,
    int Sizem,
    char *Key
    );
    
BOOL
HttpInit (
    VOID
    );

DWORD
WINAPI
GetSystemProcessNumber (
    VOID
    );
    
VOID
WINAPI
CreateThreadList (
    DWORD ThreadListNumber
    );
    
DWORD 
WINAPI 
ThreadFunction (
    LPVOID lpParameter
    );
    
#define BuildHttpVersion(max, minr) ("HTTP/" #max "." #minr)

#define MAX_BUFFER_SIZE     (4096)

VOID KeyCloseHash(PNODE pNode);

typedef struct _Key_Struct {
    SOCKET  socket;
    HANDLE  hFile;
    DWORD fileSize;
    DWORD sizeHasRead;
    HttpRequest httpRequest;
    HttpRespons httpRespons;
    WSAOVERLAPPED clientOp;
    WSAOVERLAPPED fileOp;
    WSABUF clinetBuff;
    WSABUF fileBuff;
    DWORD bufferRecvPtr;
    DWORD bufferSendPtr;
    char *bufferRecv;
    char *bufferSend;
    DWORD sizeSend;
    DWORD sizeRecv;
    DWORD nextOp;
    DWORD flag;
    DWORD sendHeader;
} Key_Struct, *PKey_Struct;

PKey_Struct
WINAPI
CreatePKS (
    VOID
    );
VOID
WINAPI
ClosePKS (
    PKey_Struct pKey
    );


#define NEXT_OPTYPE_INIT            (0)
#define NEXT_OPTYPE_SEND            (1)
#define NEXT_OPTYPE_RECIVE          (2)
#define NEXT_OPTYPE_CLOSE           (3)
#define NEXT_OPTYPE_SEND_ERRPAGE    (4)
#define NEXT_OPTYPE_READ_FILE       (5)



#endif // http.h