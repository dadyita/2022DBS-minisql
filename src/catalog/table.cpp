#include "catalog/table.h"

uint32_t TableMetadata::SerializeTo(char *buf) const {
  char *temp = buf;
  MACH_WRITE_UINT32(temp, TABLE_METADATA_MAGIC_NUM);
  temp += sizeof(uint32_t);
  MACH_WRITE_UINT32(temp, table_id_);
  temp += sizeof(uint32_t);
  MACH_WRITE_UINT32(temp, table_name_.size());
  temp += sizeof(uint32_t);
  MACH_WRITE_STRING(temp, table_name_);
  temp += sizeof(char) * table_name_.size();
  MACH_WRITE_INT32(temp, root_page_id_);
  temp += sizeof(int32_t);
  schema_->SerializeTo(temp);
  temp += schema_->GetSerializedSize();
  return temp - buf;
}

uint32_t TableMetadata::GetSerializedSize() const {
  uint32_t re = 0;
  re += sizeof(uint32_t) * 3 + sizeof(int32_t);
  re += sizeof(char) * table_name_.size();
  re += schema_->GetSerializedSize();
  return re;
}

/**
 * @param heap Memory heap passed by TableInfo
 */
uint32_t TableMetadata::DeserializeFrom(char *buf, TableMetadata *&table_meta, MemHeap *heap) {
  char *temp = buf;
  table_id_t table_id;
  size_t table_name_size, i;
  std::string table_name;
  page_id_t root_page_id;
  Schema *schema;
  if (MACH_READ_UINT32(temp) != TABLE_METADATA_MAGIC_NUM) std::cerr << "Magic Num vertification failed" << std::endl;
  temp += sizeof(uint32_t);
  table_id = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  table_name_size = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  for (i = 0; i < table_name_size; i++) {
    table_name += MACH_READ_FROM(char, temp);
    temp += sizeof(char);
  }
  root_page_id = MACH_READ_INT32(temp);
  temp += sizeof(int32_t);
  temp += Schema::DeserializeFrom(temp, schema, heap);
  table_meta = ALLOC_P(heap, TableMetadata)(table_id, table_name, root_page_id, schema);
  return temp - buf;
}

/**
 * Only called by create table
 *
 * @param heap Memory heap passed by TableInfo
 */
TableMetadata *TableMetadata::Create(table_id_t table_id, std::string table_name, page_id_t root_page_id,
                                     TableSchema *schema, MemHeap *heap) {
  // allocate space for table metadata
  void *buf = heap->Allocate(sizeof(TableMetadata));
  return new (buf) TableMetadata(table_id, table_name, root_page_id, schema);
}

TableMetadata::TableMetadata(table_id_t table_id, std::string table_name, page_id_t root_page_id, TableSchema *schema)
    : table_id_(table_id), table_name_(table_name), root_page_id_(root_page_id), schema_(schema) {}
