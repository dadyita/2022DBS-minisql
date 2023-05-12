#include "index/basic_comparator.h"
#include "index/generic_key.h"
#include "index/index_iterator.h"

INDEX_TEMPLATE_ARGUMENTS INDEXITERATOR_TYPE::IndexIterator() {

}

INDEX_TEMPLATE_ARGUMENTS INDEXITERATOR_TYPE::IndexIterator(BPlusTreeLeafPage<KeyType, ValueType, KeyComparator> *lp, int idx, BufferPoolManager *bpm){
  leaf_page = lp;
  index_ = idx;
  buffer_pool_manager = bpm;
}

INDEX_TEMPLATE_ARGUMENTS INDEXITERATOR_TYPE::~IndexIterator() {
  buffer_pool_manager->UnpinPage(leaf_page->GetPageId(), false);
}

INDEX_TEMPLATE_ARGUMENTS const MappingType &INDEXITERATOR_TYPE::operator*() {
  // ASSERT(false, "Not implemented yet.");
  return leaf_page->GetItem(index_);
}

INDEX_TEMPLATE_ARGUMENTS INDEXITERATOR_TYPE &INDEXITERATOR_TYPE::operator++() {
  // ASSERT(false, "Not implemented yet.");
  if (index_==leaf_page->GetSize()-1){//是这个页的最后一个
    page_id_t next_page_id = leaf_page->GetNextPageId();
    if (next_page_id!=INVALID_PAGE_ID){//还有下一个页
      index_ = 0;//index归零
      //leaf_page变成下一页
      Page* page = buffer_pool_manager->FetchPage(next_page_id);
      leaf_page = reinterpret_cast<LeafPage *>(page->GetData());
    }
    else{
      index_++;
    }
  }
  else{
    index_++;
  }
  return *this;
}

INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::operator==(const IndexIterator &itr) const {
  // return false;
  if (leaf_page->GetPageId()==(itr.leaf_page)->GetPageId()&&index_==itr.index_){
    return true;
  }
  else{
    return false;
  }
}

INDEX_TEMPLATE_ARGUMENTS
bool INDEXITERATOR_TYPE::operator!=(const IndexIterator &itr) const {
  // return false;
  if (leaf_page->GetPageId()==(itr.leaf_page)->GetPageId()&&index_==itr.index_){
    return false;
  }
  else{
    return true;
  }
}

template
class IndexIterator<int, int, BasicComparator<int>>;

template
class IndexIterator<GenericKey<4>, RowId, GenericComparator<4>>;

template
class IndexIterator<GenericKey<8>, RowId, GenericComparator<8>>;

template
class IndexIterator<GenericKey<16>, RowId, GenericComparator<16>>;

template
class IndexIterator<GenericKey<32>, RowId, GenericComparator<32>>;

template
class IndexIterator<GenericKey<64>, RowId, GenericComparator<64>>;
