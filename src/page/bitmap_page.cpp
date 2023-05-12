#include "page/bitmap_page.h"

template<size_t PageSize>
bool BitmapPage<PageSize>::AllocatePage(uint32_t &page_offset) {
  if(page_allocated_ >= 8*MAX_CHARS || !IsPageFree(next_free_page_)) return false;
  page_offset = next_free_page_;
  page_allocated_++;
  bytes[page_offset/8] |= (1 << (page_offset%8));
  for(uint32_t i=0; i<8*MAX_CHARS; i++){
    if(IsPageFree(i)){
      next_free_page_ = i;
      break;
    }
  }
  return true;
}

template<size_t PageSize>
bool BitmapPage<PageSize>::DeAllocatePage(uint32_t page_offset) {
  if(IsPageFree(page_offset)) return false;
  bytes[page_offset/8] &= (~(1 << (page_offset%8)));
  page_allocated_--;
  for(uint32_t i=0; i<8*MAX_CHARS; i++){
    if(IsPageFree(i)){
      next_free_page_ = i;
      break;
    }
  }
  return true;
}

template<size_t PageSize>
bool BitmapPage<PageSize>::IsPageFree(uint32_t page_offset) const {
  return IsPageFreeLow(page_offset/8, page_offset%8);
}

template<size_t PageSize>
bool BitmapPage<PageSize>::IsPageFreeLow(uint32_t byte_index, uint8_t bit_index) const {
  return (bytes[byte_index] & (1 << bit_index)) ? false : true;
}

template
class BitmapPage<64>;

template
class BitmapPage<128>;

template
class BitmapPage<256>;

template
class BitmapPage<512>;

template
class BitmapPage<1024>;

template
class BitmapPage<2048>;

template
class BitmapPage<4096>;
