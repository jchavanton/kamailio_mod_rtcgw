

This is a test module

This module can be use to extract the SDP from an INVITE comming from SIP over websocket and send it to an WebRTC endpoint
using HTTP, the HTTP 200 OK will contain the SDP answer generated by WebRTC and the modul will automaticaly reply with a 200 OK.


```
#!ifdef WITH_RTC_GW
loadmodule "rtc_gw.so"
modparam("rtc_gw", "server_address", "127.0.1.102");
#!endif


#!ifdef WITH_RTC_GW
if(is_method("INVITE") && !has_totag()) {
		rtc_sdp_offer();
		exit;
}
#!endif
```


To use this module you also need websocket support in Kamailio

```
#!ifdef WITH_WSS
enable_tls=yes
tcp_accept_no_cl=yes
loadmodule "nathelper.so"
loadmodule "tls.so"
loadmodule "xhttp.so"
loadmodule "msrp.so"  # Only required if using MSRP over WebSockets
loadmodule "websocket.so"

modparam("tls", "private_key", "/etc/certs/sip.mydomain.com/key.pem")
modparam("tls", "certificate", "/etc/certs/sip.mydomain.com/cert.pem")
modparam("tls", "ca_list", "/etc/certs/demoCA/cert.pem")
# modparam("tls", "tls_method", "SSLv23")

listen=tls:127.0.1.101:443
#!endif
```
