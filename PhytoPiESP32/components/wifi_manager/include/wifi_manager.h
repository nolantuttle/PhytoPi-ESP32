#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// Initialize Wi-Fi in station mode and begin connecting.
// Registers event handlers for connect/disconnect/IP-acquired events.
void wifi_init_sta(void);

// Block the calling task until an IP address is obtained.
void wifi_wait_connected(void);

#endif // WIFI_MANAGER_H
