#include "buffer/lru_replacer.h"

LRUReplacer::LRUReplacer(size_t num_pages) : capacity_(num_pages){

}

LRUReplacer::~LRUReplacer() {
  lst_.clear();
}

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::scoped_lock lock{latch_};
  if (lst_.empty()) return false;
  
  // The first element is the most recent used
  // and the last element is the least recent used  
  // so we need to pop the last element and return it to the caller
  *frame_id = lst_.back();
  cache_.erase(*frame_id);
  lst_.pop_back();

  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::scoped_lock lock{latch_};
  if (cache_.find(frame_id) == cache_.end()) {
    return;
  }
  // if the page is pinned, we need to remove it from the lst_
  auto it = cache_[frame_id];
  lst_.erase(it);
  cache_.erase(frame_id);
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::scoped_lock lock{latch_};

  if (cache_.count(frame_id) != 0 || lst_.size() == static_cast<size_t>(capacity_)) {
    return;
  }
  // if the page is unpinned, we need to add it to the lst_ and cache_
  lst_.push_front(frame_id);
  cache_.emplace(frame_id, lst_.begin());
}

size_t LRUReplacer::Size() {
  return lst_.size();
}