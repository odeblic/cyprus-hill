#pragma once

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

template <typename BufferType>
std::string bufferToString(BufferType const& buffer, size_t count = 0)
{
    if (count == 0)
    {
        return std::string(buffer.getData(), buffer.getSize());
    }
    else
    {
        return std::string(buffer.getData(), count);
    }
}

class ByteBufferRO
{
public:
    ByteBufferRO(std::string content)
    : content(content)
    {
    }

    char const * getData() const
    {
        return content.data();
    }

    size_t getSize() const
    {
        return content.size();
    }

    // TODO: move to some generic class
    std::string toString(size_t count = 0)
    {
        return bufferToString(*this, count);
    }

private:
    std::string content;
};

template <size_t Size>
class ByteBufferRW
{
public:
    char * getData() const
    {
        return array;
    }

    size_t getSize() const
    {
        return Size;
    }

    // TODO: move to some generic class
    std::string toString(size_t count = 0)
    {
        return bufferToString(*this, count);
    }

private:
    mutable char array[Size];
};

// TODO: add buffer types to be more flexible

#if 0
template <typename Type, size_t Size>
class BufferStatic
{
public:
    Type * getData() const
    {
        return array;
    }

    size_t getSize() const
    {
        return Size;
    }

private:
    Type array[Size];
};

template <typename Type>
class BufferDynamic
{
public:
    BufferDynamic(size_t size)
    : ptr(new Type[size])
    {
    }

    BufferDynamic(char const * data, size_t size)
    : BufferDynamic(size)
    {
        memcpy(ptr.get(), data, size);
    }

    Type * getData() const
    {
        return ptr.get();
    }

    size_t getSize() const
    {
        return size;
    }

private:
    std::unique_ptr<Type[]> ptr;
    size_t size{0};
};

/*
template <typename Type>
class BufferView
{
public:
    BufferView(char const * addr, size_t size)
    : addr(addr), size(size)
    {
    }

    Type * getData() const
    {
        return addr;
    }

    size_t getSize() const
    {
        return size;
    }

private:
    Type * addr{nullptr};
    size_t size{0};
};

template <bool Constant, typename Type = typename std::conditional<Constant, char const, char>::type, typename Base>
class BufferX : public Base
{

};

auto x = BufferX<true, BufferView>{};
*/

template <bool Constant,
          size_t Size = 0,
          typename Type = typename std::conditional<Constant, char const, char>::type,
          typename Base = typename std::conditional<Size == 0, BufferDynamic<Type>, BufferStatic<Type, Size>>::type
          >
class Buffer : public Base
{
public:
    template<typename... Args>
    Buffer(Args&&... args)
    : Base(std::forward<Args>(args)...)
    {
    }
};

// new
static auto make(size_t size)
{
    return Buffer<false, 0>{size};
}

static auto make(std::string const& text)
{
    auto buffer = Buffer<false, 0>{text.size()};
    memcpy(buffer.getData(), text.c_str(), text.size());
    return buffer;
    //return Buffer<false, 0>{text.c_str(), text.length()};
}

#if 0
// view
template <typename BufferType>
static auto make(BufferType buffer)
{
    return Buffer<false, 0>{buffer.getData(), buffer.getSize()};
}

// copy
template <typename BufferType>
static auto make(BufferType const& other)
{
    auto buffer = make(other.size);
    memcpy(buffer.addr, other.addr, other.size);
    return buffer;
}

// const
template <typename Type, size_t Size>
static auto make(Type text[Size])
{
    return Buffer<true, Size>{text};
}
#endif

/*
struct Buffer
{
    char * addr{nullptr};
    char * writePtr{nullptr};
    char * readPtr{nullptr};
    size_t writtenSize{0};
    size_t size{0};
    size_t totalsize{0};
};
*/

template <typename BufferType>
auto stringToBuffer(std::string str)
{
    BufferType buffer;
    return buffer;
}
#endif
