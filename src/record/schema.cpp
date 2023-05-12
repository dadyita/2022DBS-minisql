#include "record/schema.h"
#include <iostream>
uint32_t Schema::SerializeTo(char *buf) const {
    // replace with your code here
    char *temp = buf;
    MACH_WRITE_UINT32(temp, SCHEMA_MAGIC_NUM);
    temp += sizeof(uint32_t);
    MACH_WRITE_UINT32(temp, columns_.size());
    temp += sizeof(uint32_t);
    for (auto column : columns_) temp += column->SerializeTo(temp);
    return temp - buf;
}

uint32_t Schema::GetSerializedSize() const {
    // replace with your code here
    uint32_t serializedSize = 0;
    serializedSize += sizeof(uint32_t) * 2;
    for (auto column : columns_) serializedSize += column->GetSerializedSize();
    return serializedSize;
}

uint32_t Schema::DeserializeFrom(char *buf, Schema *&schema, MemHeap *heap) {
    // replace with your code here
    char *temp = buf;
    if (MACH_READ_UINT32(temp) != SCHEMA_MAGIC_NUM) std::cout << "magic_num verified failed" << std::endl;
    temp += sizeof(uint32_t);
    uint32_t column_size = MACH_READ_UINT32(temp);
    temp += sizeof(uint32_t);
    std::vector<Column *> columns;
    for(uint32_t i=0;i<column_size;i++){
        columns.push_back(nullptr);
        temp+=Column::DeserializeFrom(temp,columns[i],heap);
    }
    schema= ALLOC_P(heap,Schema)(columns);
    return temp-buf;
}
/*
uint32_t Schema::SerializeTo(char *buf) const {
  // replace with your code here
  //magic_num
  char *tmp = buf;
  MACH_WRITE_UINT32(tmp, SCHEMA_MAGIC_NUM);
  tmp += sizeof(uint32_t);
  //vector_size
  MACH_WRITE_UINT32(tmp, columns_.size());
  tmp += sizeof(uint32_t);
  //column_data
  size_t i = 0;
  for (; i < columns_.size(); i++) {
    columns_[i]->SerializeTo(tmp);
    tmp += columns_[i]->GetSerializedSize();
  }

  return tmp - buf;
}

uint32_t Schema::GetSerializedSize() const {
  // replace with your code here
  uint32_t columns_size = 0;
  size_t i = 0;
  for (; i < columns_.size(); i++) columns_size += columns_[i]->GetSerializedSize();
  return sizeof(uint32_t) * 2 + columns_size;
}

uint32_t Schema::DeserializeFrom(char *buf, Schema *&schema, MemHeap *heap) {
  // replace with your code here
  // magic_num
  char *tmp = buf;
  if (MACH_READ_UINT32(tmp) != SCHEMA_MAGIC_NUM) {
    std::cerr << "Magic number vertification failed" << std::endl;
  }
  tmp += sizeof(uint32_t);
  // vector_size
  uint32_t vector_size = MACH_READ_UINT32(tmp);
  tmp += sizeof(uint32_t);
  // column_data
  void *mem = heap->Allocate(sizeof(Schema));
  std::vector<Column *> columns;
  size_t i = 0;
  for (; i < vector_size; i++) {
    columns.push_back(NULL);
    Column::DeserializeFrom(tmp, (columns[i]), heap);
    tmp += columns[i]->GetSerializedSize();
  }
  schema = new (mem) Schema(columns);

  return tmp - buf;
}
 */