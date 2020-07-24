# igeFirebase

C++ extension Firebase for 3D and 2D games.

### Before running this tutorial, you have to install igeFirebase
	[pip install igeFirebase]

### Available libraries
	- Admob [ Android / iOS ]
	- Analytics [ Android / iOS ]
	- Authentication [ Android / iOS / Desktop ]
	- RemoteConfig [ Android / iOS / Desktop ]
	- MLKit [ Android ]

### Functions
#### Firebase
- First, you need to import and init the firebase system
	```
	import igeFirebase
	igeFirebase.init()
	```
- Release it when everything is done
	```
	igeFirebase.release()
	```
#### Firebase Admob
> https://firebase.google.com/docs/admob/cpp/quick-start
- _First, init the firebase admob system_
	- Following the structure : "adMobApp","bannerSize", "gender", "childDirectedTreatment","keywords","birthday", "testDevicesIds"
	- Tuple is supported
	```
	fb_admob = igeFirebase.admob()
	fb_admob.init(("ca-app-pub-1009273426450955~3020262852", "ca-app-pub-3940256099942544/6300978111", "ca-app-pub-3940256099942544/1033173712", "ca-app-pub-3940256099942544/2888167318"), (320, 50), 1, 1, ("game", "games", "gamess", "gamesss"), (12, 11, 1988), ("112F1C63CDDE8BAAEE287FDE3BA4C662",))
	```
- _Showing the ads_
	- **Banner**

	| Position | MoveTo Enum |
	| ------- | ------------ |
	| Top of the screen, horizontally centered. | kPositionTop = 0,
	| Bottom of the screen, horizontally centered. | kPositionBottom,
	| Top-left corner of the screen. | kPositionTopLeft,
	| Top-right corner of the screen. | kPositionTopRight,
	| Bottom-left corner of the screen. | kPositionBottomLeft,
	| Bottom-right corner of the screen. | kPositionBottomRight,

	```
	fb_admob.showBanner()
	fb_admob.bannerMoveTo(0)
	fb_admob.bannerMoveTo(1)
	fb_admob.bannerMoveTo(2)
	fb_admob.bannerMoveTo(3)
	fb_admob.bannerMoveTo(4)
	fb_admob.bannerMoveTo(5)
	fb_admob.hideBanner()
	```
	- **Interstitial**
	```
	fb_admob.showInterstitial()
	```
	- **RewardedVideo**
	```
	fb_admob.showRewardedVideo()
	fb_admob.pauseRewardedVideo()
	fb_admob.resumeRewardedVideo()
	```
- _Release it when everything is done_
	```
	fb_admob.release()
	```
#### Firebase Analytics
> https://firebase.google.com/docs/analytics/cpp/events
- _First, init the firebase admob system_
	```
	fb_analytics = igeFirebase.analytics()
	fb_analytics.init()
	```
- _Sending the events_
	```
	fb_analytics.setUserProperty("sign_up_method", "google")
	fb_analytics.setUserId("uber_user_510")
	fb_analytics.setCurrentScreen("Firebase Analytics C++ testapp", "testapp")

	fb_analytics.logEvent("login")
	fb_analytics.logEvent("progress", "percent", 0.4)
	fb_analytics.logEvent("post_score", "score", 42)
	fb_analytics.logEvent("join_group", "group_id", "spoon_welders")

	levelUpParameters = (("level", 5), ("character", "mrspoon"), ("hit_accuracy", 3.14))
	fb_analytics.logEvent("level_up", levelUpParameters)
	```
- _Release it when everything is done_
	```
	fb_analytics.release()
	```
#### Firebase Authentication
> https://firebase.google.com/docs/auth/cpp/start
- _First, init the firebase admob system_
	```
	fb_auth = igeFirebase.auth()
	fb_auth.init()
	```
- _Authenticate_
	```
	print('signInWithEmailAndPassword : ' + str(fb_auth.signInWithEmailAndPassword("doan.do@indigames.net", "doan.do")))
	print('isPlayerAuthenticated : ' + str(fb_auth.isPlayerAuthenticated()))

	print('signOut : ' + str(fb_auth.signOut()))
	print('isPlayerAuthenticated : ' + str(fb_auth.isPlayerAuthenticated()))

	print('registerWithEmailAndPassword : ' + str(fb_auth.registerWithEmailAndPassword("dodoan.it@gmail.com", "indigames")))
	print('isPlayerAuthenticated : ' + str(fb_auth.isPlayerAuthenticated()))
	```
