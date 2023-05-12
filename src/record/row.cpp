#include "record/row.h"

uint32_t Row::SerializeTo(char *buf, Schema *schema) const {
    // magic_num
    char *temp = buf;
    MACH_WRITE_UINT32(temp, ROW_MAGIC_NUM);
    temp = temp + sizeof(uint32_t);
    // header
    MACH_WRITE_UINT32(temp, fields_.size());
    temp += sizeof(uint32_t);
    size_t i;
    for (i = 0; i < fields_.size(); i++) {
        MACH_WRITE_TO(bool, temp, fields_[i]->IsNull());
        temp += sizeof(bool);
    }
    // fields
    for (i = 0; i < fields_.size(); i++) {
        if (!fields_[i]->IsNull()) {
            fields_[i]->SerializeTo(temp);
            temp += fields_[i]->GetSerializedSize();
        }
    }
    return temp - buf;
}

uint32_t Row::DeserializeFrom(char *buf, Schema *schema) {
    // replace with your code here
    char *temp = buf;
    if (MACH_READ_UINT32(temp) == ROW_MAGIC_NUM) {
        temp += sizeof(uint32_t);
        uint32_t size = MACH_READ_UINT32(temp);
        temp += sizeof(uint32_t);
        bool *null_bitMap = new bool[size];
        for (size_t i = 0; i < size; i++) {
            null_bitMap[i] = MACH_READ_FROM(bool, temp);
            temp += sizeof(bool);
        }
        for (size_t i = 0; i < size; i++) {
            fields_.push_back(nullptr);
            if (!null_bitMap[i]) {
                fields_[i]->DeserializeFrom(temp, schema->GetColumn(i)->GetType(), &(fields_[i]), false, heap_);
                temp += fields_[i]->GetSerializedSize();
            }
            /*
            if (null_map[i] == false) {
                fields_.push_back(nullptr);
                fields_[i]->DeserializeFrom(tmp, schema->GetColumn(i)->GetType(), &(fields_[i]), false, heap_);
                tmp += fields_[i]->GetSerializedSize();
            }
            else {
                void *mem = heap_->Allocate(sizeof(Field));
                new(mem) Field(schema->GetColumn(i)->GetType());
                fields_.push_back((Field *) mem);
            }
            */
        }
        delete[] null_bitMap;
    }
    return temp - buf;
}

uint32_t Row::GetSerializedSize(Schema *schema) const {
    // replace with your code here
    uint32_t serialize_size = 0;
    // sizeof(uint32_t)*2 is sizeof(magic_num) and sizeof(size)
    serialize_size += sizeof(uint32_t) * 2 + sizeof(bool) * fields_.size();
    size_t i;
    for (i = 0; i < fields_.size(); i++)
        if (!fields_[i]->IsNull()) serialize_size += fields_[i]->GetSerializedSize();
    return serialize_size;
}
