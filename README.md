# Custom Memory Allocation and Deallocation in C

## Project Description

The **Custom Memory Allocation and Deallocation in C** project is designed to provide a comprehensive exploration of memory management in the C programming language. It introduces custom implementations of memory allocation and deallocation functions, showcasing the process of managing memory on a low level. By creating `heap_alloc` and `heap_free` functions along with efficient chunk organization, this project offers insights into dynamic memory management techniques.

## Goals

1. **Educational Insight**: Understand the inner workings of memory allocation and deallocation by creating custom functions.
2. **Efficient Chunk Organization**: Implement a method to organize memory chunks effectively for allocation and deallocation.
3. **Code Understanding**: Provide clear code examples and explanations to help users grasp the custom memory management concepts.
4. **Demonstration**: Showcase how memory can be allocated and freed using the custom functions through a simple example.

## Specifications

- Language: C
- Platform: Any C-compatible platform
- Dependencies: None

## Design

### `heap_alloc` Function

The `heap_alloc` function is responsible for allocating memory from a custom heap. It searches through the list of freed memory chunks and selects a chunk that can accommodate the requested size. If the chunk size is larger than needed, it is split into two chunks—one for the allocated memory and the other for the remaining free memory.

### `heap_free` Function

The `heap_free` function is designed to deallocate memory and return it to the custom heap. When memory is freed, it is merged with adjacent free chunks to prevent fragmentation. This enhances the efficiency of memory allocation in the future.

### Chunk Organization

Memory chunks are organized in data structures that track the start address and size of each chunk. These chunks are managed in `Chunk_List` structures that enable efficient merging of free memory chunks and maintain their order.

### Example

The `main.c` file provides an example scenario where memory is allocated and freed using the custom functions. The allocated and freed chunks are displayed, demonstrating the efficacy of the custom memory management system.

## Getting Started

1. **Clone the Repository**: Begin by cloning this repository to your local machine using the following command: git clone https://github.com/omkar930/c_projects.git

