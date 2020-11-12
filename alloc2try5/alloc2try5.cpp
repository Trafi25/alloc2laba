#include "PageHead.h"
#include <iostream>
using namespace std;

int main()
{
    PageAllocator allocator = PageAllocator(40 * 1024);
    allocator.mem_dump();

    auto* loc1 = allocator.mem_alloc(200);  
    auto* loc2 = allocator.mem_alloc(8 * 1024);
    auto* loc3 = allocator.mem_alloc(4 * 1024);
    allocator.mem_dump();
    allocator.mem_free(loc1);
    allocator.mem_free(loc3);
    allocator.mem_dump();
    auto* loc4 = allocator.mem_alloc(400);
    auto* loc5 = allocator.mem_alloc(4 * 1024);   
    auto* loc6 = allocator.mem_alloc(100);
    auto* loc7 = allocator.mem_alloc(5 * 1024);
    allocator.mem_dump();
    loc7 = allocator.mem_realloc(loc7, 32);
    loc4 = allocator.mem_realloc(loc4, 4 * 1024);   
    allocator.mem_dump();

    allocator.mem_dump();
	return 0;
}



