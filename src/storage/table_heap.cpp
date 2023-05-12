#include "storage/table_heap.h"

bool TableHeap::InsertTuple(Row &row, Transaction *txn) {
  uint32_t record_len = row.GetSerializedSize(schema_);
  if (record_len > TablePage::SIZE_MAX_ROW) {
    return false;
  }
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(first_page_id_));
  if (page == nullptr) return false;
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
  while (!page->InsertTuple(row, schema_, txn, lock_manager_, log_manager_)) {
    page_id_t next_page_id = page->GetNextPageId();
    if (next_page_id != INVALID_PAGE_ID) {
      page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(next_page_id));
      buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
    } else {
      // create new page
      buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
      auto new_page = reinterpret_cast<TablePage *>(buffer_pool_manager_->NewPage(next_page_id));
      if (new_page == nullptr) return false;
      buffer_pool_manager_->UnpinPage(new_page->GetTablePageId(), false);
      page->SetNextPageId(next_page_id);
      new_page->Init(next_page_id, page->GetTablePageId(), log_manager_, txn);
      buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
      page = new_page;
    }
  }
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
  return true;
}

bool TableHeap::MarkDelete(const RowId &rid, Transaction *txn) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  // If the page could not be found, then abort the transaction.
  if (page == nullptr) {
    return false;
  }
  // Otherwise, mark the tuple as deleted.
  page->WLatch();
  page->MarkDelete(rid, txn, lock_manager_, log_manager_);
  page->WUnlatch();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
  return true;
}
// the cause of problem?
bool TableHeap::UpdateTuple(Row &row, const RowId &rid, Transaction *txn) {
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  if (page == nullptr) return false;
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
  Row old_row(rid);
  bool if_update = page->UpdateTuple(row, &old_row, schema_, txn, lock_manager_, log_manager_);
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), if_update);
  if (if_update) {
    row.SetRowId(rid);
    ASSERT(row.GetRowId().GetPageId() != INVALID_PAGE_ID, "wrong in page!!");
    return true;
  } else {
    uint32_t slot_num = old_row.GetRowId().GetSlotNum();
    uint32_t tuple_size = page->GetTupleSize(slot_num);
    if (slot_num >= page->GetTupleCount() || TablePage::IsDeleted(tuple_size))
      return false;
    else {
      bool isInsert;
      isInsert = InsertTuple(row, txn);
      ASSERT(row.GetRowId().GetPageId() != INVALID_PAGE_ID, "cuocuocuo OutPage!!!");
      if (isInsert) MarkDelete(rid, txn);
      buffer_pool_manager_->UnpinPage(page->GetTablePageId(), isInsert);
      return isInsert;
    }
  }
}

void TableHeap::ApplyDelete(const RowId &rid, Transaction *txn) {
  // Step1: Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  assert(page != nullptr);
  // Step2: Delete the tuple from the page.
  page->ApplyDelete(rid, txn, log_manager_);
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

void TableHeap::RollbackDelete(const RowId &rid, Transaction *txn) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  assert(page != nullptr);
  // Rollback the delete.
  page->WLatch();
  page->RollbackDelete(rid, txn, log_manager_);
  page->WUnlatch();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

void TableHeap::FreeHeap() {
  page_id_t pageId = first_page_id_;
  while (pageId != INVALID_PAGE_ID) {
    auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(pageId));
    buffer_pool_manager_->UnpinPage(pageId, true);
    page_id_t nextPageId = page->GetNextPageId();
    buffer_pool_manager_->DeletePage(pageId);
    pageId = nextPageId;
  }
}

bool TableHeap::GetTuple(Row *row, Transaction *txn) {
  RowId rid = row->GetRowId();
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  if (page == nullptr) return false;
  bool if_get = page->GetTuple(row, schema_, txn, lock_manager_);
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), false);
  return if_get;
}

TableIterator TableHeap::Begin(Transaction *txn) {
  RowId first_row_id;
  page_id_t pageId=first_page_id_;
  while(pageId!=INVALID_PAGE_ID){
    auto page=static_cast<TablePage *>(buffer_pool_manager_->FetchPage(pageId));
    auto isFound=page->GetFirstTupleRid(&first_row_id);
    buffer_pool_manager_->UnpinPage(pageId,false);
    if(isFound){
      break;
    }
    pageId=page->GetNextPageId();
  }
  return TableIterator(this,first_row_id);
}

TableIterator TableHeap::End() {
  return TableIterator(this,RowId(INVALID_ROWID));
}

