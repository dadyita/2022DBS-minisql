#include "record/column.h"
#include <iostream>

//for int float
Column::Column(std::string column_name, TypeId type, uint32_t index, bool nullable, bool unique)
        : name_(std::move(column_name)), type_(type), table_ind_(index),
          nullable_(nullable), unique_(unique) {
    ASSERT(type != TypeId::kTypeChar, "Wrong constructor for CHAR type.");
    switch (type) {
        case TypeId::kTypeInt :
            len_ = sizeof(int32_t);
            break;
        case TypeId::kTypeFloat :
            len_ = sizeof(float_t);
            break;
        default:
            ASSERT(false, "Unsupported column type.");
    }
}

//for char type
Column::Column(std::string column_name, TypeId type, uint32_t length, uint32_t index, bool nullable, bool unique)
        : name_(std::move(column_name)), type_(type), len_(length),
          table_ind_(index), nullable_(nullable), unique_(unique) {
    ASSERT(type == TypeId::kTypeChar, "Wrong constructor for non-VARCHAR type.");
}

//for other type
Column::Column(const Column *other) : name_(other->name_), type_(other->type_), len_(other->len_),
                                      table_ind_(other->table_ind_), nullable_(other->nullable_),
                                      unique_(other->unique_) {}

uint32_t Column::SerializeTo(char *buf) const {
    char *temp = buf;
    MACH_WRITE_UINT32(temp, COLUMN_MAGIC_NUM);
    temp += sizeof(uint32_t);
    MACH_WRITE_UINT32(temp, name_.length());
    temp += sizeof(uint32_t);
    MACH_WRITE_STRING(temp, name_);
    temp += sizeof(char) * name_.length();
    MACH_WRITE_UINT32(temp, type_);
    temp += sizeof(uint32_t);
    MACH_WRITE_UINT32(temp, len_);
    temp += sizeof(uint32_t);
    MACH_WRITE_UINT32(temp, table_ind_);
    temp += sizeof(uint32_t);
    MACH_WRITE_TO(bool, temp, nullable_);
    temp += sizeof(bool);
    MACH_WRITE_TO(bool, temp, unique_);
    temp += sizeof(bool);
    return temp - buf;
}

uint32_t Column::GetSerializedSize() const {
    // replace with your code here
    return sizeof(uint32_t) * 5 + name_.length()* sizeof(char) + sizeof(bool) * 2;
}

uint32_t Column::DeserializeFrom(char *buf, Column *&column, MemHeap *heap) {
    // replace with your code here
    char *temp = buf;
    void *mem = heap->Allocate(sizeof(Column));
    std::string column_name;
    TypeId type;
    uint32_t length, table_index;
    bool nullable, unique;
    if (column != nullptr) {
        std::cout << "Pointer to column is not null in column deserialize." << std::endl;
    }
    if (MACH_READ_UINT32(temp) != COLUMN_MAGIC_NUM) {
        std::cout << "MAGIC_NUM verified failed" << std::endl;
    }
    temp += sizeof(uint32_t);
    uint32_t strLen = MACH_READ_FROM(uint32_t, temp);
    temp += sizeof(uint32_t);
    for (uint32_t i = 0; i < strLen; i++) {
        char c = MACH_READ_FROM(char, temp);
        temp += sizeof(char);
        column_name += c;
    }
    type = (TypeId) MACH_READ_FROM(uint32_t, temp);
    temp += sizeof(uint32_t);
    length = MACH_READ_FROM(uint32_t, temp);
    temp += sizeof(uint32_t);
    table_index = MACH_READ_FROM(uint32_t, temp);
    temp += sizeof(uint32_t);
    nullable = MACH_READ_FROM(bool, temp);
    temp += sizeof(bool);
    unique = MACH_READ_FROM(bool, temp);
    temp += sizeof(bool);
    //modify
    if (type == kTypeChar)
      column = ALLOC_P(heap,Column)(column_name, type, length, table_index, nullable, unique);
    else if (type == kTypeInt || type == kTypeFloat)
      column = ALLOC_P(heap,Column)(column_name, type, table_index, nullable, unique);
    else
      column = new (mem) Column(nullptr);
    return temp - buf;
}
/*
uint32_t Column::SerializeTo(char *buf) const {
// replace with your code here
char *tmp = buf;
MACH_WRITE_TO(uint32_t, tmp, COLUMN_MAGIC_NUM);
tmp += sizeof(uint32_t);
MACH_WRITE_TO(uint32_t, tmp, name_.size());
tmp += sizeof(uint32_t);
MACH_WRITE_STRING(tmp, name_);
tmp += sizeof(char) * name_.length();
MACH_WRITE_TO(uint32_t, tmp, type_);
tmp += sizeof(uint32_t);
MACH_WRITE_TO(uint32_t, tmp, len_);
tmp += sizeof(uint32_t);
MACH_WRITE_TO(uint32_t, tmp, table_ind_);
tmp += sizeof(uint32_t);
MACH_WRITE_TO(uint32_t, tmp, nullable_);
tmp += sizeof(uint32_t);
MACH_WRITE_TO(uint32_t, tmp, unique_);
return GetSerializedSize();
}

uint32_t Column::GetSerializedSize() const {
// replace with your code here
uint32_t ret_val = 0;
ret_val += sizeof(uint32_t) * 7 + name_.length() * sizeof(char);
return ret_val;
}

uint32_t Column::DeserializeFrom(char *buf, Column *&column, MemHeap *heap) {
// replace with your code here
char *tmp = buf;
if (column != nullptr) std::cerr << "column pointer isn't null" << std::endl;
// deserialize field from buf
void *mem = heap->Allocate(sizeof(Column));

uint32_t val = MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);
if (val!=COLUMN_MAGIC_NUM) std::cerr << "MAGIC_NUM vertify failed" << std::endl;
//read data
uint32_t str_len = MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);

std::string column_name;
for (uint32_t i = 0; i < str_len; i++) {
char ch = MACH_READ_FROM(char, tmp);
tmp += sizeof(char);
column_name += ch;
}

TypeId type = (TypeId)MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);
uint32_t len = MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);
uint32_t col_ind = MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);
bool nullable = (bool)MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);
bool unique = (bool)MACH_READ_FROM(uint32_t, tmp);
tmp += sizeof(uint32_t);

if (type == kTypeChar)
column = new (mem) Column(column_name, type, len, col_ind, nullable, unique);
else if (type == kTypeInt || type == kTypeFloat)
column = new (mem) Column(column_name, type, col_ind, nullable, unique);
else
column = new (mem) Column(nullptr);

return tmp-buf;
}
*/