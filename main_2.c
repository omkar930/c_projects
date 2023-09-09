#include <assert.h>
#include <stdio.h>

#define HEAP_CAP 640000
#define CHUNK_LIST_CAP 1024

// Represents a memory chunk
typedef struct
{
    void *start;  // Starting address of the chunk
    size_t size;  // Size of the chunk in bytes
} Chunk;

// Represents a list of memory chunks
typedef struct
{
    size_t count;          // Number of chunks in the list
    Chunk chunks[CHUNK_LIST_CAP];  // Array to store the chunks
} Chunk_List;

// Inserts a chunk into a Chunk_List in a sorted manner
void chunklListInsert(Chunk_List *list, void *ptr, size_t size)
{
    assert(list->count < CHUNK_LIST_CAP);
    list->chunks[list->count].start = ptr;
    list->chunks[list->count].size = size;
    
    // Sort the list based on the starting address of chunks
    for (size_t i = list->count; i > 0 && list->chunks[i].start < list->chunks[i - 1].start; i--)
    {
        const Chunk t = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = t;
    }
    
    list->count++;
}

// Merges contiguous chunks in a Chunk_List
void chunkListMerge(Chunk_List *list, Chunk_List *tmp)
{
    tmp->count = 0;
    
    for (size_t i = 0; i < list->count; i++)
    {
        const Chunk chunk = list->chunks[i];
        
        if (tmp->count > 0)
        {
            Chunk *top_chunk = &tmp->chunks[tmp->count - 1];
            
            // Merge if the current chunk is contiguous with the top chunk
            if (top_chunk->start + top_chunk->size == chunk.start)
            {
                top_chunk->size += chunk.size;
            }
            else
            {
                chunklListInsert(tmp, chunk.start, chunk.size);
            }
        }
        else
        {
            chunklListInsert(tmp, chunk.start, chunk.size);
        }
    }
}

// Print the contents of a Chunk_List
void chunkListDump(const Chunk_List *list)
{
    for (size_t i = 0; i < list->count; i++)
    {
        printf("-> %p, size: %zu\n",
               list->chunks[i].start,
               list->chunks[i].size);
    }
}

// Comparison function for binary search
int comparator(const void *a, void *b)
{
    return ((Chunk *)a)->start - b;
}

// Binary search to find a chunk by its starting address
int chunkListFind(const Chunk_List *list, void *ptr)
{
    size_t l = 0, r = list->count - 1;
    
    while (l <= r)
    {
        size_t mid = l + (r - l) / 2;
        
        if (comparator(&(list->chunks[mid]), ptr) == 0)
        {
            return (int)mid;
        }
        else if (comparator(&(list->chunks[mid]), ptr) < 0)
        {
            l = mid + 1;
        }
        else if (comparator(&(list->chunks[mid]), ptr) > 0)
        {
            r = mid - 1;
        }
    }
    
    return -1; // Chunk not found
}

// Remove a chunk from a Chunk_List at a given index
void chunkListRemove(Chunk_List *list, size_t index)
{
    assert(index < list->count);
    
    for (size_t i = index; i < list->count - 1; i++)
    {
        list->chunks[i] = list->chunks[i + 1];
    }
    
    list->count--;
}

// Pre-allocated memory heap
char heap[HEAP_CAP] = {0};

// Lists to manage allocated and freed memory chunks
Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {[0] = {.start = heap, .size = HEAP_CAP}}
};
Chunk_List tmp_chunks = {0};

// Allocate memory from the heap
void *heap_alloc(size_t size)
{
    if (size <= 0)
    {
        return NULL;
    }

    // Merge freed chunks and manage them
    chunkListMerge(&freed_chunks, &tmp_chunks);
    freed_chunks = tmp_chunks;

    for (size_t i = 0; i < freed_chunks.count; i++)
    {
        const Chunk chunk = freed_chunks.chunks[i];

        if (chunk.size >= size)
        {
            // Remove the chunk from freed_chunks
            chunkListRemove(&freed_chunks, i);

            // Allocate memory from the chunk
            void *const ptr = chunk.start;
            size_t tail_size = chunk.size - size;

            // Insert the allocated chunk into alloced_chunks
            chunklListInsert(&alloced_chunks, chunk.start, size);

            // If there's remaining space in the chunk, insert it back into freed_chunks
            if (tail_size > 0)
            {
                chunklListInsert(&freed_chunks, chunk.start + size, tail_size);
            }

            return chunk.start;
        }
    }

    return NULL; // Unable to allocate memory
}

// Placeholder for potential garbage collection
void heap_collect()
{
    // Currently empty
}

// Free allocated memory
void heap_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    // Find the index of the chunk in alloced_chunks
    const int index = chunkListFind(&alloced_chunks, ptr);
    assert(index >= 0);

    // Insert the freed chunk into freed_chunks
    chunklListInsert(&freed_chunks, alloced_chunks.chunks[index].start, alloced_chunks.chunks[index].size);

    // Remove the chunk from alloced_chunks
    chunkListRemove(&alloced_chunks, (size_t)index);
}

int main()
{
    // Allocate three memory chunks of size 4 each
    char *root = heap_alloc(4);
    char *root2 = heap_alloc(4);
    char *root3 = heap_alloc(4);

    // Free the first two chunks
    heap_free(root);
    heap_free(root2);

    // Attempt to allocate a new chunk of size 6
    heap_alloc(6);

    printf("allocated chunks\n");
    chunkListDump(&alloced_chunks);
    printf("\nfreed_chunks\n");
    chunkListDump(&freed_chunks);

    return 0;
}
