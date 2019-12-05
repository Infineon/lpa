# MbedOS network suspend helper 

Copy network_activity_handler.cpp and network_activity_handler.h in the
same folder as application main.cpp, and invoke wait_net_suspend to suspend network
stack and wait till timeout or wakeup activity.

Helper API's
 int wait_net_suspend(WhdSTAInterface *wifi, uint32_t wait_ms,
        uint32_t inactive_interval_ms, uint32_t inactive_window_ms);
 Used to suspend network stack and wait till wait_ms or external wakeup event happens

##NOTE
wait_net_suspend() suspends the network stack, when network stack is resumed, the
network timers are not adjusted to reflect the time network stack was suspended,
it is users responsibility to adjust network timers on wakeup.


