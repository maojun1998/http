#include <winsock2.h>
#include <windns.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "dnsapi.lib")

char *DnsQueryIp(char *Domain)
{
    PDNS_RECORD pDNS_RECORD;
    DNS_STATUS  dns_status;
    IN_ADDR ipaddr;
    PDNS_RECORD index;
    char *ip;
    char *buffer;
    
    buffer = (char *)malloc(56);
    buffer[0] = 0;
    
    dns_status = DnsQuery_A(Domain, 
                            DNS_TYPE_A, 
                            DNS_QUERY_BYPASS_CACHE, 
                            NULL, 
                            &pDNS_RECORD, 
                            NULL);
    
    index = pDNS_RECORD;
    
    while (index) {
        ipaddr.S_un.S_addr = index->Data.A.IpAddress;
        ip = inet_ntoa(ipaddr);
        //printf("ip : %s\n", ip);
        index = index->pNext;
        if (index == NULL) {
            strcat(buffer, ip);
        }
    }
    DnsRecordListFree(pDNS_RECORD, DnsFreeRecordListDeep);
    if (buffer[0] == 0) {
        return NULL;
    }
    return buffer;
}