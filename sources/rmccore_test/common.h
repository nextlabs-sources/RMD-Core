#pragma once

template <typename T>
class ExeDir
{
public:
    ExeDir()
    {
        T szPath[MAX_PATH] = {0};
        T* pos;
        if(1 == sizeof(T)) {
            GetModuleFileNameA(NULL, (char*)szPath, MAX_PATH);
            pos = (T*) strrchr((char*)szPath, '\\');
        } else {
            GetModuleFileNameW(NULL, (wchar_t*)szPath, MAX_PATH);
            pos = (T*) wcsrchr((wchar_t*)szPath, L'\\');
        }
        *pos = 0;
        s = szPath;
    }
    ~ExeDir(){}

    const std::basic_string<T>& getExeDir() const {return s;}
private:
    std::basic_string<T> s;
};
