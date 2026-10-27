// Ordered, deduplicated receive queue for a datagram event stream.
//
// Receipt and completion are deliberately separate: the acknowledgement is the highest contiguous
// sequence actually handed to its feature, not the highest datagram merely copied into memory.
// The owner calls this on its session/simulation thread; it is not a multi-producer queue.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <deque>

namespace reliable {
template <class Record, size_t Capacity = 1024>
class Inbox {
public:
    struct Item { Record record; bool complete; };
    enum Result { accepted, duplicate, gap, full, invalid };

    Inbox() : epoch_(0), received_(0), committed_(0), retired_count_(0) {}

    // A fresh epoch must start at sequence 1. Previously retired epochs cannot roll a stream back
    // when an old datagram arrives late. Call reset() when the enclosing connection is replaced.
    bool epoch(uint32_t value, uint32_t first_sequence) {
        if (!value) return false;
        if (value == epoch_) return true;
        if (first_sequence != 1) return false;
        for (size_t i = 0; i < retired_count_; ++i) if (retired_[i] == value) return false;
        if (epoch_) {
            if (retired_count_ < k_retired) retired_[retired_count_++] = epoch_;
            else {
                for (size_t i = 1; i < k_retired; ++i) retired_[i - 1] = retired_[i];
                retired_[k_retired - 1] = epoch_;
            }
        }
        items_.clear();
        epoch_ = value;
        received_ = committed_ = 0;
        return true;
    }

    Result receive(const Record &record) {
        if (!epoch_ || !record.seq) return invalid;
        if (record.seq <= received_) return duplicate;
        // The sender retransmits gaps; never ACK an event we did not receive.
        if (received_ == UINT32_MAX || record.seq != received_ + 1) return gap;
        if (items_.size() >= Capacity) return full;
        items_.push_back({record, false});
        received_ = record.seq;
        return accepted;
    }

    // Consumers can finish independent control records ahead of a temporarily blocked event; the
    // cumulative ACK still cannot move past that event until it too is complete.
    std::deque<Item> &items() { return items_; }
    uint32_t commit() {
        while (!items_.empty() && items_.front().complete) {
            committed_ = items_.front().record.seq;
            items_.pop_front();
        }
        return committed_;
    }
    void reset() {
        items_.clear();
        epoch_ = received_ = committed_ = 0;
        retired_count_ = 0;
    }
    uint32_t current_epoch() const { return epoch_; }
    uint32_t received() const { return received_; }
    uint32_t committed() const { return committed_; }
    size_t pending() const { return items_.size(); }
    bool empty() const { return items_.empty(); }

private:
    static const size_t k_retired = 8;
    std::deque<Item> items_;
    uint32_t epoch_, received_, committed_, retired_[k_retired];
    size_t retired_count_;
};
} // namespace reliable
