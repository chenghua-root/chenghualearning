#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/*
 * 参考redis-1.0的dict实现.
 * 参数privdata未被使用.
 *
 * 编译：gcc -std=c99 -g3 hash_interface.c -o hash_interface.out
 *
 */

#define DICT_OK         0
#define DICT_ERR       -1
#define DICT_EXIST     -2
#define DICT_NOT_EXIST -3

#define dictFreeEntryVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->val)

#define dictHashKey(d, key) (d)->type->hashFunction(key)

#define dictSetHashKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

#define dictSetHashVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        entry->val = (_val_); \
} while(0)

#define dictCompareHashKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

#define dictFreeEntryKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

#define dictFreeEntryVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->val)

static int MemTotal = 0;

// TODO: 重新封装malloc, free时不再传递size参数
void * dictMalloc(size_t size) {
    void * p = malloc(size);
    if (p) MemTotal += size;
    return p;
}

void dictFree(void *p, size_t size) {
    free(p);
    MemTotal -= size;
}

typedef struct dictEntry {
    void * key;
    void * val;
    struct dictEntry *next;
} dictEntry;

// interface
typedef struct dictType {
    unsigned int (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *val);
} dictType;

typedef struct dict {
    dictEntry **table;
    dictType *type;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
    void *privdata;
} dict;

#define DICT_HT_INITIAL_SIZE     4
static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX;
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

dict *dictCreate(dictType * type, unsigned long size) {
    // dict *d = (dict*)malloc(sizeof(dict));
    dict *d = (dict*)dictMalloc(sizeof(dict));
    if (NULL == d) return NULL;

    unsigned long realsize = _dictNextPower(size);

    //d->table = (dictEntry**)malloc(sizeof(dictEntry*) * realsize);
    d->table = (dictEntry**)dictMalloc(sizeof(dictEntry*) * realsize);
    if (NULL == d->table) {
        //free(d);
        dictFree(d, sizeof(dict));
        return NULL;
    }

    d->type = type;

    memset(d->table, 0, sizeof(dictEntry*) * size);
    d->size = realsize;
    d->sizemask = realsize - 1;
    d->used = 0;

    return d;
}

// DICT_OK for new, DICT_EXIST for update val
int dictAdd(dict *d, void *key, void *val) {
    int idx = 0;
    dictEntry * entry = NULL;

    idx = dictHashKey(d, key);
    idx &= d->sizemask;
    dictEntry *iter = d->table[idx];
    while(iter) {
        if (dictCompareHashKeys(d, key, iter->key)) {
            dictFreeEntryVal(d, iter);
            dictSetHashVal(d, iter, val);
            return DICT_EXIST;
        }
        iter = iter->next;
    }

    //entry = malloc(sizeof(dictEntry));
    entry = dictMalloc(sizeof(dictEntry));
    dictSetHashKey(d, entry, key);
    dictSetHashVal(d, entry, val);

    entry->next = d->table[idx];
    d->table[idx] = entry;
    d->used++;

    return DICT_OK;
}

dictEntry *dictFind(dict *d, void *key) {
    int idx = 0;
    idx = dictHashKey(d, key);
    idx &= d->sizemask;
    dictEntry *iter = d->table[idx];
    while(iter) {
        if (dictCompareHashKeys(d, key, iter->key)) {
            return iter;
        }
        iter = iter->next;
    }

    return NULL;
}

int dictDel(dict *d, void *key) {
    int idx = 0;
    idx = dictHashKey(d, key);
    idx &= d->sizemask;
    dictEntry *iter = d->table[idx];
    dictEntry *preIter = NULL;
    while(iter) {
        if (dictCompareHashKeys(d, key, iter->key)) {
            if (preIter) preIter->next = iter->next;
            else d->table[idx] = iter->next;

            dictFreeEntryKey(d, iter);
            dictFreeEntryVal(d, iter);
            //free(iter);
            dictFree(iter, sizeof(dictEntry));
            d->used--;
            return DICT_OK;
        }
        preIter = iter;
        iter = iter->next;
    }
    return DICT_NOT_EXIST;
}

void dictDestroy(dict *d) {
    if (NULL == d) return;
    for (int i = 0; i < d->size; ++i) {
        dictEntry * iter = d->table[i];
        while (iter) {
            dictEntry *next = iter->next;
            dictFreeEntryKey(d, iter);
            dictFreeEntryVal(d, iter);
            //free(iter);
            dictFree(iter, sizeof(dictEntry));
            d->used--;
            iter = next;
        }
        d->table[i] = NULL;
    }
    assert(0 == d->used);
}

