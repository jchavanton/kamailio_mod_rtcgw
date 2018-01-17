/*
 * Copyright (C) 2018 Julien Chavanton jchavanton@gmail.com
 *
 * This file is part of Kamailio, a free SIP server.
 *
 * Kamailio is free software; you can redistribute it and/or modify
 * it under the tertc of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * Kamailio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef rtcgw_h
#define rtcgw_h

#include "../../core/data_lump.h"
#include "../../core/sr_module.h"
#include "../../core/parser/sdp/sdp_helpr_funcs.h"
#include "../../core/parser/parse_content.h"
#include "../../core/data_lump_rpl.h"
#include "../../core/clist.h"
#include "../../core/parser/contact/parse_contact.h"

#include "../tm/tm_load.h"
#include "../sdpops/api.h"

// #include "rtc_sdp.h"
// #include "rtc_media.h"

// documentation
// https://www.kamailio.org/dokuwiki/doku.php/development:write-module
// http://www.kamailio.org/docs/kamailio-devel-guide/#c16makefile



typedef struct rtc_sdp_info {
	char * remote_ip;
	char * local_ip;
	char * payloads;
	char * remote_port;
	int ipv6;
	str new_body;
	str recv_body;
	int udp_local_port;
} rtc_sdp_info_t;

/* protection against concurrent reply processing */
ser_lock_t session_list_mutex;

static int rtc_sdp_offer(struct sip_msg *, char *, char *);
// static int rtc_sdp_answer(struct sip_msg *, char *, char *);
// static int rtc_sessions_dump(struct sip_msg *, char *, char *);

struct tm_binds tmb;

typedef struct rtc_session_info {
	struct rtc_session_info* next;
	struct rtc_session_info* prev;
	rtc_sdp_info_t sdp_info_offer;
	rtc_sdp_info_t sdp_info_answer;
	str callid;
	str from;
	str to;
	str contact_uri;
	int cseq;
} rtc_session_info_t;

#endif
