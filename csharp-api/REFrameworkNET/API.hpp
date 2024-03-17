#pragma once
#pragma managed

namespace reframework {
ref class API;
}

using namespace System;

namespace REFrameworkNET {
public ref class API
{
public:
    API(uintptr_t param);
    ~API();

    reframework::API^ Get();

protected:
    reframework::API^ m_api;
};
}