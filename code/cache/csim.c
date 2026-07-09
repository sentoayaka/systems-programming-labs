#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int s, E, b;
int verbose = 0;
int hit_count = 0, miss_count = 0, eviction_count = 0;
int timer = 0;

// 1. 定义缓存行 (Cache Line)
typedef struct {
    int valid;          // 有效位：1 表示有效，0 表示无效
    unsigned long tag;  // 标记位：匹配内存地址的高位
    int lru_counter;    // LRU 计数器：用于实现“最近最少使用”替换策略
    // 注意：这个实验不需要存储实际的 data block
} cache_line;

// 2. 定义缓存组 (Cache Set)
typedef struct {
    cache_line *lines;  // 指向该组内所有行的数组（大小为 E）
} cache_set;

// 3. 定义整个缓存 (Cache)
typedef struct {
    int S;              // 组数 (S = 2^s)
    cache_set *sets;    // 指向所有组的数组
} cache_t;
cache_t cache;

void init_cache(int s, int E) {
    int S = 1 << s;
    cache.S = S;

    cache.sets = (cache_set *)malloc(S * sizeof(cache_set));

    for (int i = 0; i < S; i++) {
        cache.sets[i].lines = (cache_line *)malloc(E * sizeof(cache_line));
        for (int j = 0; j < E; j++) {
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru_counter = 0;
        }
    }
}

void free_cache(cache_t cache) {
    for (int i = 0; i < cache.S; i++) {
        free(cache.sets[i].lines);
    }
    free(cache.sets);
}

void update_lru_counter(cache_line *line) {
    line->lru_counter = ++timer;
}

int find_lru_index(cache_set *target_set) {
    int min_lru = target_set->lines[0].lru_counter;
    int min_idx = 0;

    for (int i = 1; i < E; i++) {
        if (target_set->lines[i].lru_counter < min_lru) {
            min_lru = target_set->lines[i].lru_counter;
            min_idx = i;
        }
    }
    return min_idx;
}

void update_cache(unsigned long addr) {
    // 1. 提取 Set Index (中间的 s 位)
    // 先右移 b 位去掉 block offset，再用掩码保留低 s 位
    unsigned long set_index = (addr >> b) & ((1 << s) - 1);

    // 提取 Tag (高位部分)
    // 右移 (s + b) 位即可
    unsigned long tag = addr >> (s + b);

    cache_set *target_set = &cache.sets[set_index];

    // 2. 尝试查找 Hit
    for (int i = 0; i < E; i++) {
        if (target_set->lines[i].valid && target_set->lines[i].tag == tag) {
            // 【命中 Hit】
            hit_count++;
            if (verbose) printf("hit ");
            
            // 更新 LRU 计数器（见下文策略）
            update_lru_counter(&target_set->lines[i]);
            return;
        }
    }

    // 3. 如果没命中，就是 Miss
    miss_count++;
    if (verbose) printf("miss ");

    // 寻找空位 (Cold Miss)
    for (int i = 0; i < E; i++) {
        if (!target_set->lines[i].valid) {
            target_set->lines[i].valid = 1;
            target_set->lines[i].tag = tag;
            update_lru_counter(&target_set->lines[i]);
            return;
        }
    }

    eviction_count++;
    if (verbose) printf("eviction ");

    // 【缺失且触发替换 Miss Eviction】
    int evict_idx = find_lru_index(target_set); // 找到最久没用的那一行
    target_set->lines[evict_idx].tag = tag;
    
    update_lru_counter(&target_set->lines[evict_idx]);
}

int main(int argc, char *argv[]){
    int opt;
    char *tracefile;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                // 打印帮助信息
                // printUsage(); 
                exit(0);
            case 'v':
                verbose = 1;
                break;
            case 's':
                // optarg 是一个全局变量，指向当前选项的参数字符串
                s = atoi(optarg); 
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            default:
                // 如果输入了未定义的参数，打印用法并退出
                // printUsage();
                exit(1);
        }
    }

    // 接下来检查必要的参数是否都已输入
    if (s <= 0 || E <= 0 || b <= 0 || tracefile == NULL) {
        printf("Error: Missing required arguments\n");
        exit(1);
    }

    FILE *fp = fopen(tracefile, "r");
    if (fp == NULL) {
        exit(1);
    }

    init_cache(s, E);

    char op;            // 操作类型：L, S, M (I 被忽略)
    unsigned long addr; // 64位内存地址
    int size;
    
    while (fscanf(fp, " %c %lx,%d", &op, &addr, &size) > 0) {
        if (op == 'I') continue; // 忽略指令访问 (Instruction Load)

        // 根据 op 调用你的模拟函数
        if (verbose) printf("%c %lx,%d ", op, addr, size);
        
        update_cache(addr); // 这是你需要写的核心逻辑函数

        // 特殊处理 'M' (Modify)
        // Modify 相当于一次 Load 加上一次 Store，所以要更新两次
        if (op == 'M') {
            update_cache(addr);
        }
        
        if (verbose) printf("\n");
    }

    fclose(fp);

    printSummary(hit_count, miss_count, eviction_count); // 传入实际计数
    free_cache(cache);
    return 0;
}
