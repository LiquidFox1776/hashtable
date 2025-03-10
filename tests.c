#include "tests.h"
#include <assert.h>

#ifdef HASH_TABLE_UNIT_TEST

typedef struct _INTERNAL_HASH_TABLE
{
    size_t nNumberOfEntries;
    size_t nNumberOfBuckets;
    size_t nNumberOfBucketsUsed;
    PHASH_ENTRY* Buckets;
    DOUBLE dLoadFactor;
}INTERNAL_HASH_TABLE, * PINTERNAL_HASH_TABLE;

static Test_IsPrime_0()
{
    assert(IsPrime(0) == FALSE);
}


static Test_IsPrime_1()
{
    assert(IsPrime(1) == FALSE);
}


static Test_IsPrime_2()
{
    assert(IsPrime(2) == TRUE);
}


static Test_IsPrime_3()
{
    assert(IsPrime(3) == TRUE);
}


static Test_IsPrime_4()
{
    assert(IsPrime(4) == FALSE);
}


static Test_IsPrime_5()
{
    assert(IsPrime(5) == TRUE);
}


static Test_IsPrime_6()
{
    assert(IsPrime(6) == FALSE);
}


static Test_IsPrime_7()
{
    assert(IsPrime(7) == TRUE);
}

/* FindClosestPrime Tests ********************************/

static Test_FindClosestPrime_0()
{
    assert(FindClosestPrime(0) == 2);
}


static Test_FindClosestPrime_1201()
{
    assert(FindClosestPrime(1201) == 1201);
}

static Test_FindClosestPrime_4294967290()
{
    assert(FindClosestPrime(4294967290) == 4294967291);
}

static Test_FindClosestPrime_SIZE_MAX()
{
    assert(FindClosestPrime(SIZE_MAX) == SIZE_MAX);
}

/*********************************************************/

/* _Fnv1aHash32 tests ************************************/

static Test__Fnv1aHash32_When_Data_Is_Null()
{
    assert(_Fnv1aHash32(NULL, 100) == 0);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
}


static Test__Fnv1aHash32_When_Data_Length_Is_0()
{
    assert(_Fnv1aHash32("Hello", 0) == 0);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
}


static Test__Fnv1aHash32_KnownPlainText()
{
    assert(_Fnv1aHash32("The quick brown fox jumps over the lazy dog", 43) == 0x048fff90);
}
/*********************************************************/

/* CreateHashTable tests********************************/
static void Test_CreateHashTable_0_Load_Factor()
{
    PHASH_TABLE pHashTable = CreateHashTable(100, 0);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
    assert(pHashTable == NULL);
}


static void Test_CreateHashTable_SIZE_MAX_Entries()
{
    PHASH_TABLE pHashTable = CreateHashTable(SIZE_MAX, .50);
    assert(GetLastError() == ERROR_OUTOFMEMORY);
    assert(pHashTable == NULL);
}


static void Test_CreateHashTable()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable != NULL);
    assert(pHashTable->DeleteEntry != NULL);
    assert(pHashTable->GetFirstEntry != NULL);
    assert(pHashTable->GetNextEntry != NULL);
    assert(pHashTable->GetNumberOfEntries != NULL);
    assert(pHashTable->GetValue != NULL);
    assert(pHashTable->SetEntry != NULL);
    assert(pHashTable->pInternal != NULL);

    PINTERNAL_HASH_TABLE pInternal = pHashTable->pInternal;

    assert(pInternal->Buckets != NULL);
    assert(pInternal->dLoadFactor == 0.50);
    assert(pInternal->nNumberOfEntries == 0);
    assert(pInternal->nNumberOfBucketsUsed == 0);
    assert(pInternal->nNumberOfBuckets == 2);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


/*  Insert Key tests*****************************************************************/
static void Test_SetEntry_NULL_Table()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);

    BOOL bRet = FALSE;
    bRet = pHashTable->SetEntry(NULL, (PVOID)"test", 4, (PVOID)"Hello World");
    assert(bRet == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
}


static void Test_SetEntry_0_nKeyLength()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    BOOL bRet = FALSE;
    bRet = pHashTable->SetEntry(&pHashTable, (PVOID)"test", 0, (PVOID)"Hello World");
    assert(bRet == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
}


static void Test_SetEntry_NULL_pKey()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    BOOL bRet = FALSE;
    bRet = pHashTable->SetEntry(&pHashTable, NULL, 4, (PVOID)"Hello World");
    assert(bRet == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
}


