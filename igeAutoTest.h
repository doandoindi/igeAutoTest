#include <Python.h>
#include "AutoTest.h"

typedef struct {
	PyObject_HEAD
		AutoTest* autoTest;
} autoTest_obj;

extern PyTypeObject AutoTestType;
