#include "storage/table_iterator.h"
#include "storage/table_heap.h"

TableIterator::TableIterator() {}

TableIterator::TableIterator(TableHeap *tableHeap, RowId rowId) : tableHeap_(tableHeap), row_(new Row(rowId)) {
  if (row_->GetRowId().GetPageId() != INVALID_PAGE_ID) {
    tableHeap_->GetTuple(row_, nullptr);
  }
}

TableIterator::TableIterator(const TableIterator &other) : tableHeap_(other.tableHeap_), row_(new Row(*other.row_)) {}

TableIterator::~TableIterator() {}

bool TableIterator::operator==(const TableIterator &itr) const { return row_->GetRowId() == itr.row_->GetRowId(); }

bool TableIterator::operator!=(const TableIterator &itr) const { return !(*this == itr); }

Row &TableIterator::operator*() { return *(this->row_); }

Row *TableIterator::operator->() {
  // ASSERT(*this != tableHeap_->End(), "itr is at end");
  return row_;
}
TableIterator &TableIterator::operator=(const TableIterator &other) {
  tableHeap_ = other.tableHeap_;
  row_ = other.row_;
  return *this;
}
TableIterator &TableIterator::operator++() {
  BufferPoolManager *bufferPoolManager = tableHeap_->buffer_pool_manager_;
  auto page = reinterpret_cast<TablePage *>(bufferPoolManager->FetchPage(row_->GetRowId().GetPageId()));
  RowId next_row_id;
  bool if_get = page->GetNextTupleRid(row_->GetRowId(), &next_row_id);
  while (!if_get && (page->GetNextPageId() != INVALID_PAGE_ID)) {
    auto next_page = reinterpret_cast<TablePage *>(bufferPoolManager->FetchPage(page->GetNextPageId()));
    // auto next_page=static_cast<TablePage *>(buffer_pool_manager->FetchPage(page->GetNextPageId()));
    bufferPoolManager->UnpinPage(page->GetTablePageId(), false);
    page = next_page;
    if_get = page->GetFirstTupleRid(&next_row_id);
  }
  delete row_;
  row_ = new Row(next_row_id);
  if (*this != tableHeap_->End()) {
    Transaction *txn = nullptr;
    tableHeap_->GetTuple(row_, txn);
  }
  bufferPoolManager->UnpinPage(page->GetTablePageId(), false);
  return *this;
}

TableIterator TableIterator::operator++(int) {
  TableIterator tableIterator = *this;
  ++(*this);
  return tableIterator;
}