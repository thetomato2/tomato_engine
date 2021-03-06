#include "string.hh"

namespace tom
{
internal char parse_digit(s32 d)
{
    TOM_ASSERT(d < 10);

    switch (d) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        default: break;
    }
    return '\0';
}

inline bool str_equal(const char *a, const char *b)
{
    while (*a && *a == *b) {
        ++a;
        ++b;
    }
    if (*a != *b) return false;
    return true;
}

// TODO: vectorize this
internal u32 str_len(const char *str)
{
    const char *s;
    for (s = str; *s; ++s)
        ;
    u32 res = (u32)(s - str);
    return res;
}

// TODO: vectorize this
internal u32 str_len(const wchar *str)
{
    const wchar *s;
    for (s = str; *s; ++s)
        ;
    u32 res = (u32)(s - str);
    return res;
}

internal char *str_copy(const char *str)
{
    u32 len      = str_len(str);
    char *result = (char *)malloc(sizeof(char) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + i);
    }
    result[len] = '\0';
}

internal char *str_copy(const char *str, u32 len)
{
    char *result = (char *)malloc(sizeof(char) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + i);
    }
    result[len] = '\0';

    return result;
}

internal char *str_copy(char *buf1, char *buf2)
{
    u32 len1       = str_len(buf1);
    u32 len2       = str_len(buf2);
    char *result   = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
    char *res_ptr  = result;
    char *buf1_ptr = buf1;
    char *buf2_ptr = buf2;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    while (*buf2_ptr) {
        *res_ptr++ = *buf2_ptr++;
    }
    *res_ptr = '\0';

    return result;
}

internal char *str_copy(char *buf1, char *buf2, u32 len1, u32 len2)
{
    char *result   = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
    char *res_ptr  = result;
    char *buf1_ptr = buf1;
    char *buf2_ptr = buf2;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    while (*buf2_ptr) {
        *res_ptr++ = *buf2_ptr++;
    }
    *res_ptr = '\0';

    return result;
}

internal char *str_copy(char *buf1, char c, u32 len1)
{
    char *result   = (char *)malloc(sizeof(char) * (len1 + 2));
    char *res_ptr  = result;
    char *buf1_ptr = buf1;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    *res_ptr++ = c;
    *res_ptr   = '\0';

    return result;
}

internal char *str_copy(char *buf1, const char *str)
{
    u32 len1       = str_len(buf1);
    u32 len2       = str_len(str);
    char *result   = (char *)malloc(sizeof(char) * (len1 + len2 + 1));
    char *res_ptr  = result;
    char *buf1_ptr = buf1;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    for (u32 i = 0; i < len2; ++i) {
        *res_ptr++ = *(str + i);
    }
    *res_ptr = '\0';

    return result;
}

internal char *rev_str(const char *str)
{
    u32 len      = str_len(str);
    char *result = (char *)malloc(sizeof(char) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + (len - i - 1));
    }
    result[len] = '\0';

    return result;
}

internal string_stream rev_ss(const string_stream &ss)
{
    string_stream rss(ss.size());
    for (s32 i = ss.size() - 1; i >= 0; --i) {
        rss.push_back(ss[i]);
    }

    return rss;
}

internal wchar *rev_str(const wchar *str)
{
    u32 len       = str_len(str);
    wchar *result = (wchar *)malloc(sizeof(wchar) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + (len - i - 1));
    }
    result[len] = '\0';

    return result;
}

internal wchar *str_copy(const wchar *str)
{
    u32 len       = str_len(str);
    wchar *result = (wchar *)malloc(sizeof(wchar) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + i);
    }
    result[len] = '\0';
}

internal wchar *str_copy(const wchar *str, u32 len)
{
    wchar *result = (wchar *)malloc(sizeof(wchar) * (len + 1));
    for (u32 i = 0; i < len; ++i) {
        result[i] = *(str + i);
    }
    result[len] = '\0';

    return result;
}

internal wchar *str_copy(wchar *buf1, wchar *buf2)
{
    u32 len1        = str_len(buf1);
    u32 len2        = str_len(buf2);
    wchar *result   = (wchar *)malloc(sizeof(wchar) * (len1 + len2 + 1));
    wchar *res_ptr  = result;
    wchar *buf1_ptr = buf1;
    wchar *buf2_ptr = buf2;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    while (*buf2_ptr) {
        *res_ptr++ = *buf2_ptr++;
    }
    *res_ptr = '\0';

    return result;
}

