#include "firebase/internal/platform.h"

#import <Foundation/Foundation.h>

#include "Firebase.h"
#include "FirebaseAnalytics.h"
#include "FirebaseRemoteConfig.h"

#include "firebase/app.h"
#include "firebase/future.h"
#include "firebase/remote_config.h"

#include <objc/objc.h>
#include <mach/mach_time.h>

static NSCondition* s_shutdown_signal;
static dispatch_semaphore_t s_semaphore;
static dispatch_once_t s_onceToken;

static mach_timebase_info_data_t timebase;

void FirebaseLogMessage(const char* format, ...)
{
  va_list args;
  NSString *formatString = @(format);

  va_start(args, format);
  NSString *message = [[NSString alloc] initWithFormat:formatString arguments:args];
  va_end(args);

  NSLog(@"%@", message);
}

FirebaseImpl::FirebaseImpl()
		: m_listener(nullptr)
		, m_messageInitializer(nullptr)
		, m_remoteConfigInitializer(nullptr)
		, m_isReady(false)
		, m_isAnalytics(false)
		, m_isMessaging(false)
		, m_isRemoteConfig(false)
		, m_isFirestore(false)
{
    mach_timebase_info(&timebase);
}

FirebaseImpl::~FirebaseImpl()
{
}

bool FirebaseImpl::ProcessEvents(int msec)
{
    [s_shutdown_signal waitUntilDate:[NSDate dateWithTimeIntervalSinceNow:static_cast<float>(msec) / 1000.0f]];
    return false;
}

void FirebaseImpl::Init(bool analytics, bool messaging, bool remoteConfig, bool firestore)
{
    s_shutdown_signal = [[NSCondition alloc] init];
    
    dispatch_once(&s_onceToken, ^{
        // Initialize with count=1 (this is executed only once):
        s_semaphore = dispatch_semaphore_create(1);
    });

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		m_isAnalytics = analytics;
		m_isMessaging = messaging;
		m_isRemoteConfig = remoteConfig;
		m_isFirestore = firestore;

		// app
		Firebase::firebase_app = ::firebase::App::GetInstance();
		if(Firebase::firebase_app == nullptr)
		{
			Firebase::firebase_app = ::firebase::App::Create();
		}

		// analytics
		if (m_isAnalytics)
		{
			AnalyticsInit();
		}

		// messaging
		if (m_isMessaging)
		{
			MessagingInit();
		}

		// remote config
		if (m_isRemoteConfig)
		{
			RemoteConfigInit();
		}

		// firestore
		if (m_isFirestore)
		{
			FirestoreInit();
		}

		m_isReady = true;
    });
}

void FirebaseImpl::Release()
{
    m_isReady = false;
    
	if (m_isAnalytics)
	{
		AnalyticsRelease();
	}

	if (m_isMessaging)
	{
		MessagingRelease();
	}

	if (m_isRemoteConfig)
	{
		RemoteConfigRelease();
	}

	if (m_isFirestore)
	{
		FirestoreRelease();
	}
}

void FirebaseImpl::AnalyticsInit()
{
    analytics::Initialize(*Firebase::firebase_app);
    analytics::SetAnalyticsCollectionEnabled(true);
    analytics::SetSessionTimeoutDuration(1000 * 60 * 30);
}

void FirebaseImpl::AnalyticsRelease()
{
	analytics::Terminate();
}

void FirebaseImpl::MessagingInit()
{
    LOG("Initialize the Messaging library");
	m_listener = new ::firebase::messaging::PollableListener();
    m_messageInitializer = new ::firebase::ModuleInitializer();

    m_messageInitializer->Initialize(
            Firebase::firebase_app, m_listener, [](::firebase::App* app, void* userdata) {
                firebase::messaging::PollableListener* listener = static_cast<::firebase::messaging::PollableListener*>(userdata);

                firebase::messaging::MessagingOptions options;
                options.suppress_notification_permission_prompt = true;

                return firebase::messaging::Initialize(*app, listener, options);
            });


    while (m_messageInitializer->InitializeLastResult().status() != firebase::kFutureStatusComplete) {
        if (ProcessEvents(100)) return;  // exit if requested
    }

    if (m_messageInitializer->InitializeLastResult().error() != 0) {
        LOG("Failed to initialize Firebase Messaging: %s", m_messageInitializer->InitializeLastResult().error_message());
        return;
    }

    // Display permission prompt if necessary
    firebase::Future<void> result = firebase::messaging::RequestPermission();
    WaitForFutureCompletion(result);
    if (result.error() == firebase::messaging::kErrorFailedToRegisterForRemoteNotifications) {
        LOG("Error registering for remote notifications.");
        return;
    }
	MessagingGetRegistrationToken();
}

void FirebaseImpl::MessagingRelease()
{
    delete m_listener;
    delete m_messageInitializer;
    firebase::messaging::Terminate();
}

const char* FirebaseImpl::MessagingGetRegistrationToken()
{
    static std::string token;
    if (m_listener && m_listener->PollRegistrationToken(&token)) {
        LOG("Received Registration Token: %s", token.c_str());
    }
    return token.c_str();
}

void FirebaseImpl::RemoteConfigInit()
{
    LOG("Initialize the RemoteConfig library");
    
    m_remoteConfigInitializer = new ::firebase::ModuleInitializer();

    m_remoteConfigInitializer->Initialize(Firebase::firebase_app, nullptr, [](::firebase::App* app, void*) {
        LOG("Try to initialize Remote Config");
        return firebase::remote_config::Initialize(*app);
    });

    while (m_remoteConfigInitializer->InitializeLastResult().status() != firebase::kFutureStatusComplete) {
        if (ProcessEvents(100)) return;  // exit if requested
    }

    if (m_remoteConfigInitializer->InitializeLastResult().error() != 0) {
        LOG("Failed to initialize Firebase Remote Config: %s", m_remoteConfigInitializer->InitializeLastResult().error_message());
        ProcessEvents(2000);
        return;
    }

    LOG("Initialized the Firebase Remote Config API");

    // NOTE: Developer mode should not be enabled in production applications.
    firebase::remote_config::SetConfigSetting(firebase::remote_config::kConfigSettingDeveloperMode,    "1");
    if ((*firebase::remote_config::GetConfigSetting(firebase::remote_config::kConfigSettingDeveloperMode).c_str()) != '1') {
        LOG("Failed to enable developer mode");
    }

    auto future_result = firebase::remote_config::Fetch(0);
    while (future_result.status() == firebase::kFutureStatusPending) {
        if (ProcessEvents(1000)) {
            break;
        }
    }

    if (future_result.status() == firebase::kFutureStatusComplete) {
        bool activate_result = firebase::remote_config::ActivateFetched();
        LOG("ActivateFetched %s", activate_result ? "succeeded" : "failed");
    }
}

void FirebaseImpl::RemoteConfigRelease()
{
    delete m_remoteConfigInitializer;
    firebase::remote_config::Terminate();
}
void FirebaseImpl::RemoteConfigFetchAll()
{
}

void FirebaseImpl::WaitForFutureCompletion(firebase::FutureBase future, firebase::FutureStatus toCheck, int msec, double timeout)
{
    double time = GetTime();
    while (ProcessEvents(msec))
    {
        double elapsedTime = GetTime() - time;
        if (future.status() != firebase::kFutureStatusPending || elapsedTime > timeout)
        {
            break;
        }
    }
}

double FirebaseImpl::GetTime()
{
    uint64_t t = mach_absolute_time();
    double tsec = (double)t * (double)timebase.numer / (double)timebase.denom / 1000000000.0;
    return tsec;
}