static void Test_SetEntry_Test_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    BOOL bRet = FALSE;
    DWORD dwTestValue = 0xABCDEF00;

    bRet = pHashTable->SetEntry(&pHashTable, "test", 4, (PVOID)&dwTestValue);
    assert(bRet == TRUE);
    assert(GetLastError() == ERROR_SUCCESS);

    PINTERNAL_HASH_TABLE pInternal = pHashTable->pInternal;

    assert(pInternal->nNumberOfBucketsUsed == 1);
    assert(pInternal->nNumberOfEntries == 1);
    
    DWORD dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"test", 4);
    assert(dwGetValue == dwTestValue);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_SetEntry_Test_Keys()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    BOOL bRet = FALSE;
    DWORD dwTestValue1 = 0x00000000;
    DWORD dwTestValue2 = 0x00000001;
    DWORD dwTestValue3 = 0x00000002;

    bRet = pHashTable->SetEntry(&pHashTable, "spider", 6, (PVOID)&dwTestValue1);
    bRet = pHashTable->SetEntry(&pHashTable, "eye", 3, (PVOID)&dwTestValue2);
    bRet = pHashTable->SetEntry(&pHashTable, "lamb", 4, (PVOID)&dwTestValue3);

    assert(bRet == TRUE);
    assert(GetLastError() == ERROR_SUCCESS);

    PINTERNAL_HASH_TABLE pInternal = pHashTable->pInternal;

    assert(pInternal->nNumberOfBucketsUsed == 3);
    assert(pInternal->nNumberOfEntries == 3);
    // (3 / 0.50) = 6
    // next prime number >= 6 is 7
    assert(pInternal->nNumberOfBuckets == 7);

    DWORD dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"spider", 6);
    assert(dwGetValue == dwTestValue1);

    dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"eye", 3);
    assert(dwGetValue == dwTestValue2);

    dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"lamb", 4);
    assert(dwGetValue == dwTestValue3);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_SetEntry_Test_Keys_Known_Collision()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 1.0);
    BOOL bRet = FALSE;
    DWORD dwTestValue1 = 0x00000000;
    DWORD dwTestValue2 = 0x00000001;

    bRet = pHashTable->SetEntry(&pHashTable, "costarring", 10, (PVOID)&dwTestValue1);
    assert(bRet == TRUE);
    assert(GetLastError() == ERROR_SUCCESS);

    bRet = pHashTable->SetEntry(&pHashTable, "liquid", 6, (PVOID)&dwTestValue2);
    assert(bRet == TRUE);

    assert(GetLastError() == ERROR_SUCCESS);

    PINTERNAL_HASH_TABLE pInternal = pHashTable->pInternal;

    assert(pInternal->nNumberOfBucketsUsed == 1);
    assert(pInternal->nNumberOfEntries == 2);
    assert(pInternal->nNumberOfBuckets == 2);

    DWORD dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"costarring", 10);
    assert(dwGetValue == dwTestValue1);

    dwGetValue = *(DWORD*)pHashTable->GetValue(pHashTable, (PVOID)"liquid", 6);
    assert(dwGetValue == dwTestValue2);

 
    FreeHashTable(&pHashTable, FALSE, FALSE);
}

/* DeleteEntry Tests *************************************************************/
static void Test_DeleteEntry_NULL_Table()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    
    assert(pHashTable != NULL);
    assert(pHashTable->DeleteEntry(NULL, "test", 4, FALSE, FALSE) == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_NULL_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);

    assert(pHashTable != NULL);
    assert(pHashTable->DeleteEntry(pHashTable, NULL, 4, FALSE, FALSE) == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_0_nKeyLength()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);

    assert(pHashTable != NULL);
    assert(pHashTable->DeleteEntry(pHashTable, "test", 0, FALSE, FALSE) == FALSE);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_Test_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    DWORD dwValue = 0x00ABCDEF;
    
    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, (PVOID)"test", 4, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    assert(pHashTable->DeleteEntry(pHashTable, "test", 4, FALSE, FALSE) == TRUE);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 0);
    assert(pHashTable->GetValue(pHashTable, "test", 4) == FALSE);
    assert(GetLastError() == ERROR_NO_MATCH);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_From_List()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    DWORD dwValue = 0x00ABCDEF;

    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, (PVOID)"costarring", 10, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    pHashTable->SetEntry(&pHashTable, (PVOID)"liquid", 6, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 2);
    assert(pHashTable->DeleteEntry(pHashTable, "liquid", 6, FALSE, FALSE) == TRUE);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    assert(*(int*)(pHashTable->GetValue(pHashTable, (PVOID)"costarring", 10)) == dwValue);
    assert(GetLastError() == ERROR_SUCCESS);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_Head_From_List()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    DWORD dwValue = 0x00ABCDEF;

    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, (PVOID)"costarring", 10, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    pHashTable->SetEntry(&pHashTable, (PVOID)"liquid", 6, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 2);
    assert(pHashTable->DeleteEntry(pHashTable, "costarring", 10, FALSE, FALSE) == TRUE);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    assert(*(int*)(pHashTable->GetValue(pHashTable, (PVOID)"liquid", 6)) == dwValue);
    assert(GetLastError() == ERROR_SUCCESS);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_Nonexistent_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    DWORD dwValue = 0x00ABCDEF;
    CHAR* pTestKey = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 5);
    assert(pTestKey != NULL);

    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, (PVOID)"test", 4, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    assert(pHashTable->DeleteEntry(pHashTable, (PVOID)"aaaa", 4, FALSE, FALSE) == FALSE);
    assert(GetLastError() == ERROR_NO_MATCH);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    
    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_DeleteEntry_Free_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    DWORD dwValue = 0x00ABCDEF;
    CHAR* pTestKey = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 5);
    assert(pTestKey != NULL);
    memcpy(pTestKey, (void*)"test", 5);

    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, (PVOID)pTestKey, 4, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 1);
    assert(pHashTable->DeleteEntry(pHashTable, (PVOID)pTestKey, 4, TRUE, FALSE) == TRUE);
    assert(pHashTable->GetNumberOfEntries(pHashTable) == 0);
    assert(pHashTable->GetValue(pHashTable, "test", 4) == FALSE);
    assert(GetLastError() == ERROR_NO_MATCH);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}

