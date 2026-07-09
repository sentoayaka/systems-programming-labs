/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 单字 (4字节) 和双字 (8字节) */
#define WSIZE       4       
#define DSIZE       8       
#define CHUNKSIZE   (1<<7)  /* 每次扩展堆的大小 (1KB) */

/* 将 size 和已分配位 (alloc) 合并为一个字 */
#define PACK(size, alloc)  ((size) | (alloc))

/* 在地址 p 处读写一个字 */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* 从地址 p 读取块的大小和分配状态 */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* 给定有效载荷指针 bp, 计算其 Header 和 Footer 的地址 */
#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 给定 bp, 计算前后相邻块的 bp 地址 */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* 在空闲块 bp 中读写 PREV 和 NEXT 指针 */
#define GET_PREV(bp) (*(char **)(bp))
#define GET_NEXT(bp) (*(char **)(bp + WSIZE))

/* 设置空闲块 bp 的 PREV 和 NEXT 指针 */
#define SET_PREV(bp, ptr) (*(char **)(bp) = (ptr))
#define SET_NEXT(bp, ptr) (*(char **)(bp + WSIZE) = (ptr))

#define CHECKHEAP(lineno) printf("Checking at line %d\n", lineno); mm_checkheap(1);

#define SEG_LIST_SIZE 16
static char *free_lists[SEG_LIST_SIZE]; // 15个桶，每个桶是一个显式链表的头

static int get_index(size_t size) {
    int idx = 0;
    size_t s = size;
    while (idx < SEG_LIST_SIZE - 1 && s > 1) {
        s >>= 1;
        idx++;
    }
    return idx;
}

/*static int get_index(size_t size) {
    if (size <= 16) return 0;
    if (size <= 80) return (size - 16) / 8; // 16-80字节每8字节一桶 (桶0-8)
    if (size <= 128) return 9;
    if (size <= 256) return 10;
    if (size <= 512) return 11;
    if (size <= 1024) return 12;
    if (size <= 2048) return 13;
    if (size <= 4096) return 14;
    return SEG_LIST_SIZE - 1;
}*/

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void insert_node(void *bp);
static void delete_node(void *bp);
void mm_checkheap(int verbose);

/* 
 * mm_init - initialize the malloc package.
 */

static char *heap_listp = 0;    

