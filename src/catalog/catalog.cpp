#include "catalog/catalog.h"

void CatalogMeta::SerializeTo(char *buf) const {
  char *temp = buf;
  std::map<table_id_t, page_id_t>::const_iterator table_iterator;
  std::map<index_id_t, page_id_t>::const_iterator index_iterator;
  MACH_WRITE_TO(uint32_t, temp, CATALOG_METADATA_MAGIC_NUM);
  temp += sizeof(uint32_t);
  MACH_WRITE_TO(size_t, temp, table_meta_pages_.size());
  temp += sizeof(size_t);
  for (table_iterator = table_meta_pages_.begin(); table_iterator != table_meta_pages_.end(); table_iterator++) {
    MACH_WRITE_TO(table_id_t, temp, table_iterator->first);
    temp += sizeof(table_id_t);
    MACH_WRITE_TO(page_id_t, temp, table_iterator->second);
    temp += sizeof(page_id_t);
  }
  MACH_WRITE_TO(size_t, temp, index_meta_pages_.size());
  temp += sizeof(size_t);
  for (index_iterator = index_meta_pages_.begin(); index_iterator != index_meta_pages_.end(); index_iterator++) {
    MACH_WRITE_TO(index_id_t, temp, index_iterator->first);
    temp += sizeof(index_id_t);
    MACH_WRITE_TO(page_id_t, temp, index_iterator->second);
    temp += sizeof(page_id_t);
  }
}

CatalogMeta *CatalogMeta::DeserializeFrom(char *buf, MemHeap *heap) {
  CatalogMeta *meta = CatalogMeta::NewInstance(heap);
  char *temp = buf;
  size_t table_size, index_size, i;
  // std::map<table_id_t, page_id_t> table_meta_pages;
  // std::map<index_id_t, page_id_t> index_meta_pages;
  if (MACH_READ_UINT32(temp) != CATALOG_METADATA_MAGIC_NUM) std::cerr << "Magic Num vertification failed" << std::endl;
  temp += sizeof(uint32_t);
  table_size = MACH_READ_FROM(size_t, temp);
  temp += sizeof(size_t);
  for (i = 0; i < table_size; i++) {
    table_id_t tableId;
    page_id_t pageId;
    tableId = MACH_READ_FROM(table_id_t, temp);
    temp += sizeof(table_id_t);
    pageId = MACH_READ_FROM(page_id_t, temp);
    temp += sizeof(page_id_t);
    // table_meta_pages.insert(map<table_id_t, page_id_t>::value_type(tableId, pageId));
    (meta->table_meta_pages_).insert(map<table_id_t, page_id_t>::value_type(tableId, pageId));
  }
  index_size = MACH_READ_FROM(size_t, temp);
  temp += sizeof(size_t);
  for (i = 0; i < index_size; i++) {
    index_id_t indexId;
    page_id_t pageId;
    indexId = MACH_READ_FROM(index_id_t, temp);
    temp += sizeof(index_id_t);
    pageId = MACH_READ_FROM(page_id_t, temp);
    temp += sizeof(page_id_t);
    // index_meta_pages.insert(map<index_id_t, page_id_t>::value_type(indexId, pageId));
    (meta->index_meta_pages_).insert(map<index_id_t, page_id_t>::value_type(indexId, pageId));
  }
  return meta;
  // return ALLOC_P(heap, CatalogMeta)(table_meta_pages, index_meta_pages);
}

uint32_t CatalogMeta::GetSerializedSize() const {
  uint32_t re;
  re = sizeof(uint32_t) + sizeof(size_t) * 2;
  re += table_meta_pages_.size() * (sizeof(table_id_t) + sizeof(page_id_t));
  re += index_meta_pages_.size() * (sizeof(index_id_t) + sizeof(page_id_t));
  return re;
}

CatalogMeta::CatalogMeta() {}

CatalogManager::CatalogManager(BufferPoolManager *buffer_pool_manager, LockManager *lock_manager,
                               LogManager *log_manager, bool init)
    : buffer_pool_manager_(buffer_pool_manager),
      lock_manager_(lock_manager),
      log_manager_(log_manager),
      heap_(new SimpleMemHeap()) {
  if (init) {
    catalog_meta_ = CatalogMeta::NewInstance(heap_);
  } else {
    auto page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
    catalog_meta_ = CatalogMeta::DeserializeFrom(page->GetData(), heap_);
    for (auto &table_meta_page : catalog_meta_->table_meta_pages_)
      LoadTable(table_meta_page.first, table_meta_page.second);
    for (auto &index_meta_page : catalog_meta_->index_meta_pages_)
      LoadIndex(index_meta_page.first, index_meta_page.second);
    buffer_pool_manager->UnpinPage(CATALOG_META_PAGE_ID, false);
    FlushCatalogMetaPage();
  }
}

CatalogManager::~CatalogManager() { delete heap_; }

