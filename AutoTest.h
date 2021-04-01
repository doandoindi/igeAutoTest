#pragma once
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#ifdef _WIN32
    #define IGE_EXPORT __declspec(dllexport)
#else
    #define IGE_EXPORT
#endif
class AutoTestImpl;
class IGE_EXPORT AutoTest
{
public:
	AutoTest();
	~AutoTest();
    void finishLoop();
    void writeResults(const char* name, const char* value);
    void writeResults(const char* name, int value);
    void screenshots();
    void collectModuleInfo();
    void adjustInfo();
    void gameAnalyticsInfo();
    void applovinInfo();
    void facebookInfo();
    bool isLoopTest();
	
	AutoTestImpl* GetImpl() { return m_autoTestImpl; }
    
    static AutoTest* Instance()
    {
        if (instance == nullptr)
        {
            instance = new AutoTest();
        }
        return instance;
    }
private:
    static AutoTest* instance;
    AutoTestImpl* m_autoTestImpl;
    json result_json;
};
