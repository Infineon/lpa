# MbedOS helper 

Helper API's

* int32_t wait_net_suspend(void *net_intf,  uint32_t wait_ms,
							uint32_t inactive_interval_ms, uint32_t inactive_window_ms);
		
	Call wait_net_suspend to suspend network stack and wait till timeout or wakeup activity.
	wait_net_suspend() suspends the network stack, when network stack is resumed, the
	network timers are not adjusted to reflect the time network stack was suspended,
	it is users responsibility to adjust network timers on wakeup.

* int cy_tcp_create_socket_connection(void *net_intf, void **socket_handle, const char *remote_ip, uint16_t remote_port, uint16_t local_port,
    cy_tko_ol_cfg_t *downloaded, int socket_keepalive_enable);
	
	This function can be used as reference to create tcp socket connection to provided remote ip address, remote port, local port
	and the tcp socket handle is returned in *socket_handle.




