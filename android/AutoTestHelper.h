#include <jni.h>

static const char* className = "net/indigames/lib/AutoTestHelper";

extern jclass mAutoTestClass;
/* method signatures */
extern jmethodID midAutoTestIsGameLoopTest;
extern jmethodID midAutoTestGameFinish;
extern jmethodID midAutoTestGameOutputFileDescriptor;
extern jmethodID midAutoTestGameOutputFile;
extern jmethodID midAutoTestTakeScreenshot;

void AutoTest_NativeInit(JavaVM* vm);
JNIEnv* AutoTestGetJniEnv(bool &envAttached);
void AutoTestDetachVM(bool envAttached);