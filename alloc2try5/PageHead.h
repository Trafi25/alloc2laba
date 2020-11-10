#include <vector>
#include <map>
#include <cmath>

class PageAllocator
{
public:
    explicit PageAllocator(size_t size);
    void* mem_alloc(size_t size);
    void* mem_realloc(void* addr, size_t size);
    void mem_free(void* addr);    
    void mem_dump();
    ~PageAllocator();

private:
    enum class PageState
    {
        Free,
        Divided,
        MultiplePage,
    };
    struct Header
    {
        PageState state;
        size_t size;
        void* HelperAddr; //указатель на следующий свободный блок для разделенной страницы
        unsigned short blocks;
    };
    struct BlockHeader
    {
        void* next;
    };

    size_t const MinPageSize = 4 * 1024;
    size_t const MinBlockSize = 16;
    size_t AmountPages;

    void* HelperAddr;

    std::vector<void*> FreePages;
    std::map<void*, Header> Headers;
    std::map<size_t, std::vector<void*>> FreePagesOfClass;



    void* GetAddrOfPage(void* addr);

   
    void* DivideFree(size_t classSize);    
    void* AllocateBlock(void* page);
    void* MultyPages(size_t size);
   
    void FreeMulti(void* FirstPageAddr, size_t AmountPages);

    void MoveMemory(void* addr, void* newAddr);
};
