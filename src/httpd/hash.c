#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "hash.h"
/*
int main()
{
    PHASH_TABLE pHash_Table;
    PNODE pNode, pOldNode;
    
    
    
    pHash_Table = CreateHashTable(1024, NULL);
    
    assert(pHash_Table);
    
    pNode = CreateNode();
    
    assert(pNode);
    
    pNode->key = (char *)malloc(20);
    pNode->key[0] = 0;
    strcat(pNode->key, "hellword");
    pNode->value = 100;
    
    if (!InsertIntoHashTable(pHash_Table, pNode, &pOldNode)) {
        printf("inder error\n");
        return 0;
    }
    if (ContainKey(pHash_Table, "hellword", &pOldNode)) {
        printf("key %s, value:%d\n", pOldNode->key, (int)pOldNode->value);
    }
    
    return 0;
}
*/
PTR_TYPE
WINAPI
HashCode (
    PTR_TYPE key
    )
{
    PTR_TYPE hash;
    PTR_TYPE tmp;
    char *p;
    
    p = (char *)key;
    hash = 0;
    tmp = 0;
    
    while (*p) {
        hash = (hash << 4) + *p;
        if (tmp = hash & (0x0FUL << (sizeof(PTR_TYPE) * 8  - 4))) {
            hash ^= (tmp >> 24);
            hash &= ~tmp;
        }
        p++;
    }
#ifdef _M_IX86
    return hash & (LONG_MAX);
#else
    return hash & (LLONG_MAX);
#endif
}


PHASH_TABLE
WINAPI
CreateHashTable (
     DWORD size,
     HashCodeFunction hashCodeF
    )
{
    PHASH_TABLE pHash_Table;
    PNODE *table;
    int i;
    
    if (size < 1) {
        return NULL;
    }
    
    pHash_Table = (PHASH_TABLE)malloc(sizeof(HASH_TABLE));
    if (pHash_Table == NULL) {
        return NULL;
    }        
    table = (PNODE *) malloc(size * sizeof(PNODE));
    if (table == NULL) {
        return NULL;
    }
    
    for (i = 0; i < size; i++) {
        table[i] = NULL;
    }
    pHash_Table->table = table;
    pHash_Table->size = size;
    if (hashCodeF) {
        pHash_Table->hashCodeF = hashCodeF;
    } else {
        pHash_Table->hashCodeF = HashCode;
    }
    return pHash_Table;
}

DWORD
WINAPI
DestoryHashTable (
    PHASH_TABLE pHash_Table,
    KeyClose key_close_f
    )
{
    PNODE pNode, pNextNode;
    int i;
    
    if (pHash_Table == NULL) {
        return 0;
    }
    for (i = 0; i < pHash_Table->size; i++) {
        pNode = pHash_Table->table[i];
        
        while (pNode) {
            pNextNode = pNode->next;
           if (key_close_f) {
                key_close_f(pNode);
           }
           free(pNode);
           pNode = pNextNode;
        }
    }
    free(pHash_Table);
    
    return 0;
}

BOOL
WINAPI
ContainKey (
    PHASH_TABLE pHash_Table,
    char *key,
    PNODE *ppNode
    )
{
    PNODE pNode;
    int index;
    if (pHash_Table == NULL || key == NULL) {
        return FALSE;
    }
    index = pHash_Table->hashCodeF((LONG)key) % pHash_Table->size;
    
    pNode = pHash_Table->table[index];
    
    while (pNode) {
        if (equl(key, pNode->key)) {
            if (ppNode) {
                *ppNode = pNode;
            }
            return TRUE;
        }
        pNode = pNode->next;
    }
    *ppNode = NULL;
    return FALSE;
}


BOOL
WINAPI
InsertIntoHashTable (
    PHASH_TABLE pHash_Table,
    PNODE pNode,
    PNODE *ppOldNode
    )
{
    PNODE pNodeHead, pNextNode;
    int i,index;
    
    if (pHash_Table == NULL || pNode == NULL || pNode->key == NULL) {
        return FALSE;
    }
    index = pHash_Table->hashCodeF((LONG)pNode->key) % pHash_Table->size;
    pNodeHead = pHash_Table->table[index];
    if (pNodeHead == NULL) {
        pHash_Table->table[index] = pNode;
        *ppOldNode = NULL;
        return TRUE;
    }
    if (equl(pNodeHead->key, pNode->key)) {
        pNode->next = pNodeHead->next;
        pHash_Table->table[index] = pNode;
        *ppOldNode = pNodeHead;
        return TRUE;
    }
    pNextNode = pNodeHead->next;
    
    while (pNextNode) {
        if (equl(pNextNode->key, pNode->key)) {
            pNodeHead->next = pNode;
            pNode->next = pNextNode->next;
            *ppOldNode = pNextNode;
            return TRUE;
        }
        pNodeHead = pNextNode;
        pNextNode = pNextNode->next;
    }
    pNodeHead->next = pNode;
    *ppOldNode = NULL;
    pNode->next = NULL;
    return TRUE;
}
BOOL
WINAPI
RemoveKey (
    PHASH_TABLE pHash_Table,
    char *key,
    PNODE *ppOldNode
    )
{
    PNODE pNodeHead, pNextNode;
    int i,index;
    
    if (pHash_Table == NULL || key == NULL) {
        *ppOldNode = NULL;
        return FALSE;
    }
    index = pHash_Table->hashCodeF((LONG)key) % pHash_Table->size;
    pNodeHead = pHash_Table->table[index];
    
    if (pNodeHead == NULL) {
        *ppOldNode = NULL;
        return FALSE;
    }
    if (equl(pNodeHead->key, key)) {
        pHash_Table->table[index] = pNodeHead->next;
        *ppOldNode = pNodeHead;
        return TRUE;
    }
    pNextNode = pNodeHead->next;
    
    while (pNextNode) {
        if (equl(pNextNode->key, key)) {
            pNodeHead->next = pNextNode->next;;
            *ppOldNode = pNextNode;
            return TRUE;
        }
        pNodeHead = pNextNode;
        pNextNode = pNextNode->next;
    }
    *ppOldNode = NULL;
    return TRUE;
}



PNODE
WINAPI
CreateNode (
    VOID
    )
{
    PNODE pNode = (PNODE)malloc(sizeof(NODE));
    if (pNode == NULL) {
        return NULL;
    }
    pNode->key = NULL;
    pNode->next = NULL;
    pNode->value = 0;
    
    return pNode;
}
    
VOID
WINAPI
CloseNode (
    PNODE pNode
    )
{
    free(pNode);
}