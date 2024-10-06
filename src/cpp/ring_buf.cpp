#include "ring_buf.hpp"

/* Constructor */
RingBuf::RingBuf(RingBufElement sto[], RingBufCtr sto_len)
    : buf(&sto[0]), end(sto_len) {
  std::atomic_store(&head, 0U); // initialize head atomically
  std::atomic_store(&tail, 0U); // initialize tail atomically
}

/* Returns the number of free elements in the buffer */
RingBufCtr RingBuf::num_free() const {
    RingBufCtr h = head.load(std::memory_order_acquire);
    RingBufCtr t = tail.load(std::memory_order_relaxed);
    
    if (h == t) {
        return end - 1;
    }
    else if (h < t) {
        return t - h - 1;
    }
    else {
        return end + t - h - 1;
    }
}

/* Adds an element to the buffer */
bool RingBuf::put(RingBufElement const el) {
    RingBufCtr h = head.load(std::memory_order_relaxed);
    RingBufCtr next_head = h + 1;
    if (next_head == end) {
        next_head = 0;
    }

    RingBufCtr t = tail.load(std::memory_order_acquire);

    if (next_head != t) { // Buffer not full?
        buf[h] = el; // Write element
        head.store(next_head, std::memory_order_release);
        return true;
    }
    else {
        return false; // Buffer full
    }
}

/* Retrieves an element from the buffer */
bool RingBuf::get(RingBufElement& el) {
    RingBufCtr t = tail.load(std::memory_order_relaxed);
    RingBufCtr h = head.load(std::memory_order_acquire);

    if (h != t) { // Buffer not empty?
        el = buf[t];
        RingBufCtr next_tail = t + 1;
        if (next_tail == end) {
            next_tail = 0;
        }
        tail.store(next_tail, std::memory_order_release);
        return true;
    }
    else {
        return false; // Buffer empty
    }
}

/* Processes all elements in the buffer using a handler */
void RingBuf::process_all(RingBufHandler handler) {
    RingBufCtr t = tail.load(std::memory_order_relaxed);
    RingBufCtr h = head.load(std::memory_order_acquire);

    while (h != t) {
        handler(buf[t]);
        RingBufCtr next_tail = t + 1;
        if (next_tail == end) {
            next_tail = 0;
        }
        t = next_tail;
    }
    tail.store(t, std::memory_order_release);
}