internal wchar *str_copy(wchar *buf1, wchar *buf2, u32 len1, u32 len2)
{
    wchar *result   = (wchar *)malloc(sizeof(wchar) * (len1 + len2 + 1));
    wchar *res_ptr  = result;
    wchar *buf1_ptr = buf1;
    wchar *buf2_ptr = buf2;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    while (*buf2_ptr) {
        *res_ptr++ = *buf2_ptr++;
    }
    *res_ptr = '\0';

    return result;
}

internal wchar *str_copy(wchar *buf1, const wchar *str)
{
    u32 len1        = str_len(buf1);
    u32 len2        = str_len(str);
    wchar *result   = (wchar *)malloc(sizeof(wchar) * (len1 + len2 + 1));
    wchar *res_ptr  = result;
    wchar *buf1_ptr = buf1;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    for (u32 i = 0; i < len2; ++i) {
        *res_ptr++ = *(str + i);
    }
    *res_ptr = '\0';

    return result;
}

internal wchar *str_copy(wchar *buf1, wchar c, u32 len1)
{
    wchar *result   = (wchar *)malloc(sizeof(wchar) * (len1 + 2));
    wchar *res_ptr  = result;
    wchar *buf1_ptr = buf1;
    while (*buf1_ptr) {
        *res_ptr++ = *buf1_ptr++;
    }
    *res_ptr++ = c;
    *res_ptr   = '\0';

    return result;
}

string convert_wstring_to_string(const wstring &wstr)
{
    s32 cnt   = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    char *buf = (char *)malloc(sizeof(char) * cnt);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buf, cnt, NULL, NULL);
    // TODO: direct access to string buffer
    string result(buf);

    return result;
}

wstring convert_string_to_wstring(const string &str)
{
    s32 cnt    = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar *buf = (wchar *)malloc(sizeof(char) * cnt);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buf, cnt);
    // TODO: direct access to string buffer
    wstring result(buf);

    return result;
}

string convert_wstring_to_string_utf8(const wstring &wstr)
{
    s32 cnt   = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.len(), NULL, 0, NULL, NULL);
    char *buf = (char *)malloc(sizeof(char) * cnt);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buf, cnt, NULL, NULL);
    // TODO: direct access to string buffer
    string result(buf);

    return result;
}

wstring convert_string_to_wstring_utf8(const string &str)
{
    s32 cnt    = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.len(), NULL, 0);
    wchar *buf = (wchar *)malloc(sizeof(char) * cnt);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buf, cnt);
    // TODO: direct access to string buffer
    wstring result(buf);

    return result;
}

string::string() : _buf(nullptr), _size(0)
{
}

string::string(char c)
{
    u32 len = 1;
    _size   = 2;
    _buf    = (char *)malloc(sizeof(char) * 2);
    _buf[0] = c;
    _buf[1] = '\0';
}

string::string(const char *str)
{
    u32 len = str_len(str);
    _buf    = str_copy(str, len);
    _size   = len + 1;
}

string::string(char *buf)
{
    u32 len = str_len(buf);
    _buf    = str_copy(buf, len);
    _size   = len + 1;
}

string::string(char *buf, u32 len)
{
    _buf  = str_copy(buf, len);
    _size = len + 1;
}

string::~string()
{
    clear();
}

string::string(const string &other)
{
    _size = other._size;
    _buf  = str_copy(other._buf, _size - 1);
}

string &string::operator=(const string &rhs)
{
    clear();
    _size = rhs._size;
    _buf  = str_copy(rhs._buf, _size - 1);

    return *this;
}

string::string(string &&other)
{
    _size      = other._size;
    _buf       = other._buf;
    other._buf = nullptr;
}

string &string::operator=(string &&rhs)
{
    clear();
    _size    = rhs._size;
    _buf     = rhs._buf;
    rhs._buf = nullptr;

    return *this;
}

void string::append(char c)
{
    ++_size;
    char *temp = _buf;
    _buf       = str_copy(_buf, c, _size);
    free(temp);
}

void string::append(const char *str)
{
    _size      = _size + str_len(str) - 1;
    char *temp = _buf;
    _buf       = str_copy(_buf, str);
    free(temp);
}

void string::append(const string &str)
{
    _size      = _size + str._size - 1;
    char *temp = _buf;
    _buf       = str_copy(_buf, str._buf, _size - 1, str._size - 1);
    free(temp);
}

void string::append(const wstring &wstr)
{
    string str = convert_wstring_to_string(wstr);
    _size      = _size + str._size - 1;
    char *temp = _buf;
    _buf       = str_copy(_buf, str._buf, _size - 1, str._size - 1);
    free(temp);
}

