#include "PageHead.h"
#include <string>
#include <cassert>
#include <iostream>
 
using namespace std;

PageAllocator::PageAllocator(size_t size)
{
    if (size < MinPageSize) {
        cerr << "ERROR\n";
        exit(EXIT_SUCCESS);
    }
    AmountPages = ceil(size / MinPageSize);
    size = AmountPages * MinPageSize;
    HelperAddr = malloc(size);
    for (int i = 0; i < AmountPages; ++i)
    {
        void* page = (void*)((char*)(HelperAddr)+i);
        FreePages.push_back(page);
        Header header = Header();
        header.state = PageState::Free;
        Headers.insert(pair<void*, Header>(page, header));
    }
    for (size_t i = MinBlockSize; i <= MinPageSize / 2; i *= 2)
    {
        FreePagesOfClass.insert(pair<size_t, vector<void*>>(i, vector<void*>()));
    }
}

void* PageAllocator::mem_alloc(size_t size)
{
    if (size < MinPageSize / 2)
    {
        auto  newsize= (size_t)pow(2, ceil(log(size) / log(2)));
        auto classSize = max(MinBlockSize, newsize);
        auto page = DivideFree(classSize);
        return AllocateBlock(page);
        
    }
    else {
        return MultyPages(size);
    }
}



void* PageAllocator::DivideFree(size_t classSize)
{
    if (FreePages.empty()) {
        cerr << "There are no more free pages\n";
        exit(EXIT_SUCCESS);
    }
    auto page = FreePages.back();
    FreePages.pop_back();

    Headers[page].size = classSize;
    Headers[page].HelperAddr = page;
    Headers[page].state = PageState::Divided;
    Headers[page].blocks = MinPageSize / classSize;    

    for (int i = 0; i < Headers[page].blocks - 1; i++)
    {
        void* block = (int*)((char*)page + i);
        auto blockHeader = (BlockHeader*)block;
        void* nextBlock = (int*)((char*)page + (1+i));
        blockHeader->next = nextBlock;
    }

    FreePagesOfClass[classSize].push_back(page);
    return page;
}
void* PageAllocator::AllocateBlock(void* page)
{
   
    auto addr = Headers[page].HelperAddr;
    auto next = ((BlockHeader*)addr)->next;
    Headers[page].HelperAddr= next;
    Headers[page].blocks--;
    if (Headers[page].blocks == 0)  
    {
        auto pages = FreePagesOfClass[Headers[page].size];
        pages.clear();
    }    
    return addr;
}



void* PageAllocator::MultyPages(size_t size)
{
    size_t amount = ceil(size / MinPageSize);
    if (amount > FreePages.size()) {
        cerr << "Need more space for Multiple page placements\n";
        exit(EXIT_SUCCESS);
    }
    vector<void*> pages;
    for (int i = 0; i < AmountPages; ++i)
    {
        void* page = (void*)((char*)(HelperAddr)+i);        
        if (Headers[page].state == PageState::Free)
        {
            pages.push_back(page);
            if (pages.size() == amount) break;
        }
        else
        {
            pages.clear();
        }
    }
    if (pages.size() != amount) {
        cerr << "Not enough pages to accommodate data\n";
        exit(EXIT_SUCCESS);
    }
    for (auto const& page : pages)
    {

        Headers[page].state = PageState::MultiplePage;
        Headers[page].blocks = amount;
        Headers[page].size = amount * MinPageSize;

        FreePages.erase(FreePages.begin());
    }
    return pages.front();
}

void* PageAllocator::GetAddrOfPage(void* addr)
{
    size_t pageNumber = ((char*)addr - (char*)HelperAddr);
    return (char*)HelperAddr + pageNumber;
}



void PageAllocator::mem_free(void* addr)
{
    auto page =  GetAddrOfPage(addr);
    if (Headers[page].state == PageState::Divided) {
        auto next = Headers[page].HelperAddr;
        Headers[page].HelperAddr = addr;
        ((BlockHeader*)addr)->next = next;
        Headers[page].blocks++;
        if (Headers[page].blocks == MinPageSize) {
            Headers[page].state = PageState::Free;
            FreePages.push_back(page);
            auto pages = FreePagesOfClass[Headers[page].size];
            pages.clear();;
        }
    }       
    else { FreeMulti(page, Headers[page].blocks); }
}



void PageAllocator::FreeMulti(void* FirstPageAddr, size_t AmountPages)
{
    for (int i = 0; i < AmountPages; ++i)
    {
        void* page = ((char*)(FirstPageAddr)+i);     
        Headers[page].state = PageState::Free;        
        FreePages.push_back(page);
    }
}

void* PageAllocator::mem_realloc(void* addr, size_t size)
{
    auto page = GetAddrOfPage(addr);
    mem_free(addr);
    auto newAddr = mem_alloc(size);
    auto newPage = GetAddrOfPage(newAddr);      

    memcpy(addr, newAddr, min(Headers[page].size, Headers[newPage].size));
   
    return newAddr;
}


void PageAllocator::mem_dump()
{
    cout << "Page amount: " << AmountPages << endl;
    cout << "Page size: " << MinPageSize << endl;
    cout << "Free pages: " << FreePages.size() << endl;
    for (int i = 0; i < AmountPages; ++i)
    {
        void* page = (void*)((char*)(HelperAddr)+i);
        auto header = Headers[page];
        auto state = header.state == PageState::Free ? "Free" :
            header.state == PageState::Divided ? "Divided" : "Multiple";
        auto classSize = header.state == PageState::Divided ? " | Class size: " + to_string(header.size) : "";
        auto freePlace = header.state == PageState::Divided ? " | Free blocks: " + to_string(header.blocks) : "";
        cout << "Page " << page << " " << state << classSize << freePlace << endl;
    }
    cout << endl;
}



PageAllocator::~PageAllocator()
{
    free(HelperAddr);
}
