#pragma onces


class AutoTestImpl {
public:
    AutoTestImpl();
    ~AutoTestImpl();
	
    void FinishLoop();
    void Screenshots(const char* name);
    const char* GetResultPath();
    void CollectModuleInfo();
};
