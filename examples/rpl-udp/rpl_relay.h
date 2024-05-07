#define RADIO_OFFSET -12 //reduced radio strengh. Testing possible on smaller distances.
#define RELAY_DBM -50 // level used for predicate switch to relay
#define RELAY_WARNING RELAY_DBM + 10 //an early warning of decaying signal. Used to print its time to send a drone
#define RELAY_RETURN_RSSI RELAY_DBM + 5 //While the client is connected through a relay it checks is the server RSSI is below this value.
