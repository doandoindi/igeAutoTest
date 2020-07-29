#include <jni.h>
#include "AutoTestHelper.h"
#include "android/log.h"

JavaVM* autoTest_VM;
jclass mAutoTestClass;

/* method signatures */
jmethodID midAutoTestIsGameLoopTest;
jmethodID midAutoTestGameFinish;
jmethodID midAutoTestGameOutputFileDescriptor;
jmethodID midAutoTestGameOutputFile;
jmethodID midAutoTestTakeScreenshot;

void AutoTest_NativeInit(JavaVM* vm)
{
    autoTest_VM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    mAutoTestClass = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass(className)));

    midAutoTestIsGameLoopTest = env->GetStaticMethodID(mAutoTestClass, "IsGameLoopTest", "()Z");
    midAutoTestGameFinish = env->GetStaticMethodID(mAutoTestClass, "GameFinish", "()V");
    midAutoTestGameOutputFileDescriptor = env->GetStaticMethodID(mAutoTestClass, "GameOutputFileDescriptor", "()I");
    midAutoTestGameOutputFile = env->GetStaticMethodID(mAutoTestClass, "GameOutputFile", "()Ljava/lang/String;");
    midAutoTestTakeScreenshot = env->GetStaticMethodID(mAutoTestClass, "TakeScreenshot", "(Ljava/lang/String;)V");
}

JNIEnv* AutoTestGetJniEnv(bool &envAttached)
{
    JNIEnv* env;
    int getEnvStat = autoTest_VM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED)
    {
        envAttached = true;
        if (autoTest_VM->AttachCurrentThread(&env, NULL) != 0) {
            return nullptr;
        }
    }
    else if (getEnvStat == JNI_EVERSION) {
        return nullptr;
    }
    return env;
}

void AutoTestDetachVM(bool envAttached)
{
    if(envAttached)
    {
        autoTest_VM->DetachCurrentThread();
    }
}