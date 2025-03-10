#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
//#define HASH_TABLE_UNIT_TEST

#ifdef HASH_TABLE_UNIT_TEST

typedef struct _HASH_ENTRY
{
    void* pKey;
    SIZE_T nKeyLength;
    void* pValue;
    BOOL bIsFull;

    struct _HASH_ENTRY* pNext;
}HASH_ENTRY, * PHASH_ENTRY;

typedef struct _HASH_TABLE
{
    PVOID pInternal;
    BOOL(*DeleteEntry)(struct _HASH_TABLE* self, PVOID pKey, SIZE_T nKeyLength, BOOL bFreeKey, BOOL bFreeValue);
    BOOL(*SetEntry)(struct _HASH_TABLE** self, PVOID pKey, SIZE_T nKeyLength, PVOID pValue);
    PVOID(*GetValue)(struct _HASH_TABLE* self, PVOID pKey, SIZE_T nKeyLength);
    SIZE_T(*GetNumberOfEntries)(struct _HASH_TABLE* self);
    HANDLE(*GetFirstEntry)(struct _HASH_TABLE* self, PHASH_ENTRY* pHashEntry);
    BOOL(*GetNextEntry)(HANDLE hIterator, PHASH_ENTRY* pHashEntry);

}HASH_TABLE, * PHASH_TABLE;

BOOL
FreeHashTable(
    PHASH_TABLE* pHashTable,
    BOOL bFreeKey,
    BOOL bFreeValue);

PHASH_TABLE
CreateHashTable(
    SIZE_T nNumberOfInitalSlots,
    DOUBLE dLoadFactor);



typedef struct _HASH_TABLE_ITERATOR
{
    PHASH_TABLE pHashTable;
    PHASH_ENTRY pHashEntry;
    SIZE_T nIndex;
} HASH_TABLE_ITERATOR, * PHASH_TABLE_ITERATOR;

BOOL
IsPrime(SIZE_T n);

DWORD
_Fnv1aHash32(
    PVOID data,
    SIZE_T data_length);

SIZE_T
FindClosestPrime(
    SIZE_T number);

PHASH_ENTRY
_FindEntry(
    struct _HASH_TABLE* self,
    PVOID pKey,
    SIZE_T nKeyLength);

BOOL
_HashTableGetNextEntry(
    PHASH_TABLE_ITERATOR* sIterator,
    PHASH_ENTRY* pHashEntry);

PHASH_TABLE_ITERATOR
_HashTableGetFirstEntry(
    PHASH_TABLE pHashTable,
    PHASH_ENTRY* pHashEntry);
#endif