string operator+(const string &lhs, char rhs)
{
    string result;
    result._size = lhs._size + 1;
    result._buf  = str_copy(lhs._buf, rhs, lhs._size);

    return result;
}

string operator+(const string &lhs, const char *rhs)
{
    string result;
    result._size = lhs._size + str_len(rhs) - 1;
    result._buf  = str_copy(lhs._buf, rhs);

    return result;
}

string operator+(const string &lhs, const string &rhs)
{
    string result;
    result._size = lhs._size + rhs._size - 1;
    result._buf  = str_copy(lhs._buf, rhs._buf, lhs._size - 1, rhs._size - 1);

    return result;
}

string operator+(const string &lhs, const wstring &rhs)
{
    string result;
    string str_rhs = convert_wstring_to_string(rhs);
    result._size   = lhs._size + str_rhs._size - 1;
    result._buf    = str_copy(lhs._buf, str_rhs._buf, lhs._size - 1, str_rhs._size - 1);

    return result;
}

void string::operator+=(char rhs)
{
    append(rhs);
}

void string::operator+=(const char *rhs)
{
    append(rhs);
}

void string::operator+=(const string &rhs)
{
    append(rhs);
}

void string::operator+=(const wstring &rhs)
{
    append(rhs);
}

void string::clear()
{
    if (_buf != nullptr) {
        free(_buf);
        _buf = nullptr;
    }
    _size = 0;
}

void string::pop_back()
{
    if (_size > 0) {
        char *temp = _buf;
        _buf       = str_copy(_buf, _size - 2);
        free(temp);
    }
}

wstring::wstring() : _buf(nullptr), _size(0)
{
}

wstring::wstring(wchar c)
{
    u32 len = 1;
    _size   = 2;
    _buf    = (wchar *)malloc(sizeof(wchar) * 2);
    _buf[0] = c;
    _buf[1] = L'\0';
}

wstring::wstring(const wchar *str)
{
    u32 len = str_len(str);
    _buf    = str_copy(str, len);
    _size   = len + 1;
}

wstring::wstring(wchar *buf)
{
    u32 len = str_len(buf);
    _buf    = str_copy(buf, len);
    _size   = len + 1;
}

wstring::~wstring()
{
    clear();
}

wstring::wstring(const wstring &other)
{
    _size = other._size;
    _buf  = str_copy(other._buf, _size - 1);
}

wstring &wstring::operator=(const wstring &rhs)
{
    clear();
    _size = rhs._size;
    _buf  = str_copy(rhs._buf, _size - 1);

    return *this;
}

wstring::wstring(wstring &&other)
{
    _size      = other._size;
    _buf       = other._buf;
    other._buf = nullptr;
}

wstring &wstring::operator=(wstring &&rhs)
{
    clear();
    _size    = rhs._size;
    _buf     = rhs._buf;
    rhs._buf = nullptr;

    return *this;
}

void wstring::append(wchar c)
{
    ++_size;
    wchar *temp = _buf;
    _buf        = str_copy(_buf, c, _size);
    free(temp);
}

void wstring::append(const wchar *str)
{
    _size       = _size + str_len(str) - 1;
    wchar *temp = _buf;
    _buf        = str_copy(_buf, str);
    free(temp);
}

void wstring::append(const string &str)
{
    wstring wstr = convert_string_to_wstring(str);
    _size        = _size + wstr._size - 1;
    wchar *temp  = _buf;
    _buf         = str_copy(_buf, wstr._buf, _size - 1, wstr._size - 1);
    free(temp);
}

void wstring::append(const wstring &wstr)
{
    _size       = _size + wstr._size - 1;
    wchar *temp = _buf;
    _buf        = str_copy(_buf, wstr._buf, _size - 1, wstr._size - 1);
    free(temp);
}

void wstring::clear()
{
    if (_buf != nullptr) {
        free(_buf);
        _buf = nullptr;
    }
    _size = 0;
}

wstring operator+(const wstring &lhs, wchar rhs)
{
    wstring result;
    result._size = lhs._size + 1;
    result._buf  = str_copy(lhs._buf, rhs, lhs._size);

    return result;
}

wstring operator+(const wstring &lhs, const wchar *rhs)
{
    wstring result;
    result._size = lhs._size + str_len(rhs) - 1;
    result._buf  = str_copy(lhs._buf, rhs);

    return result;
}

wstring operator+(const wstring &lhs, const wstring &rhs)
{
    wstring result;
    result._size = lhs._size + rhs._size - 1;
    result._buf  = str_copy(lhs._buf, rhs._buf, lhs._size - 1, rhs._size - 1);

    return result;
}

