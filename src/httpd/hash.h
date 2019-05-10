#ifndef __HASH_H__ //hash.h
#define __HASH_H__

#include <windows.h>




#ifdef _M_IX86
    typedef LONG  PTR_TYPE;
#else
    typedef VOID *PTR_TYPE;
#endif

typedef PTR_TYPE (WINAPI *HashCodeFunction)(PTR_TYPE key);

typedef struct _NODE {
    char *key;
    PTR_TYPE value;
    struct _NODE *next;
} NODE, *PNODE;

typedef VOID (*KeyClose)(PNODE pNode);


typedef struct _HASH_TABLE {
    PNODE *table;
    DWORD size;
    HashCodeFunction hashCodeF;
} HASH_TABLE, *PHASH_TABLE;

LONG
WINAPI
HashCode (
    LONG key
    );


PHASH_TABLE
WINAPI
CreateHashTable (
     DWORD size,
     HashCodeFunction hashCodeF
    );

DWORD
WINAPI
DestoryHashTable (
    PHASH_TABLE pHash_Table,
    KeyClose key_close_f
    );

BOOL
WINAPI
ContainKey (
    PHASH_TABLE pHash_Table,
    char *key,
    PNODE *ppNode
    );

BOOL
WINAPI
RemoveKey (
    PHASH_TABLE pHash_Table,
    char *key,
    PNODE *ppOldNode
    );


BOOL
WINAPI
InsertIntoHashTable (
    PHASH_TABLE pHash_Table,
    PNODE pNode,
    PNODE *ppOldNode
    );

PNODE
WINAPI
CreateNode (
    VOID
    );
    
VOID
WINAPI
CloseNode (
    PNODE pNode
    );


#define  equl(str1, str2)  (strcmp(str1, str2) == 0 ? TRUE : FALSE)









#endif // hash.h