dberr_t CatalogManager::CreateIndex(const std::string &table_name, const string &index_name,
                                    const std::vector<std::string> &index_keys, Transaction *txn,
                                    IndexInfo *&index_info) {
  // what we need?
  // indexes_ needs index_id,index_info
  // index_info needs index_meta, table_info
  // index_meta needs key_map_, table_id
  // catalog_meta_->index_meta_pages_ needs index_id,page_id
  // serialize index_meta to page whose id is page_id
  // index_names_ needs table_name,index_name,index_id

  if (table_names_.find(table_name) == table_names_.end()) return DB_TABLE_NOT_EXIST;
  auto iter1 = index_names_.find(table_name);
  if (iter1 != index_names_.end()) {
    auto iter2 = (iter1->second).find(index_name);
    if (iter2 != (iter1->second).end()) return DB_INDEX_ALREADY_EXIST;
  } else {
  }

  index_id_t index_id = catalog_meta_->GetNextIndexId();
  table_id_t tableId = table_names_[table_name];
  TableInfo *tableInfo = tables_[tableId];

  // form key_map
  std::vector<uint32_t> key_map;
  std::vector<Column *> columns = tableInfo->GetSchema()->GetColumns();
  for (const auto &index_key : index_keys) {
    for (size_t j = 0; j < columns.size(); j++) {
      if (columns[j]->GetName() == index_key) {
        key_map.push_back(j);
        break;
      }
      if (columns.size() - 1 == j) return DB_COLUMN_NAME_NOT_EXIST;
    }
  }

  IndexMetadata *indexMetadata = IndexMetadata::Create(index_id, index_name, tableId, key_map, heap_);
  page_id_t pageId;
  Page *index_meta_page = buffer_pool_manager_->NewPage(pageId);
  indexMetadata->SerializeTo(index_meta_page->GetData());

  index_info = IndexInfo::Create(heap_);
  index_info->Init(indexMetadata, tableInfo, buffer_pool_manager_);

  index_names_[table_name][index_name] = index_id;
  indexes_[index_id] = index_info;
  (catalog_meta_->index_meta_pages_)[index_id] = pageId;

  Page *catalog_meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
  catalog_meta_->SerializeTo(catalog_meta_page->GetData());

  buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);
  buffer_pool_manager_->UnpinPage(pageId, true);
  return DB_SUCCESS;
}

dberr_t CatalogManager::LoadIndex(const index_id_t index_id, const page_id_t page_id) {
  // need table_name, index_name & index_info
  auto index_page = buffer_pool_manager_->FetchPage(page_id);
  IndexMetadata *indexMetadata;
  IndexMetadata::DeserializeFrom(index_page->GetData(), indexMetadata, heap_);
  IndexInfo *indexInfo = IndexInfo::Create(heap_);
  TableInfo *table_info = (tables_.find(indexMetadata->GetTableId()))->second;
  indexInfo->Init(indexMetadata, table_info, buffer_pool_manager_);
  std::string table_name = table_info->GetTableName();
  std::string index_name = indexInfo->GetIndexName();

  index_names_[table_name][index_name] = index_id;

  indexes_[index_id] = indexInfo;
  buffer_pool_manager_->UnpinPage(page_id, false);
  return DB_SUCCESS;
}

dberr_t CatalogManager::DropIndex(const string &table_name, const string &index_name) {
  if (index_names_.find(table_name) == index_names_.end()) return DB_INDEX_NOT_FOUND;
  auto index_map=index_names_[table_name];
  if(index_map.find(index_name)==index_map.end()) return DB_INDEX_NOT_FOUND;
  index_id_t indexId=index_map[index_name];
  page_id_t pageId=(catalog_meta_->index_meta_pages_)[indexId];

  buffer_pool_manager_->DeletePage(pageId);
  (catalog_meta_->index_meta_pages_).erase((catalog_meta_->index_meta_pages_).find(indexId));
  indexes_.erase(indexes_.find(indexId));
  (index_names_[table_name]).erase((index_names_[table_name]).find(index_name));

  Page *catalog_meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
  catalog_meta_->SerializeTo(catalog_meta_page->GetData());
  buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

  return DB_SUCCESS;
}

dberr_t CatalogManager::FlushCatalogMetaPage() const {
  buffer_pool_manager_->FlushPage(CATALOG_META_PAGE_ID);
  return DB_SUCCESS;
}

