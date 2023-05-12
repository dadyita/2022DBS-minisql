#include "catalog/indexes.h"

IndexMetadata *IndexMetadata::Create(const index_id_t index_id, const string &index_name, const table_id_t table_id,
                                     const vector<uint32_t> &key_map, MemHeap *heap) {
  void *buf = heap->Allocate(sizeof(IndexMetadata));
  return new (buf) IndexMetadata(index_id, index_name, table_id, key_map);
}

uint32_t IndexMetadata::SerializeTo(char *buf) const {
  char *temp = buf;
  size_t i;
  MACH_WRITE_UINT32(temp, INDEX_METADATA_MAGIC_NUM);
  temp += sizeof(uint32_t);
  MACH_WRITE_UINT32(temp, index_id_);
  temp += sizeof(uint32_t);
  MACH_WRITE_UINT32(temp, index_name_.size());
  temp += sizeof(uint32_t);
  MACH_WRITE_STRING(temp, index_name_);
  temp += sizeof(char) * index_name_.size();
  MACH_WRITE_UINT32(temp, table_id_);
  temp += sizeof(uint32_t);
  MACH_WRITE_UINT32(temp, key_map_.size());
  temp += sizeof(uint32_t);
  for (i = 0; i < key_map_.size(); i++) {
    MACH_WRITE_TO(uint32_t, temp, key_map_.at(i));
    temp += sizeof(uint32_t);
  }
  return temp - buf;
}

uint32_t IndexMetadata::GetSerializedSize() const {
  uint32_t re = 0;
  re += sizeof(uint32_t) * 5;
  re += index_name_.size() * sizeof(char);
  re += key_map_.size() * sizeof(uint32_t);
  return re;
}

uint32_t IndexMetadata::DeserializeFrom(char *buf, IndexMetadata *&index_meta, MemHeap *heap) {
  char *temp = buf;
  index_id_t index_id;
  table_id_t table_id;
  size_t map_size, name_size, i;
  std::string index_name;
  std::vector<uint32_t> key_map;
  if (MACH_READ_UINT32(temp) != INDEX_METADATA_MAGIC_NUM) std::cerr << "Magic Num vertification failed" << std::endl;
  temp += sizeof(uint32_t);
  index_id = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  name_size = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  for (i = 0; i < name_size; i++) {
    index_name += MACH_READ_FROM(char, temp);
    temp += sizeof(char);
  }
  table_id = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  map_size = MACH_READ_UINT32(temp);
  temp += sizeof(uint32_t);
  for (i = 0; i < map_size; i++) {
    key_map.push_back(MACH_READ_UINT32(temp));
    temp += sizeof(uint32_t);
  }
  index_meta = ALLOC_P(heap, IndexMetadata)(index_id, index_name, table_id, key_map);
  return temp - buf;
}