void dictDestruct(dict *d) {
    if (NULL == d) return;
    dictDestroy(d);
    //free(d->table);
    dictFree(d->table, sizeof(dictEntry*) * d->size);
    d->size = 0;
    d->sizemask = 0;
    d->used = 0;
    //free(d);
    dictFree(d, sizeof(dict));
}

/**********************int type********************/

unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return hash;
}

static unsigned int dictIntHash(const void *key) {
    return dictGenHashFunction((const unsigned char*)&key, sizeof(void *));
}

static int dictIntKeyCmp(void *privdata, const void *key1, const void *key2) {
    if (key1 == key2) return 1;
    else return 0;
}

static dictType DictTypeInt = {
    dictIntHash,
    NULL,
    NULL,
    dictIntKeyCmp,
    NULL,
    NULL,
};

/**********************string type********************/

static unsigned int dictStringHash(const void *key) {
    return dictGenHashFunction((const unsigned char*)key, strlen((char *)key)+1);
}

static int dictStringKeyCmp(void *privdata, const void *key1, const void *key2) {
    return 0 == strcmp((char*)key1, (char*)key2);
}

void dictStringKeyDestructor(void *privdata, void *key) {
    //free(key);
    dictFree(key, strlen((char*)key) + 1);
}

void dictStringValDestructor(void *privdata, void *val) {
    //free(val);
    dictFree(val, strlen((char*)val) + 1);
}

static dictType DictTypeString = {
    dictStringHash,
    NULL,
    NULL,
    dictStringKeyCmp,
    dictStringKeyDestructor,
    dictStringValDestructor,
};

void test_int() {
    /**********************int***************************/
    dict *d = dictCreate(&DictTypeInt, 3);
    assert(4 == d->size);

    int ret = 0;
    dictEntry * entry = NULL;
    unsigned long test_cnt = 7;

    // add
    for (unsigned long i = 0; i < test_cnt; ++i) {
      ret = dictAdd(d, (void*)i, (void*)i);
      assert(DICT_OK == ret);
    }

    // get
    for (unsigned long i = 0; i < test_cnt; ++i) {
      entry = dictFind(d, (void*)i);
      assert(NULL != entry);
      assert(i == (unsigned long)entry->val);
    }
    entry = dictFind(d, (void*)test_cnt);
    assert(NULL == entry);

    // add a exist key, with different val(-1)
    ret = dictAdd(d, (void*)0, (void*)-1);
    assert(DICT_EXIST == ret);
    entry = dictFind(d, (void*)0);
    assert(-1 == (unsigned long)entry->val);

    // del
    ret = dictDel(d, (void*)test_cnt);
    assert(DICT_NOT_EXIST == ret);
    for (unsigned long i = 0; i < test_cnt; ++i) {
      ret = dictDel(d, (void*)i);
      assert(DICT_OK == ret);
    }

    dictDestruct(d);
    d = NULL;

    assert(0 == MemTotal);
}

void test_string() {
    /************************string****************************/
    dict *d = dictCreate(&DictTypeString, 3);
    assert(4 == d->size);

    int ret = 0;
    dictEntry * entry = NULL;
    unsigned long test_cnt = 7;

    char * keys[test_cnt];
    char * vals[test_cnt];

    // add
    for (unsigned long i = 0; i < test_cnt; ++i) {
        char * key = dictMalloc(i+1);
        memset(key, i, i); *(key+i) = '\0';
        keys[i] = key;

        char * val = dictMalloc(i+1);
        memset(val, i, i); *(val+i) = '\0';
        vals[i] = val;

        ret = dictAdd(d, (void*)key, (void*)val);
        assert(DICT_OK == ret);
    }

    // get
    for (unsigned long i = 0; i < test_cnt; ++i) {
        entry = dictFind(d, (void*)keys[i]);
        assert(NULL != entry);

        assert(0 == strcmp(vals[i], entry->val));
    }

    (*keys[test_cnt-1])--;
    entry = dictFind(d, (void*)keys[test_cnt-1]);
    assert(NULL == entry);
    (*keys[test_cnt-1])++;

    // del part
    for (unsigned long i = 0; i < test_cnt/2; ++i) {
      ret = dictDel(d, (void*)keys[i]);
      assert(DICT_OK == ret);
    }

    dictDestruct(d);
    d = NULL;

    assert(0 == MemTotal);
}

int main() {
    test_int();
    test_string();

    return 0;
}
