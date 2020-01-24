// Wifi credentials
#define WIFI_NETWORK  "your-wifi-name-here"
#define WIFI_KEY      "your-wifi-key-here"

// Where does the room data come from?
// `compact` parameter means we don't get the events JSON back, we don't need it
#define ROOM_JSON_URL  "http://rooms.local/room/leeds_mustard.json?compact=true"

// How far before things to warn people (in minutes)
#define MINUTES_BEFORE_BUSY_TO_WARN            10
#define MINUTES_BEFORE_END_OF_MEETING_TO_WARN  5

// How often should this indicator poll the server for an update (in milliseconds)
#define DATA_UPDATE_INTERVAL  1000 * 45
