#include <stdexcept>
#include <sys/stat.h>

#include "glog/logging.h"
#include "page/bitmap_page.h"
#include "storage/disk_manager.h"

DiskManager::DiskManager(const std::string &db_file) : file_name_(db_file) {
  std::scoped_lock<std::recursive_mutex> lock(db_io_latch_);
  db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
  // directory or file does not exist
  if (!db_io_.is_open()) {
    db_io_.clear();
    // create a new file
    db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::out);
    db_io_.close();
    // reopen with original mode
    db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
    if (!db_io_.is_open()) {
      throw std::exception();
    }
  }
  ReadPhysicalPage(META_PAGE_ID, meta_data_);
}

void DiskManager::Close() {
  std::scoped_lock<std::recursive_mutex> lock(db_io_latch_);
  if (!closed) {
    db_io_.close();
    closed = true;
  }
}

void DiskManager::ReadPage(page_id_t logical_page_id, char *page_data) {
  ASSERT(logical_page_id >= 0, "Invalid page id.");
  ReadPhysicalPage(MapPageId(logical_page_id), page_data);
}

void DiskManager::WritePage(page_id_t logical_page_id, const char *page_data) {
  ASSERT(logical_page_id >= 0, "Invalid page id.");
  WritePhysicalPage(MapPageId(logical_page_id), page_data);
}

// page_id_t DiskManager::AllocatePage() {
//   ASSERT(false, "Not implemented yet.");
//   return INVALID_PAGE_ID;
// }

// void DiskManager::DeAllocatePage(page_id_t logical_page_id) {
//   ASSERT(false, "Not implemented yet.");
// }

// bool DiskManager::IsPageFree(page_id_t logical_page_id) {
//   return false;
// }

// page_id_t DiskManager::MapPageId(page_id_t logical_page_id) {
//   return 0;
// }

page_id_t DiskManager::AllocatePage() {
  u_int32_t bit_map_id=0;
  while(1)
  {
    char buf[PAGE_SIZE];
    memset(buf, 0, PAGE_SIZE);
    u_int32_t page_offset;
    this->ReadPhysicalPage(bit_map_id*(this->BITMAP_SIZE+1)+1,buf);
    BitmapPage<PAGE_SIZE> *bit_map = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buf);
    if(bit_map->AllocatePage(page_offset))
    {
      this->WritePhysicalPage(bit_map_id*(this->BITMAP_SIZE+1)+1,buf);
      DiskFileMetaPage *m_data = reinterpret_cast <DiskFileMetaPage *> (this->meta_data_);
      if(bit_map_id>=m_data->num_extents_)m_data->num_extents_++;
      m_data->num_allocated_pages_++;
      m_data->extent_used_page_[bit_map_id]++;
      return page_offset+this->BITMAP_SIZE*bit_map_id;
    }
    else bit_map_id++;
  }
  return INVALID_PAGE_ID;
}

void DiskManager::DeAllocatePage(page_id_t logical_page_id) {
  page_id_t physical_id=this->MapPageId(logical_page_id);
  page_id_t bitmap_id=physical_id;
  char buf[PAGE_SIZE];
  memset(buf, 0, PAGE_SIZE);
  while(bitmap_id%(this->BITMAP_SIZE+1)!=1)bitmap_id--;
  u_int32_t page_offset = physical_id-bitmap_id-1;
  this->ReadPhysicalPage(bitmap_id,buf);
  BitmapPage<PAGE_SIZE> *bit_map = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buf);
  if(bit_map->DeAllocatePage(page_offset))
  {
    this->WritePhysicalPage(bitmap_id,buf);
    DiskFileMetaPage *m_data = reinterpret_cast <DiskFileMetaPage *> (this->meta_data_);
    m_data->num_allocated_pages_--;
    m_data->extent_used_page_[(bitmap_id-1)/this->BITMAP_SIZE]--;
  }
  return;
}
//2 N+1 N+3 N+4 N+5
bool DiskManager::IsPageFree(page_id_t logical_page_id) {
  page_id_t physical_id=this->MapPageId(logical_page_id);
  page_id_t bitmap_id=physical_id;
  char buf[PAGE_SIZE];
  memset(buf, 0, PAGE_SIZE);
  while(bitmap_id%this->BITMAP_SIZE!=1)bitmap_id--;
  u_int32_t page_offset = physical_id-bitmap_id;
  this->ReadPhysicalPage(bitmap_id,buf);
  BitmapPage<PAGE_SIZE> *bit_map = reinterpret_cast<BitmapPage<PAGE_SIZE> *>(buf);
  if(bit_map->IsPageFree(page_offset))return true;
  else return false;
}

page_id_t DiskManager::MapPageId(page_id_t logical_page_id) {
  return logical_page_id+2+logical_page_id/this->BITMAP_SIZE;
}

int DiskManager::GetFileSize(const std::string &file_name) {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

void DiskManager::ReadPhysicalPage(page_id_t physical_page_id, char *page_data) {
  int offset = physical_page_id * PAGE_SIZE;
  // check if read beyond file length
  if (offset >= GetFileSize(file_name_)) {
#ifdef ENABLE_BPM_DEBUG
    LOG(INFO) << "Read less than a page" << std::endl;
#endif
    memset(page_data, 0, PAGE_SIZE);
  } else {
    // set read cursor to offset
    db_io_.seekp(offset);
    db_io_.read(page_data, PAGE_SIZE);
    // if file ends before reading PAGE_SIZE
    int read_count = db_io_.gcount();
    if (read_count < PAGE_SIZE) {
#ifdef ENABLE_BPM_DEBUG
      LOG(INFO) << "Read less than a page" << std::endl;
#endif
      memset(page_data + read_count, 0, PAGE_SIZE - read_count);
    }
  }
}

void DiskManager::WritePhysicalPage(page_id_t physical_page_id, const char *page_data) {
  size_t offset = static_cast<size_t>(physical_page_id) * PAGE_SIZE;
  // set write cursor to offset
  db_io_.seekp(offset);
  db_io_.write(page_data, PAGE_SIZE);
  // check for I/O error
  if (db_io_.bad()) {
    LOG(ERROR) << "I/O error while writing";
    return;
  }
  // needs to flush to keep disk file in sync
  db_io_.flush();
}