#ifndef TOMATO_STRING_HH_
#define TOMATO_STRING_HH_

#include "core.hh"
#include "vector.hh"

// Custom string stuff here
// TODO: float to string converstion -> https://github.com/ulfjack/ryu/tree/master/ryu
// not a trivial thing to implement, but my Handmade spirit wills me so
// TODO: c++ stream overload. I don't know if I want to use streams
// TODO: fix the operator+ overloads and write some tests

namespace tom
{

class string
{
public:
    string();
    string(const char *str);
    string(char *buf);
    string(char *buf, u32 len);
    ~string();

    string(const string &other);
    string &operator=(const string &rhs);

    string(string &&other);
    string &operator=(string &&rhs);

    string operator+(const string &rhs);
    string operator+(const char *rhs);
    string operator+(char rhs);

    void clear();

    u32 len() const { return _size - 1; }
    const char *c_str() const { return _buf; }
    char front() const { return _buf[0]; }
    char back() const { return _buf[_size - 1]; }

    char &operator[](szt i)
    {
        TOM_ASSERT(i < _size);
        return _buf[i];
    }

    char const &operator[](szt i) const
    {
        TOM_ASSERT(i < _size);
        return _buf[i];
    }

    friend string operator+(const string &lhs, const string &rhs);

private:
    char *_buf;
    u32 _size;  // NOTE: with null terminator
};

// NOTE: mostly for win32 stuff
class wstring
{
public:
    wstring();
    wstring(const wchar *str);
    wstring(wchar *buf);
    wstring(wchar *buf, u32 len);
    ~wstring();

    wstring(const wstring &other);
    wstring &operator=(const wstring &rhs);

    wstring(wstring &&other);
    wstring &operator=(wstring &&rhs);

    wstring operator+(const wstring &rhs);
    wstring operator+(const wchar *rhs);
    wstring operator+(wchar rhs);

    void clear();

    u32 len() const { return _size - 1; }
    const wchar *c_str() const { return _buf; }
    wchar front() const { return _buf[0]; }
    wchar back() const { return _buf[_size - 1]; }

    friend wstring operator+(const wstring &lhs, const wstring &rhs);

private:
    wchar *_buf;
    u32 _size;  // NOTE: with null terminator
};

// wrapper around a vector to easily build strings
// TODO: add more + overloads
// TODO: add << overloads?
class string_stream
{
public:
    string_stream();
    string_stream(char c);
    string_stream(const char *str);
    string_stream(const string &str);
    string_stream(const wstring &wstr);
    ~string_stream();

    void push_back(char c);
    void push_back(const char *str);
    void push_back(const string &str);
    void push_back(const wstring &wstr);

    void push_back(s64 n);
    void push_back(s32 n);
    void push_back(s16 n);
    void push_back(s8 n);

    void push_back(u64 n);
    void push_back(u32 n);
    void push_back(u16 n);
    void push_back(u8 n);

    void pop_back() { _buf.pop_back(); }

    void reserve(szt new_capacity) { _buf.reserve(new_capacity); }
    void clear() { _buf.clear(); }

    string to_string();
    wstring to_wstring();

    szt size() const { return _buf.size(); }
    szt capacity() const { return _buf.capacity(); }
    bool empty() const { return _buf.empty(); }
    char back() { _buf.back(); }
    void *data() { return (void *)_buf.data(); }
    void *data() const { return (void *)_buf.data(); }

    char *begin() { return _buf.begin(); }
    char *end() { return _buf.end(); }
    char const *begin() const { return _buf.begin(); }
    char const *end() const { return _buf.end(); }

    char &operator[](szt i)
    {
        TOM_ASSERT(i < _buf.size());
        return reinterpret_cast<char *>(_buf.data())[i];
    }

    char const &operator[](szt i) const
    {
        TOM_ASSERT(i < _buf.size());
        return reinterpret_cast<char *>(_buf.data())[i];
    }

    friend string_stream operator+(const string_stream &lhs, char rhs);
    friend string_stream operator+(const string_stream &lhs, const char *rhs);
    friend string_stream operator+(const string_stream &lhs, const string &rhs);
    friend string_stream operator+(const string_stream &lhs, const wstring &rhs);

    void operator+=(char rhs);
    void operator+=(const char *rhs);
    void operator+=(const string &rhs);
    void operator+=(const wstring &rhs);

private:
    vector<char> _buf;
};

string operator+(const string &lhs, const string &rhs);
wstring operator+(const wstring &lhs, const wstring &rhs);

string_stream operator+(const string_stream &lhs, char rhs);
string_stream operator+(const string_stream &lhs, const char *rhs);
string_stream operator+(const string_stream &lhs, const string &rhs);
string_stream operator+(const string_stream &lhs, const wstring &rhs);

string convert_wstring_to_string(const wstring &wstr);
wstring convert_string_to_wstring(const string &str);
wstring convert_string_to_wstring_utf8(const string &str);
string convert_wstring_to_string_utf8(const wstring &wstr);

string to_string(s8 n);
string to_string(s16 n);
string to_string(s32 n);
string to_string(s64 n);

string to_string(u64 n);
string to_string(u32 n);
string to_string(u16 n);
string to_string(u16 n);

}  // namespace tom

#endif  // !TOMATO_STRING_HH_