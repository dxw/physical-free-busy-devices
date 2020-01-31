// Wifi credentials
#define WIFI_NETWORK  "your-wifi-name-here"
#define WIFI_KEY      "your-wifi-key-here"

// Where does the room data come from?
#define ROOMS_DASHBOARD_HOST              "rooms.local"
#define ROOMS_DASHBOARD_SHA1_FINGERPRINT  "10:CC:55:95:BC:8B:30:17:8D:13:75:4A:F2:F4:97:F3:12:30:6E:F2"  // The ESP2866 can't do full-blown certificate validation, so we tell it which certificate to trust
#define ROOM_SLUG                         "leeds_mustard"

// How far before things to warn people (in minutes)
#define MINUTES_BEFORE_BUSY_TO_WARN            10
#define MINUTES_BEFORE_END_OF_MEETING_TO_WARN  5

// How often should this indicator poll the server for an update (in milliseconds)
#define DATA_UPDATE_INTERVAL  1000 * 45