dberr_t CatalogManager::LoadTable(const table_id_t table_id, const page_id_t page_id) {
  // table_names_ needs table_name(get from tableMetadata)
  // tables_ needs table_info
  auto table_page = buffer_pool_manager_->FetchPage(page_id);
  TableMetadata *tableMetadata;
  TableMetadata::DeserializeFrom(table_page->GetData(), tableMetadata, heap_);
  TableHeap *tableHeap =
      TableHeap::Create(buffer_pool_manager_, page_id, tableMetadata->GetSchema(), log_manager_, lock_manager_, heap_);
  TableInfo *tableInfo = TableInfo::Create(heap_);
  tableInfo->Init(tableMetadata, tableHeap);
  table_names_[tableMetadata->GetTableName()] = table_id;
  tables_[table_id] = tableInfo;
  buffer_pool_manager_->UnpinPage(page_id, false);
  return DB_SUCCESS;
}
dberr_t CatalogManager::DropTable(const string &table_name) {
  auto table_names_iter = table_names_.find(table_name);
  if (table_names_iter == table_names_.end()) return DB_TABLE_NOT_EXIST;

  table_id_t tableId = table_names_iter->second;
  auto table_meta_pages_iter = (catalog_meta_->GetTableMetaPages())->find(tableId);
  page_id_t pageId = table_meta_pages_iter->second;
  TableInfo *tableInfo = tables_[tableId];

  buffer_pool_manager_->DeletePage(pageId);
  table_names_.erase(table_names_iter);
  tables_.erase(tables_.find(tableId));
  (catalog_meta_->GetTableMetaPages())->erase(table_meta_pages_iter);

  // the problem is probably in TableHeap::FreeHeap
  (tableInfo->GetTableHeap())->FreeHeap();

  Page *catalog_meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
  catalog_meta_->SerializeTo(catalog_meta_page->GetData());
  buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);

  return DB_SUCCESS;
}
dberr_t CatalogManager::CreateTable(const string &table_name, TableSchema *schema, Transaction *txn,
                                    TableInfo *&table_info) {
  auto iter = table_names_.find(table_name);
  if (iter != table_names_.end()) {
    return DB_TABLE_ALREADY_EXIST;
  }
  table_id_t tableId = catalog_meta_->GetNextTableId();
  page_id_t pageId;
  Page *page = buffer_pool_manager_->NewPage(pageId);
  if (page == nullptr) {
    return DB_FAILED;
  }
  TableMetadata *tableMetadata = TableMetadata::Create(tableId, table_name, pageId, schema, heap_);
  tableMetadata->SerializeTo(page->GetData());
  TableHeap *tableHeap = TableHeap::Create(buffer_pool_manager_, schema, txn, log_manager_, lock_manager_, heap_);
  table_info = TableInfo::Create(heap_);
  table_info->Init(tableMetadata, tableHeap);
  table_names_.emplace(table_name, tableId);
  tables_.emplace(tableId, table_info);
  (catalog_meta_->table_meta_pages_).emplace(tableId, pageId);
  auto catalog_meta_page = buffer_pool_manager_->FetchPage(CATALOG_META_PAGE_ID);
  catalog_meta_->SerializeTo(catalog_meta_page->GetData());
  buffer_pool_manager_->UnpinPage(pageId, true);
  buffer_pool_manager_->UnpinPage(CATALOG_META_PAGE_ID, true);
  return DB_SUCCESS;
}

dberr_t CatalogManager::GetIndex(const std::string &table_name, const std::string &index_name,
                                 IndexInfo *&index_info) const {
  auto out_index_names_iter = index_names_.find(table_name);
  if (out_index_names_iter == index_names_.end()) return DB_INDEX_NOT_FOUND;
  auto inner_index_names_iter = (out_index_names_iter->second).find(index_name);
  if (inner_index_names_iter == (out_index_names_iter->second).end()) return DB_INDEX_NOT_FOUND;
  auto indexes_iter = indexes_.find(inner_index_names_iter->second);
  index_info = indexes_iter->second;
  return DB_SUCCESS;
}

dberr_t CatalogManager::GetTableIndexes(const std::string &table_name, std::vector<IndexInfo *> &indexes) const {
  auto index_names_iter = index_names_.find(table_name);
  if (index_names_iter == index_names_.end()) return DB_INDEX_NOT_FOUND;
  for (const auto &indexes_iter : index_names_iter->second) {
    index_id_t indexId = indexes_iter.second;
    indexes.push_back(indexes_.at(indexId));
  }
  return DB_SUCCESS;
}

dberr_t CatalogManager::GetTable(const string &table_name, TableInfo *&table_info) {
  auto table_names_iter = table_names_.find(table_name);
  if (table_names_.find(table_name) == table_names_.end()) {
    return DB_TABLE_NOT_EXIST;
  }
  table_id_t tableId = table_names_iter->second;
  auto tables_iter = tables_.find(tableId);
  table_info = tables_iter->second;
  return DB_SUCCESS;
}

dberr_t CatalogManager::GetTable(const table_id_t table_id, TableInfo *&table_info) {
  auto tables_iter = tables_.find(table_id);
  if (tables_iter == tables_.end()) {
    return DB_TABLE_NOT_EXIST;
  }
  table_info = tables_iter->second;
  return DB_SUCCESS;
}

dberr_t CatalogManager::GetTables(vector<TableInfo *> &tables) const {
  if (tables_.empty()) return DB_TABLE_NOT_EXIST;
  auto iter = tables_.begin();
  for (; iter != tables_.end(); iter++) {
    tables.push_back(iter->second);
  }
  return DB_SUCCESS;
}