/* GetNumberOfEntries Tests *************************************************************/
static void Test_GetNumberOfEntries_NULL_Table()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    assert(pHashTable != NULL);
    assert(pHashTable->GetNumberOfEntries(NULL) == 0);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_GetNumberOfEntries_3_Entries()
{
    PHASH_TABLE pHashTable = CreateHashTable(7, 0.50);
    DWORD dwValue = 0x00ABCDEF;

    assert(pHashTable != NULL);
    pHashTable->SetEntry(&pHashTable, "test1", 5, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    pHashTable->SetEntry(&pHashTable, "test2", 5, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);
    pHashTable->SetEntry(&pHashTable, "test3", 5, (PVOID)&dwValue);
    assert(GetLastError() == ERROR_SUCCESS);

    assert(pHashTable->GetNumberOfEntries(pHashTable) == 3);
    assert(GetLastError() == ERROR_SUCCESS);
  
    FreeHashTable(&pHashTable, FALSE, FALSE);
}

/* GetValue Tests *************************************************************/
static void Test_GetValue_NULL_Table()
{
    PHASH_TABLE pHashTable = CreateHashTable(7, 0.50);
    DWORD dwValue = 0x00ABCDEF;

    assert(pHashTable != NULL);
    assert(pHashTable->GetValue(NULL, "test", 4) == NULL);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_GetValue_NULL_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(7, 0.50);
    DWORD dwValue = 0x00ABCDEF;

    assert(pHashTable != NULL);
    assert(pHashTable->GetValue(pHashTable, NULL, 4) == NULL);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_GetValue_0_KeyLength()
{
    PHASH_TABLE pHashTable = CreateHashTable(31, 0.50);
    DWORD dwValue0 = 0x00ABCDEF;
    DWORD dwValue1 = 0xFFFFFFFF;
    DWORD dwValue2 = 0x10101010;

    assert(pHashTable != NULL);
    assert(pHashTable->SetEntry(&pHashTable, "test", 4, &dwValue0) == TRUE);

    assert(pHashTable->GetValue(pHashTable, "test", 0) == NULL);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_GetValue_NonExistent_Key()
{
    PHASH_TABLE pHashTable = CreateHashTable(31, 0.50);
    DWORD dwValue0 = 0x00ABCDEF;
    DWORD dwValue1 = 0xFFFFFFFF;
    DWORD dwValue2 = 0x10101010;

    assert(pHashTable != NULL);
    assert(pHashTable->SetEntry(&pHashTable, "test", 4, &dwValue0) == TRUE);
    assert(pHashTable->SetEntry(&pHashTable, "costarring", 10, &dwValue1) == TRUE);
    assert(pHashTable->SetEntry(&pHashTable, "liquid", 6, &dwValue2) == TRUE);

    assert(pHashTable->GetValue(pHashTable, "test1", 5) == NULL);
    assert(GetLastError() == ERROR_NO_MATCH);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_GetValue()
{
    PHASH_TABLE pHashTable = CreateHashTable(31, 0.50);
    DWORD dwValue0 = 0x00ABCDEF;
    DWORD dwValue1 = 0xFFFFFFFF;
    DWORD dwValue2 = 0x10101010;

    assert(pHashTable != NULL);
    assert(pHashTable->SetEntry(&pHashTable, "test", 4, &dwValue0) == TRUE);
    assert(pHashTable->SetEntry(&pHashTable, "costarring", 10, &dwValue1) == TRUE);
    assert(pHashTable->SetEntry(&pHashTable, "liquid", 6, &dwValue2) == TRUE);

    assert(*(DWORD *)pHashTable->GetValue(pHashTable, "test", 4) == dwValue0);
    assert(GetLastError() == ERROR_SUCCESS);
    // create a list with keys that collide
    assert(*(DWORD*)pHashTable->GetValue(pHashTable, "costarring", 10) == dwValue1);
    assert(GetLastError() == ERROR_SUCCESS);
    assert(*(DWORD*)pHashTable->GetValue(pHashTable, "liquid", 6) == dwValue2);
    assert(GetLastError() == ERROR_SUCCESS);

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_IterateTable()
{
    PHASH_TABLE pHashTable = CreateHashTable(31, 0.50);
    assert(pHashTable != NULL);

    char *strA = "test_value";
    char *strB = "costarring_value";
    char *strC = "liquid_value";

    pHashTable->SetEntry(&pHashTable, (PVOID)"test", 4, (PVOID)strA);
    pHashTable->SetEntry(&pHashTable, (PVOID)"costarring", 10, (PVOID)strB);
    pHashTable->SetEntry(&pHashTable, (PVOID)"liquid", 6, (PVOID)strC);

    printf("\nNumber of entries is: %zu\n", pHashTable->GetNumberOfEntries(pHashTable)); // 5
    PHASH_ENTRY pHashEntry = NULL;
    PHASH_TABLE_ITERATOR sIterator = _HashTableGetFirstEntry(pHashTable, &pHashEntry);
    BOOL f = TRUE;
    while (pHashEntry)
    {
        if (f)
        {
            printf("{%s: %s ", (char*)pHashEntry->pKey, (char *)pHashEntry->pValue);
            f = FALSE;
        }
        else
            printf(", %s: %s", (char*)pHashEntry->pKey, (char *)pHashEntry->pValue);
        _HashTableGetNextEntry(&sIterator, &pHashEntry);
    }

    if(!f)
        printf("}\n\n");

    FreeHashTable(&pHashTable, FALSE, FALSE);
}


static void Test_FreeHashTable()
{
    PHASH_TABLE pHashTable = CreateHashTable(0, 0.50);
    assert(GetLastError() == ERROR_INVALID_PARAMETER);
    assert(pHashTable == NULL);
}


static
void RunTests()
{
    /* IsPrime tests*/
    Test_IsPrime_0();
    Test_IsPrime_1();
    Test_IsPrime_2();
    Test_IsPrime_3();
    Test_IsPrime_4();
    Test_IsPrime_5();
    Test_IsPrime_6();
    Test_IsPrime_7();

    /* FindClosestPrime tests*/
    Test_FindClosestPrime_0();
    Test_FindClosestPrime_1201();
    Test_FindClosestPrime_4294967290();
    Test_FindClosestPrime_SIZE_MAX();

    /* _Fnv1aHash32 tests*/
    Test__Fnv1aHash32_When_Data_Is_Null();
    Test__Fnv1aHash32_When_Data_Length_Is_0();
    Test__Fnv1aHash32_KnownPlainText();

    /* CreateHashTableTests*/
    Test_CreateHashTable_0_Load_Factor();
    Test_CreateHashTable_SIZE_MAX_Entries();
    Test_CreateHashTable();

    /* SetEntry Test*/
    Test_SetEntry_NULL_Table();
    Test_SetEntry_NULL_pKey();
    Test_SetEntry_Test_Key();
    Test_SetEntry_Test_Keys();
    Test_SetEntry_Test_Keys_Known_Collision();

    /* Delete Entry */
    Test_DeleteEntry_NULL_Table();
    Test_DeleteEntry_NULL_Key();
    Test_DeleteEntry_0_nKeyLength();
    Test_DeleteEntry_Test_Key();
    Test_DeleteEntry_From_List();
    Test_DeleteEntry_Head_From_List();
    Test_DeleteEntry_Nonexistent_Key();
    Test_DeleteEntry_Free_Key();
    
    /* GetNumberOfEntries tests*/
    Test_GetNumberOfEntries_NULL_Table();
    Test_GetNumberOfEntries_3_Entries();

    /* GetValue tests */
    Test_GetValue_NULL_Table();
    Test_GetValue_NULL_Key();
    Test_GetValue_0_KeyLength();
    Test_GetValue_NonExistent_Key();
    Test_GetValue();

    /* Iterate tests */
    Test_IterateTable();
    printf("All tests have passed\n");
}


void main()
{
    RunTests();
}
#endif
