FROM debian

ENV DEB_PKG_CORES="libevent-dev libjansson-dev libpcre3 libpcre3-dev libunistring-dev"

ENV DEB_PKG="procps netcat curl dnsutils sqlite net-tools vim ngrep wget libhiredis-dev ${DEB_PKG_CORES}"
ENV DEB_PKG_TMP="bison flex build-essential git libssl-dev libpq-dev liblua5.1-0-dev libcurl4-openssl-dev libncurses5-dev libxml2-dev libsqlite3-dev"
ENV DEB_PKG_DEP="libcurl3 libssl1.1 libpq5 liblua5.1-0 libncurses5 libxml2 libsqlite3-0"

ENV KAM_INC_MOD="uac debugger usrloc registrar siputils tls xhttp websocket kex"
ENV KAM_SKIP_MOD="uid_domain diversion smsops mediaproxy rtpproxy mqueue topoh app_jsdt tsilo drouting kex acc stun usrloc p_usrloc dmq dmq_usrloc sipjson userblacklist path libtrie kex sipdump  uid_uri_db msrp sst imc mohqueue nattraversal pike xhttp rtpengine sca pdt matrix auth_diameter keepalive seas carrierroute ss7ops pipelimit db_cluster secfilter mangler acc_diameter enum app_sqlang pdb mtree sms"

ENV COMMIT_KAMAILIO=master

RUN echo "building Kamailio with RTC Gateway module" \
	&& apt-get update && apt-get -y install ${DEB_PKG} ${DEB_PKG_TMP} \
	&& mkdir -p /git && cd /git

RUN apt-get install -y automake autogen autoconf libtool pkg-config

RUN echo "building Kamailio" \
 	&& mkdir -p /git && cd /git \
 	&& git clone https://github.com/kamailio/kamailio.git \
 	&& cd kamailio && git checkout ${COMMIT_KAMAILIO} \
 	&& make include_modules="${KAM_INC_MOD}" skip_modules="\$(mod_list_extra) \$(mod_list_db) ${KAM_SKIP_MOD}" cfg \
 	&& make install

RUN echo "building kamailio_mod_rtcgw" \
 	&& cd /git/kamailio/src/modules \
 	&& git clone https://github.com/jchavanton/kamailio_mod_rtcgw.git \
 	&& cd kamailio_mod_rtcgw && git checkout master \
 	&& make install
