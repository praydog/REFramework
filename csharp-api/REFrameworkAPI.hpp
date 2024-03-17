#pragma once

namespace reframework {
ref class API;
}

using namespace System;

namespace REFramework {
public ref class API
{
public:
    API(uintptr_t param);
    ~API();

    void Test();

    reframework::API^ Get();

protected:
    reframework::API^ m_api;
};
}