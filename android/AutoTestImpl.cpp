#include "AutoTestImpl.h"
#include "AutoTest.h"
#include "AutoTestHelper.h"

AutoTestImpl::AutoTestImpl()
{
}

AutoTestImpl::~AutoTestImpl()
{
}

void AutoTestImpl::FinishLoop()
{
	bool envAttached = false;
	JNIEnv *env = AutoTestGetJniEnv(envAttached);
	env->CallStaticVoidMethod(mAutoTestClass, midAutoTestGameFinish);
	AutoTestDetachVM(envAttached);
}

void AutoTestImpl::Screenshots(const char* name)
{
	bool envAttached = false;
	JNIEnv *env = AutoTestGetJniEnv(envAttached);
	jstring _name = env->NewStringUTF(name);
	env->CallStaticVoidMethod(mAutoTestClass, midAutoTestTakeScreenshot, _name);
	env->DeleteLocalRef(_name);
	AutoTestDetachVM(envAttached);
}

int AutoTestImpl::GetFileDescriptor()
{
	bool envAttached = false;
	JNIEnv *env = AutoTestGetJniEnv(envAttached);
	int fd = env->CallStaticIntMethod(mAutoTestClass, midAutoTestGameOutputFileDescriptor);
	AutoTestDetachVM(envAttached);

    return fd;
}

const char* AutoTestImpl::GetResultPath()
{
	bool envAttached = false;
	JNIEnv *env = AutoTestGetJniEnv(envAttached);
	jstring jPath = (jstring)env->CallStaticObjectMethod(mAutoTestClass, midAutoTestGameOutputFile);
	const char *path = env->GetStringUTFChars(jPath, NULL);
	env->ReleaseStringUTFChars( jPath, path);
	AutoTestDetachVM(envAttached);

	return path;
}

const char* AutoTestImpl::GetFacebookID()
{    
    return nullptr;
}