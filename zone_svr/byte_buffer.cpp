//
// @Crete by Caizhili on 20170905
// @Brief TODO
//

#include "byte_buffer.h"

ByteBuffer::ByteBuffer()
    : rpos_(0), wpos_(0) {
    storage_.reserve(kDefaultSize);
}

ByteBuffer::ByteBuffer(size_t size)
    : rpos_(0), wpos_(0) {
    storage_.reserve(size);
}

ByteBuffer::ByteBuffer(const ByteBuffer &buf)
    : rpos_(buf.rpos_), wpos_(buf.wpos_), storage_(buf.storage_) {

}

size_t ByteBuffer::Capacity() {
    return storage_.capacity();
}

size_t ByteBuffer::Size() {
    return storage_.size();
}

size_t ReadableSize() {
    return (storage_.size() - rpos_);
}

void ByteBuffer::Reserve(size_t size) {
    if (storage_.size() < size)
        storage_.reserve(size);
}

const char *ByteBuffer::Content() {
    return &storage_[0];
}

void ByteBuffer::ReadSkip(size_t skip) {
    if (rpos_ + skip > storage_.size())
        return;

    rpos_ += skip;
}

void ByteBuffer::Append(const void *src, size_t len) {
    if (!len)
        return;

    if (kMaxLimitSize < wpos_ + len )
        return;

    if (storage_.size() < wpos_ + len)
        storage_.resize(wpos_ + len);

    memcpy(&storage_[wpos_], src, len);
    wpos_ += len;
}

size_t ByteBuffer::rpos() {
    return rpos_;
}

size_t ByteBuffer::wpos() {
    return wpos_;
}
