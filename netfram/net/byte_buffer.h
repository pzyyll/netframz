//
// @Create by Caizhili on 20170905
// @Brief
//

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cstring>

//TODO

class ByteBuffer {
public:
    const static size_t kDefaultSize = 4096;
    const static size_t kMaxLimitSize = 1000000;

    ByteBuffer();

    ByteBuffer(size_t size);

    ByteBuffer(const ByteBuffer &buf);

    size_t Capacity();

    size_t Size();

    size_t ReadableSize();

    /*
     * @Brief just increase
     */
    void Reserve(size_t size);

    const char *Content();

    void ReadSkip(size_t skip);

    void Append(const void *src, size_t len);

    size_t rpos();

    size_t wpos();

protected:
    size_t rpos_, wpos_;
    std::vector<char> storage_;
}
