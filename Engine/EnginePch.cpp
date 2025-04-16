#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

unique_ptr<Engine> GEngine = make_unique<Engine>();

std::string WStringToString(const std::wstring &wstr) {
    if (wstr.empty())
        return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
    return str;
}

wstring s2ws(const string &s) {
    int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, nullptr, 0);
    if (len == 0)
        return L"";

    vector<wchar_t> buf(len);
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, buf.data(), len);

    return wstring(buf.data());
}

string ws2s(const wstring &s) {
    int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len == 0)
        return "";

    vector<char> buf(len);
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, buf.data(), len, nullptr, nullptr);

    return string(buf.data());
}
