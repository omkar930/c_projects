#include<assert.h>
#include<stdio.h>

#define HEAP_CAP 640000
#define CHUNK_LIST_CAP 1024
typedef struct
{
    void* start;
    size_t size;
}Chunk;
typedef struct
{
    size_t count;
    Chunk chunks[CHUNK_LIST_CAP];
}Chunk_List;

void chunklListInsert(Chunk_List * list,void *ptr, size_t size){
    assert(list->count < CHUNK_LIST_CAP);
    list->chunks[list->count].start = ptr;
    list->chunks[list->count].size = size;
    for(size_t i = list->count;
    i>0 && list->chunks[i].start < list->chunks[i-1].start;
    i--){
        const Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i-1];
        list->chunks[i-1] = t;
    }
    list->count++;
}

void chunkListMerge(Chunk_List *list ,Chunk_List *tmp){
    tmp->count = 0;
    for(size_t i = 0;i<list->count;i++){
        const Chunk chunk = list->chunks[i];
        if(tmp->count > 0){
            Chunk *top_chunk = &tmp->chunks[tmp->count-1];
            if(top_chunk->start + top_chunk->size == chunk.start){
                top_chunk->size += chunk.size;
            }else{
                chunklListInsert(tmp,chunk.start,chunk.size);
            }
        }else{
            chunklListInsert(tmp,chunk.start,chunk.size);
        }
    }

}

void chunkListDump(const Chunk_List *list){
    
    for(size_t i = 0;i<list->count;i++){
        printf("-> %p, size: %zu\n",
            list->chunks[i].start,
            list->chunks[i].size
        ); 
    }
}
int comparator(const void* a,void*b){
    return ((Chunk*)a)->start - b;
}

int chunkListFind(const Chunk_List * list, void *ptr){
   
    size_t l  = 0,r = list->count-1;
      while(l<=r){
        size_t mid = l+(r-l)/2;
        if(comparator(&(list->chunks[mid]),ptr) == 0){
            return (int)mid;
        }
        else if(comparator(&(list->chunks[mid]),ptr) < 0){
            l = mid+1;
        }else if(comparator(&(list->chunks[mid]),ptr) > 0){
            r = mid-1;
        }
     }
    return -1;
}

void chunkListRemove(Chunk_List * list,size_t index){
    assert(index<list->count);
    for(size_t i = index;i<list->count - 1;i++){
        list->chunks[i] = list->chunks[i+1];
    }
    list->count--;

}




char heap[HEAP_CAP] = {0};

Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {[0] = {.start = heap,.size = HEAP_CAP}}
};
Chunk_List tmp_chunks = {0};



void *heap_alloc(size_t size){
    if(size<=0){
        return NULL;
    }
    
    chunkListMerge(&freed_chunks,&tmp_chunks);
    freed_chunks = tmp_chunks;
    
    for(size_t i =0;i<freed_chunks.count;i++){
        const Chunk chunk = freed_chunks.chunks[i];
        if(chunk.size>=size){

            chunkListRemove(&freed_chunks,i);
            void * const ptr = chunk.start;
            size_t tail_size = chunk.size - size;
            chunklListInsert(&alloced_chunks,chunk.start,size);

            if(tail_size>0){
                chunklListInsert(&freed_chunks,chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }
    return NULL;
}

void heap_collect(){
    
}

void heap_free(void *ptr){
    if(ptr == NULL){
        return;
    }
    const int index = chunkListFind(&alloced_chunks,ptr);
    assert(index>=0);
    chunklListInsert(&freed_chunks,alloced_chunks.chunks[index].start,alloced_chunks.chunks[index].size);
    chunkListRemove(&alloced_chunks,(size_t)index);
}

int main(){
    char *root = heap_alloc(4);
    char *root2 = heap_alloc(4);
    char *root3 = heap_alloc(4);
    
    heap_free(root);
    heap_free(root2);
    heap_alloc(6);
    printf("allocated chunks\n");
    chunkListDump(&alloced_chunks);
    printf("\nfreed_chunks\n");
    chunkListDump(&freed_chunks);

    return 0;
    
}