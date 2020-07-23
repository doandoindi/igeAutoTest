#pragma onces

#include "firebase/messaging.h"
#include "firebase/util.h"
#include "firebase/future.h"

class FirebaseImpl {
public:
	FirebaseImpl();
	~FirebaseImpl();

	void Init(bool analytics = 1, bool messaging = 1, bool remoteConfig = 1, bool firestore = 1);
	void Release();
	bool ProcessEvents(int msec);
	void WaitForFutureCompletion(firebase::FutureBase future, firebase::FutureStatus toCheck = firebase::FutureStatus::kFutureStatusPending, int msec = 1000, double timeout = 5.0);
	static double GetTime();

	void AnalyticsInit();
	void AnalyticsRelease();

	void MessagingInit();
	void MessagingRelease();

	void RemoteConfigInit();
	void RemoteConfigRelease();
	void RemoteConfigFetchAll();

	void FirestoreInit() {}
	void FirestoreRelease() {}
	
	const char* MessagingGetRegistrationToken();
	bool IsReady() { return m_isReady; }
	bool IsAuthenticated() { return false; }
	std::string GetUserUID() { return ""; }

private:
	firebase::messaging::PollableListener* m_listener;
	firebase::ModuleInitializer* m_messageInitializer;

	firebase::ModuleInitializer* m_remoteConfigInitializer;

	bool m_isReady;

	bool m_isAnalytics;
	bool m_isMessaging;
	bool m_isRemoteConfig;
	bool m_isFirestore;
};