int mm_init(void)
{
    /* 1. 申请 4 个字的初始空间 */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    for (int i = 0; i < SEG_LIST_SIZE; i++) {
        free_lists[i] = NULL; // 初始化所有显式链表为空
    }

    /* 2. 写入初始结构的内容 */
    PUT(heap_listp, 0);                          /* 对齐填充 */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); /* 序言块 Header (8/1) */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* 序言块 Footer (8/1) */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     /* 结尾块 Header (0/1) */
    
    /* 3. 让 heap_listp 指向序言块 Header 和 Footer 之间 */
    heap_listp += (2 * WSIZE);

    /* 4. 扩展堆：将堆初始化为 CHUNKSIZE 个字节 */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;


    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      /* 调整后的块大小 (Adjusted size) */
    size_t extendsize; /* 如果没找着，需要扩展的大小 */
    char *bp;

    if (size == 0) return NULL;

    /* 1. 调整大小以满足对齐要求 */
    if (size <= DSIZE) 
        asize = 2 * DSIZE; /* 最小块：8字节Payload + 8字节H/F */
    else 
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* 2. 在空闲链表中寻找合适的块 */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* 3. 没找着合适的块，申请更多堆空间 */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    mm_checkheap(0); // 调试检查
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp) 
{
    // 如果指针为空，直接返回
    if (bp == NULL) return;

    // 1. 获取当前块的大小
    size_t size = GET_SIZE(HDRP(bp));

    // 2. 将 Header 和 Footer 的已分配位设为 0
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // 3. 立即尝试合并相邻的空闲块
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t asize;
    if (size <= DSIZE) asize = 2 * DSIZE;
    else asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    size_t oldsize = GET_SIZE(HDRP(ptr)); 
    
    // 情况 1：当前块已经足够大
    if (oldsize >= asize) {
        size_t remaining = oldsize - asize;
        if (remaining >= 2 * DSIZE) {
            // 分割出一个新的空闲块
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));

            void *new_free = NEXT_BLKP(ptr);
            PUT(HDRP(new_free), PACK(remaining, 0));
            PUT(FTRP(new_free), PACK(remaining, 0));
            coalesce(new_free); // 将剩余的空闲块合并到空闲链表中
        }
        return ptr;
    } 
    
    // 情况 2：尝试合并后面的空闲块
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
    size_t next_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
    
    if (!next_alloc && (oldsize + next_size >= asize)) {
        delete_node(NEXT_BLKP(ptr)); // 从显式链表中移除后一块
        size_t total_size = oldsize + next_size;

        if (total_size - asize >= 2 * DSIZE) {
            // 分割出一个新的空闲块
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));

            void *new_free = NEXT_BLKP(ptr);
            PUT(HDRP(new_free), PACK(total_size - asize, 0));
            PUT(FTRP(new_free), PACK(total_size - asize, 0));
            coalesce(new_free); // 将剩余的空闲块合并到空闲链表中
        } else {
            // 合并当前块和后一块
            PUT(HDRP(ptr), PACK(total_size, 1));
            PUT(FTRP(ptr), PACK(total_size, 1));
        }
        return ptr;
    }

    // 情况 3：如果是在堆的末尾（后一块是结尾块），直接扩堆
    if (GET_SIZE(HDRP(NEXT_BLKP(ptr))) == 0) {
        size_t extend_size = asize - oldsize;
        size_t final_extend = MAX(extend_size, 2 * DSIZE);
        if (extend_heap(final_extend / WSIZE) == NULL) return NULL;

        delete_node(NEXT_BLKP(ptr)); // 从显式链表中移除新扩展的空闲块

        // 扩堆后，新的空闲块会自动和结尾块处理，逻辑同情况 2
        size_t new_total_size = oldsize + GET_SIZE(HDRP(NEXT_BLKP(ptr)));

        if (new_total_size - asize >= 2 * DSIZE) {
            // 分割出一个新的空闲块
            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));

            void *new_free = NEXT_BLKP(ptr);
            PUT(HDRP(new_free), PACK(new_total_size - asize, 0));
            PUT(FTRP(new_free), PACK(new_total_size - asize, 0));
            insert_node(new_free); // 将剩余的空闲块合并到空闲链表中
        } else {
            PUT(HDRP(ptr), PACK(new_total_size, 1));
            PUT(FTRP(ptr), PACK(new_total_size, 1));
        }
        return ptr;
    }

    void *newptr;
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    size_t copySize = oldsize - DSIZE; // 只复制有效载荷部分
    memcpy(newptr, ptr, copySize);
    mm_free(ptr);
    return newptr;
}

/*void *mm_realloc(void *ptr, size_t size) {
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    void *newptr = mm_malloc(size);
    if (!newptr) return NULL;

    size_t oldsize = GET_SIZE(HDRP(ptr));
    size_t copySize = oldsize - DSIZE; // 减去 8 字节头尾
    if (size < copySize) copySize = size;
    
    memcpy(newptr, ptr, copySize);
    mm_free(ptr);
    return newptr;
}*/

static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* 1. 维持双字对齐：确保申请的字节数是 8 的倍数 */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    
    if ((long)(bp = mem_sbrk(size)) == -1)  
        return NULL;

    /* 2. 初始化新空闲块的 Header/Footer 和新的 Epilogue Header */
    /* 这里的 bp 指向的是原 Epilogue Header 的位置 */
    PUT(HDRP(bp), PACK(size, 0));         /* 新空闲块 Header */
    PUT(FTRP(bp), PACK(size, 0));         /* 新空闲块 Footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* 新的 Epilogue Header */

    /* 3. 如果前一个块是空闲的，合并它们 */
    return coalesce(bp);
}

static void *coalesce(void *bp) 
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        delete_node(NEXT_BLKP(bp)); // 从显式链表中移除后一块
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        delete_node(PREV_BLKP(bp)); // 从显式链表中移除前一块
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
        delete_node(PREV_BLKP(bp)); // 从显式链表中移除前一块
        delete_node(NEXT_BLKP(bp)); // 从显式链表中移除后一块
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
            GET_SIZE(HDRP(NEXT_BLKP(bp)));
        bp = PREV_BLKP(bp);
    }
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    insert_node(bp); // 将合并后的块插入显式链表

    // mm_checkheap(0); // 调试检查
    return bp;
}

