#include "AutoTest.h"
#include "AutoTestImpl.h"
#include <fstream>
#include "Adj.h"
#include "SDL.h"
#include "GAnalytics.h"
#include "AdsApplovin.h"
#include "pyxieTime.h"

extern SDL_bool SDL_IsGameLoopTest();

AutoTest* AutoTest::instance = nullptr;

AutoTest::AutoTest()
    : m_autoTestImpl(new AutoTestImpl())
{
}

AutoTest::~AutoTest()
{
}

void AutoTest::collectModuleInfo()
{
    adjustInfo();
    gameAnalyticsInfo();
    applovinInfo();
}

void AutoTest::adjustInfo()
{
    Adj* adjust = Adj::Instance();
    
    json jAdjust = adjust->dumpInfo();
    result_json["Adjust"] = jAdjust;
}

void AutoTest::gameAnalyticsInfo()
{
    GAnalytics* ga = GAnalytics::Instance();
    json jGA = ga->dumpInfo();;
    
    result_json["GameAnalytics"] = jGA;
}

void AutoTest::applovinInfo()
{
    AdsApplovin* applovin = AdsApplovin::Instance();
    json jApplovin = applovin->dumpInfo();;

    result_json["Applovin"] = jApplovin;
}

void AutoTest::finishLoop()
{
    if(GetImpl() != nullptr)
    {
        collectModuleInfo();
        
        writeResults("Result", "Success");
        
        const char* path = GetImpl()->GetResultPath();
        std::ofstream file(path);
        file << result_json;
        
        printf("%s", result_json.dump(4).c_str());
        GetImpl()->FinishLoop();
    }
}

void AutoTest::writeResults(const char* name, const char* value)
{
    result_json.emplace(name, value);
}

void AutoTest::writeResults(const char* name, int value)
{
    result_json.emplace(name, value);
}

void AutoTest::screenshots()
{
    if(GetImpl() != nullptr)
    {        
        GetImpl()->Screenshots(std::to_string(pyxie::pyxieTime::Instance().GetCPUTime()).c_str());
    }
}

bool AutoTest::isLoopTest()
{
    return (SDL_IsGameLoopTest() == SDL_TRUE);
}
