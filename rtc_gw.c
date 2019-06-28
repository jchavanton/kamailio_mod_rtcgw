/*
 * Copyright (C) 2018 Julien Chavanton jchavanton@gmail.com
 *
 * This file is part of Kamailio, a free SIP server.
 *
 * Kamailio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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

#include "rtc_gw.h"
#include "curl.h"

MODULE_VERSION

static int mod_init(void);
static void mod_destroy(void);
static int child_init(int);

str server_address = {0, 0};

static cmd_export_t cmds[] = {
	{"rtc_sdp_offer",(cmd_function)rtc_sdp_offer,0,0,0,ANY_ROUTE },
	{0, 0, 0, 0, 0, 0}
};

static pv_export_t mod_pvs[] = {
	{{0, 0}, 0, 0, 0, 0, 0, 0, 0}
};

static param_export_t mod_params[]={
	{"server_address", PARAM_STR, &server_address},
	{0,0,0}
};

struct module_exports exports = {
	"rtc_gw",        /* 1 module name */
	DEFAULT_DLFLAGS, /* 2 dlopen flags */
	cmds,            /* 3 exported functions */
	mod_params,      /* 4 exported parameters */
	0,               /* 5 exported RPC functions */
	mod_pvs,         /* 6 exported pseudo-variables */
	0,               /* 7 response function */
	mod_init,        /* 8 module initialization function */
	child_init,      /* 9 per-child init function */
	mod_destroy      /* 0 destroy function */
};

/**
 * @return 0 to continue to load the OpenSER, -1 to stop the loading
 * and abort OpenSER.
 */
static int mod_init(void) {
	LM_INFO("RTC GateWay module init\n");
	if (load_tm_api(&tmb)!=0) {
		LM_ERR( "can't load TM API\n");
		return -1;
	}
	return(0);
}

/**
 * Called only once when OpenSER is shuting down to clean up module
 * resources.
 */
static void mod_destroy() {
	LM_INFO("RTC GateWay module destroy\n");
	return;
}

void rtc_signal_handler(int signum) {
	LM_INFO("signal received [%d]\n", signum);
}

/**
 * The rank will be o for the main process calling this function,
 * or 1 through n for each listener process. The rank can have a negative
 * value if it is a special process calling the child init function.
 * Other then the listeners, the rank will equal one of these values:
 * PROC_MAIN      0  Main ser process
 * PROC_TIMER    -1  Timer attendant process 
 * PROC_FIFO     -2  FIFO attendant process
 * PROC_TCP_MAIN -4  TCP main process
 * PROC_UNIXSOCK -5  Unix domain socket server processes
 *
 * If this function returns a nonzero value the loading of OpenSER will
 * stop.
 */
static int child_init(int rank) {
	// signal(SIGINT,rtc_signal_handler);
	int rtn = 0;
	return(rtn);
}

int rtc_str_dup(str* dst, str* src, int shared) {
	if (!dst) {
		LM_ERR("dst null\n");
		return -1;
	}
	dst->len = 0;
	dst->s = NULL;
	if (!src) {
		LM_ERR("src null\n");
		return 0;
	}
	if ( (!src->s) || (src->len < 1)) {
		LM_ERR("empty src\n");
		return 0;
	}
	if (shared) {
		dst->s = shm_malloc(src->len +1);
	} else {
		dst->s = pkg_malloc(src->len +1);
	}
	if (!dst->s) {
		LM_ERR("%s_malloc: can't allocate memory (%d bytes)\n", shared?"shm":"pkg", src->len);
		return -1;
	}
	strncpy(dst->s, src->s, src->len);
	dst->s[src->len] = '\0';
	dst->len = src->len;
	return 1;
}

static int rtc_check_msg(struct sip_msg* msg) {
	if(!msg || !msg->callid || !msg->callid->body.s) {
		LM_INFO("no callid ?\n");
		return -1;
	}
	return 1;
}

rtc_session_info_t *rtc_session_new(struct sip_msg* msg) {
	if(!rtc_check_msg(msg))
		return NULL;
	rtc_session_info_t *si = shm_malloc(sizeof(rtc_session_info_t));
	if (!si) {
		LM_ERR("can not allocate session info !\n");
		return NULL;
	}
	memset(si,0,sizeof(rtc_session_info_t));

	if (!rtc_str_dup(&si->callid, &msg->callid->body,1)) {
		LM_ERR("can not get callid .\n");
		return NULL;
	}
	if (!rtc_str_dup(&si->from, &msg->from->body,1))
		return NULL;
	if (!rtc_str_dup(&si->to, &msg->to->body,1))
		return NULL;

	LM_INFO("...ok\n");
	return si;
}

int rtc_sdp_offer(struct sip_msg* msg, char* param1, char* param2) {
	int status = tmb.t_newtran(msg);
	LM_INFO("invite new transaction[%d]\n", status);
	if(status < 0) {
		LM_INFO("error creating transaction \n");
		return -1;
	} else if (status == 0) {
		LM_INFO("retransmission");
		return 0;
	}

	// send / receive
	rtc_session_info_t *si = rtc_session_new(msg);
	LM_INFO("[rtc_session_new] callid[%s] from[%s] to[%s] cseq[%d]\n", si->callid.s, si->from.s, si->to.s, si->cseq);
	rtc_sdp_info_t *sdp_info = &si->sdp_info_offer;
	if (!si)
		return -1;

	if(parse_sdp(msg) < 0) {
		LM_INFO("can not parse sdp\n");
		return -1;
	}
	sdp_info_t *sdp = (sdp_info_t*)msg->body;
	if(!sdp) {
		LM_INFO("sdp null\n");
		return -1;
	}
	sdp_info->recv_body.s = sdp->text.s;
	sdp_info->recv_body.len = sdp->text.len;

	LM_INFO("[rtc_session_new] len[%d]sdp[%s]", sdp_info->recv_body.len,
			sdp_info->recv_body.s);

	char buffer[100000];
	str recv_data = {0,0};
	sprintf(buffer, "{\"sdp\" : \"%s\",\n\r\"type\" : \"offer\"\n\r}", sdp_info->recv_body.s);

	char target_uri[256];
	LM_INFO("[%s][%d]\n", server_address.s, server_address.len);
	sprintf(target_uri, "http://%s:9999/offer", server_address.s);
	curl_send(target_uri, buffer, &recv_data);
	LM_INFO("curl_send [%s] done !\n", target_uri);

	// REPLY to the browser !
	str contact_hdr = str_init("Contact: <sip:rtp_gw@1.1.1.1>\r\nContent-Type: application/sdp\r\n");
	str to_tag = str_init("faketotag");
	str reason = str_init("OK");

	LM_INFO("reply body[%s]", recv_data.s);

	if(!tmb.t_reply_with_body(tmb.t_gett(),200,&reason,&recv_data,&contact_hdr,&to_tag)) {
		LM_INFO("t_reply error");
	}

	return 1;
}