static void *find_fit(size_t asize) {
    int idx = get_index(asize);
    char *bp;

    /* ---------- 阶段1：当前 bucket 内做 limited best-fit ---------- */
    char *best_bp = NULL;
    size_t min_diff = (size_t)-1;

    int count = 0;
    for (bp = free_lists[idx]; bp != NULL && count < 10; bp = GET_NEXT(bp)) {
        size_t size = GET_SIZE(HDRP(bp));
        if (size >= asize) {
            size_t diff = size - asize;
            if (diff < min_diff) {
                min_diff = diff;
                best_bp = bp;
                if (diff == 0) break; // 完全匹配，直接返回
            }
        }
        count++;
    }

    if (best_bp != NULL)
        return best_bp;

    /* ---------- 阶段2：后续 bucket 用 first-fit ---------- */
    for (int i = idx + 1; i < SEG_LIST_SIZE; i++) {
        for (bp = free_lists[i]; bp != NULL; bp = GET_NEXT(bp)) {
            if (GET_SIZE(HDRP(bp)) >= asize) {
                return bp;
            }
        }
    }

    return NULL;
}

/*static void *find_fit(size_t asize) {
    int idx = get_index(asize);
    char *bp;

    // 从当前大小对应的桶开始，遍历所有可能存放该大小的桶 
    for (int i = idx; i < SEG_LIST_SIZE; i++) {
        for (bp = free_lists[i]; bp != NULL; bp = GET_NEXT(bp)) {
            if (asize <= GET_SIZE(HDRP(bp))) {
                return bp;
            }
        }
    }
    return NULL; 
}*/

static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp)); 

    delete_node(bp); // 从显式链表中移除该块

    if ((csize - asize) >= (2 * DSIZE)) { 
        /* 剩余空间足够，进行分割 */
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        // coalesce(bp); // 将剩余的空闲块合并到空闲链表中
        insert_node(bp); // 将剩余的空闲块插入显式链表
    } else {
        /* 剩余空间太小，直接全部分配 */
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

static void insert_node(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_index(size);

    SET_NEXT(bp, free_lists[idx]);
    SET_PREV(bp, NULL);

    if (free_lists[idx] != NULL)
        SET_PREV(free_lists[idx], bp);

    free_lists[idx] = bp;
}

static void delete_node(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_index(size);

    if (GET_PREV(bp) != NULL)
        SET_NEXT(GET_PREV(bp), GET_NEXT(bp));
    else
        free_lists[idx] = GET_NEXT(bp);

    if (GET_NEXT(bp) != NULL)
        SET_PREV(GET_NEXT(bp), GET_PREV(bp));
}

void mm_checkheap(int verbose) {
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    /* 1. 检查序言块 */
    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");

    /* 2. 遍历堆中所有块 */
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose);

        /* 检查地址对齐 */
        if ((size_t)bp % 8)
            printf("Error: %p is not doubleword aligned\n", bp);

        /* 检查 Header 和 Footer 是否一致 */
        if (GET(HDRP(bp)) != GET(FTRP(bp)))
            printf("Error: header does not match footer at %p\n", bp);

        /* 检查合并：是否有连续的空闲块 */
        if (!GET_ALLOC(HDRP(bp)) && !GET_ALLOC(HDRP(NEXT_BLKP(bp))))
            printf("Error: contiguous free blocks not coalesced at %p\n", bp);
    }

    /* 3. 检查显式空闲链表 */
    int free_count_heap = 0;
    int free_count_list = 0;

    // 重新遍历堆统计空闲块
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp))) free_count_heap++;
    }

    // 遍历显式链表
    for (int i = 0; i < SEG_LIST_SIZE; i++) {
        for (bp = free_lists[i]; bp != NULL; bp = GET_NEXT(bp)) {
            /* 检查链表中的块是否标记为已分配 */
            if (GET_ALLOC(HDRP(bp)))
                printf("Error: allocated block %p is in free list\n", bp);
            
            /* 检查指针一致性：A->next = B, 则 B->prev = A */
            if (GET_NEXT(bp) != NULL && GET_PREV(GET_NEXT(bp)) != bp)
                printf("Error: next/prev pointer inconsistency at %p\n", bp);
            
            free_count_list++;
        }
    }   

    if (free_count_heap != free_count_list)
        printf("Error: list count (%d) != heap count (%d)\n", free_count_list, free_count_heap);
}





