// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_ZONE_ACCOUNTING_ALLOCATOR_H_
#define V8_ZONE_ACCOUNTING_ALLOCATOR_H_

#include <atomic>

#include "src/base/macros.h"
#include "src/logging/tracing-flags.h"

namespace v8 {
namespace internal {

class Segment;
class Zone;

class V8_EXPORT_PRIVATE AccountingAllocator {
 public:
  AccountingAllocator() = default;
  virtual ~AccountingAllocator();

  // Allocates a new segment. Returns nullptr on failed allocation.
  Segment* AllocateSegment(size_t bytes);

  // Return unneeded segments to either insert them into the pool or release
  // them if the pool is already full or memory pressure is high.
  void ReturnSegment(Segment* memory);

  size_t GetCurrentMemoryUsage() const {
    return current_memory_usage_.load(std::memory_order_relaxed);
  }

  size_t GetMaxMemoryUsage() const {
    return max_memory_usage_.load(std::memory_order_relaxed);
  }

  void TraceZoneCreation(const Zone* zone) {
    if (V8_LIKELY(!TracingFlags::is_zone_stats_enabled())) return;
    TraceZoneCreationImpl(zone);
  }

  void TraceZoneDestruction(const Zone* zone) {
    if (V8_LIKELY(!TracingFlags::is_zone_stats_enabled())) return;
    TraceZoneDestructionImpl(zone);
  }

  void TraceAllocateSegment(Segment* segment) {
    if (V8_LIKELY(!TracingFlags::is_zone_stats_enabled())) return;
    TraceAllocateSegmentImpl(segment);
  }

 protected:
  virtual void TraceZoneCreationImpl(const Zone* zone) {}
  virtual void TraceZoneDestructionImpl(const Zone* zone) {}
  virtual void TraceAllocateSegmentImpl(Segment* segment) {}

 private:
  std::atomic<size_t> current_memory_usage_{0};
  std::atomic<size_t> max_memory_usage_{0};

  DISALLOW_COPY_AND_ASSIGN(AccountingAllocator);
};

}  // namespace internal
}  // namespace v8

#endif  // V8_ZONE_ACCOUNTING_ALLOCATOR_H_
