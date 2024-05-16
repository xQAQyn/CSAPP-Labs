#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    int valid;
    int tag;
    int last_used;
} cache_line_t;

typedef struct {
    cache_line_t* lines;
} cache_set_t;

typedef struct {
    cache_set_t* sets;
} cache_t;

#define VERBOSE_PRINTF(fmt, ...) do { if (verbose_mode) printf(fmt, __VA_ARGS__); } while (0)

void parse_addr(unsigned addr, int* tag, int* set_index, int* block_offset, int set_index_bit, int block_bits){
    *tag = addr >> (set_index_bit + block_bits);
    *set_index = (addr >> block_bits) & ((1 << set_index_bit) - 1);
    *block_offset = addr & ((1 << block_bits) - 1);
}

int find_match(cache_t* cache, int set_index, int tag, int lines){
    for(int i = 0; i < lines; i++)
        if(cache->sets[set_index].lines[i].valid && cache->sets[set_index].lines[i].tag == tag)
            return i;
    return -1;
}

int load_new_block(cache_t* cache, int set_index, int tag, int lines, int timestamp){
    int line_index = -1, evic = 0;
    for(int i = 0; i < lines; i++)
        if(cache->sets[set_index].lines[i].valid == 0){
            line_index = i;
            break;
        }
    if(line_index == -1){
        line_index = 0;
        evic = 1;
        for(int i = 1; i < lines; i++)
            if(cache->sets[set_index].lines[i].last_used < cache->sets[set_index].lines[line_index].last_used)
                line_index = i;
    }
    
    cache->sets[set_index].lines[line_index].last_used = timestamp;
    cache->sets[set_index].lines[line_index].tag = tag;
    cache->sets[set_index].lines[line_index].valid = 1;

    return evic;
}

int main(int argc, char *argv[])
{
    int hits = 0, misses = 0, evictions = 0;

    int verbose_mode = 0;
    int set_index_bit = -1, lines = -1, block_bits = -1;
    FILE* tracefile = NULL;

    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            switch(argv[i][1]){
                case 'v':
                    verbose_mode = 1;
                    break;
                case 's':
                    set_index_bit = atoi(argv[++i]);
                    break;
                case 'E':
                    lines = atoi(argv[++i]);
                    break;
                case 'b':
                    block_bits = atoi(argv[++i]);
                    break;
                case 't':
                    tracefile = fopen(argv[++i], "r");
                    break;
            }
        }
    }

    cache_t cache;
    cache.sets = (cache_set_t*)malloc(sizeof(cache_set_t) * (1 << set_index_bit));
    for(int i = 0; i < (1 << set_index_bit); i++){
        cache.sets[i].lines = (cache_line_t*)malloc(sizeof(cache_line_t) * lines);
        for(int j = 0; j < lines; j++)
            cache.sets[i].lines[j].valid = 0;
    }
    
    char op[3];
    int addr, size;
    int timestamp = 0;
    while(fscanf(tracefile, " %s %x,%d", op, &addr, &size) != EOF){
        VERBOSE_PRINTF("%c %x,%d", op[0], addr, size);
        int tag, set_index, block_offset, match;
        parse_addr(addr, &tag, &set_index, &block_offset, set_index_bit, block_bits);
        switch(op[0]){
            case 'I': case 'L':
                match = find_match(&cache, set_index, tag, lines);
                if(match != -1){
                    hits++;
                    VERBOSE_PRINTF("%s"," hit");
                    cache.sets[set_index].lines[match].last_used = timestamp;
                }else{
                    misses++;
                    VERBOSE_PRINTF("%s"," miss");
                    int evict = load_new_block(&cache, set_index, tag, lines, timestamp);
                    if(evict){
                        evictions++;
                        VERBOSE_PRINTF("%s"," eviction");
                    }
                }
                break;
            case 'S': case 'M':
                match = find_match(&cache, set_index, tag, lines);
                if(match != -1){
                    hits++;
                    VERBOSE_PRINTF("%s"," hit");
                    cache.sets[set_index].lines[match].last_used = timestamp;
                }else{
                    misses++;
                    VERBOSE_PRINTF("%s"," miss");
                    int evict = load_new_block(&cache, set_index, tag, lines, timestamp);
                    if(evict){
                        evictions++;
                        VERBOSE_PRINTF("%s"," eviction");
                    }
                    VERBOSE_PRINTF("%s"," hit");
                    hits++;
                }
                break;
            
        }
        VERBOSE_PRINTF("%s","\n");
        timestamp++;
    }

    printSummary(hits, misses, evictions);
    return 0;
}