- _Release it when everything is done_
	```
	fb_auth.release()
	```
#### Firebase MLKit
> https://firebase.google.com/docs/ml-kit
- _First, init the firebase mlkit system_
	```
	fb_mlkit = igeFirebase.mlKit()
	fb_mlkit.init(mode)	// 1 = FAST ; 2 = ACCURATE
	```
- _API_
	```
	fb_mlkit.getCameraSize()				// result : tuple(w, h)
	fb_mlkit.getCameraData()				// result : list(unsigned char)
	fb_mlkit.getContours					// result : list(float)
	fb_mlkit.getLeftEyeOpenProbability		// Returns a value between 0.0 and 1.0 giving a probability that the face's left eye is open
	fb_mlkit.getRightEyeOpenProbability		// Returns a value between 0.0 and 1.0 giving a probability that the face's right eye is open
	```
- _Release it when everything is done_
	```
	fb_mlkit.release()
	```

#### Firebase Firestore
- _callback_
	```
	def FirestoreGetCB(self, collection, field, value):
        print(self, 'get --- collection=' + collection + ' field=' + str(field) + ' value=' + str(value))
        
    def FirestoreSetCB(self, collection, field, value):
        print(self, 'set --- collection=' + collection + ' field=' + str(field) + ' value=' + str(value))
        
    def FirestoreDeleteCB(self, collection, field=None, value=None):
        print(self, 'del --- collection=' + collection + ' field=' + str(field) + ' value=' + str(value))
	```
- _get_
	> get data with Cloud Firestore
	>
	> firestore().get(collection, field, callback)
	>
	> > collection : string
	> >
	> > field : string or None
	> >
	> > callback : function(collection, field, value)
	> > > value : (string, int, double, dictionary)

	Example
	```
	igeFirebase.firestore().get("users", "str_s", self.FirestoreGetCB)
	igeFirebase.firestore().get("users", "int_s", self.FirestoreGetCB)
	igeFirebase.firestore().get("users", "double_s", self.FirestoreGetCB)
	igeFirebase.firestore().get("users", "bool_s", self.FirestoreGetCB)
	igeFirebase.firestore().get("users", "map_s", self.FirestoreGetCB)
	igeFirebase.firestore().get("users", None, self.FirestoreGetCB)
	```
- _set_
	> add data to Cloud Firestore
	
	> firestore().set(collection, field, value, callback, timestamp)
	> > collection : string
	> >
	> > field : string
	> >
	> > value : (string, int, double, dictionary)
	> >
	> > callback : optional function(collection, field, result)
    > >
	> > timestamp : bool(optinal)

	Example
	```
	igeFirebase.firestore().set("users", "str_s", "str_s", self.FirestoreSetCB)
	igeFirebase.firestore().set("users", "int_s", 1212, self.FirestoreSetCB)
	igeFirebase.firestore().set("users", "double_s", 12.1, self.FirestoreSetCB)
	igeFirebase.firestore().set("users", "bool_s", True, self.FirestoreSetCB)
	igeFirebase.firestore().set("users", "map_s", {"map_bool_s": True, "map_double_s": 12.1, "map_int_s": 1212, "map_str_s": "map_str_s"}, self.FirestoreSetCB)
	igeFirebase.firestore().set("users", "timestampValue", 10, self.FirestoreSetCB, timestamp=True)
	igeFirebase.firestore().set("users", "timestamp", None, self.FirestoreSetCB, timestamp=True)
	```
- _delete_
	> delete data from Cloud Firestore

	> firestore().delete(collection, field, callback)
	>
	> > collection : string
	> >
	> >field : string or None
	> >
	>> callback : optional function(collection, field, result)

	Example
	```
	igeFirebase.firestore().delete("users", "bool_s", self.FirestoreDeleteCB)
	igeFirebase.firestore().delete("users", None, self.FirestoreDeleteCB)
	```

### Notes
	- Firebase C++ SDK desktop support is a beta feature so only a subset of features supported for now.
		- Authentication
		- Cloud Functions
		- Cloud Storage
		- Realtime Database
		- Remote Config
		- Firestore [TODO]

### Reference
- https://firebase.google.com/docs/cpp/setup?platform=ios 
