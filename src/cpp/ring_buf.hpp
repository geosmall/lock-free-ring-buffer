#ifndef RING_BUF_HPP
#define RING_BUF_HPP

#include <atomic>
#include <cstdint>

typedef uint16_t RingBufCtr;
typedef uint8_t RingBufElement;

class RingBuf {
public:
    RingBuf(RingBufElement* buffer, RingBufCtr size);
    
    RingBufCtr num_free() const;
    bool put(RingBufElement const el);
    bool get(RingBufElement& el);
    
    using RingBufHandler = void (*)(RingBufElement const el);
    void process_all(RingBufHandler handler);

private:
    RingBufElement* buf;
    RingBufCtr end;
    std::atomic<RingBufCtr> head;
    std::atomic<RingBufCtr> tail;
};

#endif /* RING_BUF_HPP */