wstring operator+(const wstring &lhs, const string &rhs)
{
    wstring result;
    wstring str_rhs = convert_string_to_wstring(rhs);
    result._size    = lhs._size + str_rhs._size - 1;
    result._buf     = str_copy(lhs._buf, str_rhs._buf, lhs._size - 1, str_rhs._size - 1);

    return result;
}

void wstring::operator+=(wchar rhs)
{
    append(rhs);
}

void wstring::operator+=(const wchar *rhs)
{
    append(rhs);
}

void wstring::operator+=(const string &rhs)
{
    append(rhs);
}

void wstring::operator+=(const wstring &rhs)
{
    append(rhs);
}

string to_string(u64 n)
{
    string_stream ss;

    if (n == 0llu) {
        ss.push_back('0');
    } else {
        while (n > 0) {
            s32 t = n % 10;
            ss.push_back(parse_digit(t));
            n /= 10;
        }

        ss = rev_ss(ss);
    }

    return ss.to_string();
}

string to_string(s64 n)
{
    bool neg = n < 0;
    if (neg) {
        n *= -1;
    }

    string_stream ss;

    if (n == 0) {
        ss.push_back('0');
    } else {
        while (n > 0) {
            s32 t = n % 10;
            ss.push_back(parse_digit(t));
            n /= 10;
        }

        if (neg) ss.push_back('-');
        ss = rev_ss(ss);
    }

    return ss.to_string();
}

string to_string(s32 n)
{
    return to_string((s64)n);
}

string to_string(s16 n)
{
    return to_string((s64)n);
}

string to_string(s8 n)
{
    return to_string((s64)n);
}

string to_string(u32 n)
{
    return to_string((u64)n);
}

string to_string(u16 n)
{
    return to_string((u64)n);
}

string to_string(u8 n)
{
    return to_string((u64)n);
}

string_stream::string_stream()
{
}

string_stream::string_stream(char c)
{
    push_back(c);
}

string_stream::string_stream(const char *str)
{
    push_back(str);
}

string_stream::string_stream(const string &str)
{
    push_back(str);
}

string_stream::string_stream(const wstring &wstr)
{
    push_back(wstr);
}

string_stream::~string_stream()
{
}

void string_stream::push_back(char c)
{
    _buf.push_back(c);
}

void string_stream::push_back(const char *str)
{
    u32 len = str_len(str);
    for (u32 i = 0; i < len; ++i) {
        _buf.push_back(str[i]);
    }
}
void string_stream::push_back(const string &str)
{
    for (u32 i = 0; i < str.len(); ++i) {
        _buf.push_back(str[i]);
    }
}

void string_stream::push_back(const wstring &wstr)
{
    string str = convert_wstring_to_string(wstr);
    for (u32 i = 0; i < str.len(); ++i) {
        _buf.push_back(str[i]);
    }
}
string_stream operator+(const string_stream &lhs, char rhs)
{
    string_stream result;
    for (char c : lhs) {
        result.push_back(c);
    }
    result.push_back(rhs);

    return result;
}

string_stream operator+(const string_stream &lhs, const char *rhs)
{
    string_stream result;
    for (char c : lhs) {
        result.push_back(c);
    }
    result.push_back(rhs);

    return result;
}

string_stream operator+(const string_stream &lhs, const string &rhs)
{
    string_stream result;
    for (char c : lhs) {
        result.push_back(c);
    }
    result.push_back(rhs);

    return result;
}

string_stream operator+(const string_stream &lhs, const wstring &rhs)
{
    string_stream result;
    for (char c : lhs) {
        result.push_back(c);
    }
    result.push_back(rhs);

    return result;
}
void string_stream::operator+=(char rhs)
{
    push_back(rhs);
}

void string_stream::operator+=(const char *rhs)
{
    push_back(rhs);
}

void string_stream::operator+=(const string &rhs)
{
    push_back(rhs);
}

void string_stream::operator+=(const wstring &rhs)
{
    push_back(rhs);
}

string string_stream::to_string()
{
    string result(_buf.begin(), _buf.size());

    return result;
}

wstring string_stream::to_wstring()
{
    string str(_buf.begin(), _buf.size());
    wstring result = convert_string_to_wstring(str);

    return result;
}

void string_stream::push_back(s64 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(s32 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(s16 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(s8 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(u64 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(u32 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(u16 n)
{
    push_back(tom::to_string(n));
}

void string_stream::push_back(u8 n)
{
    push_back(tom::to_string(n));
}

}  // namespace tom
