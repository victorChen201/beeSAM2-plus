#ifndef	__M_FILE_NAME_sonos_drv_H__
#define __M_FILE_NAME_sonos_drv_H__

#include "common.h"
#include "ssdp.h"
#include "net_manage.h"
#include "data_common.h"
#define MUI_SONOS_STATE_VOLUME_MAX		99

char MUI_SONOS_MUSIC_URL[1024];

extern int sonos_drv_m_search_broadcasting (const S_SocketCfg *psSocketCfg);
extern int sonos_drv_answer_deal (const S_SocketCfg *psSocketCfg, const char *pchMsg, size_t tLen);
extern int sonos_drv_notify_deal (const S_SocketCfg *psSocketCfg, const char *pchMsg, size_t tLen);
extern int sonos_drv_join (uint16_t ui16Duration);

extern int sonos_drv_init (void);
extern int sonos_drv_dev_init (const S_DevZbAttr* psDevNetAttr);
extern int sonos_drv_play (const S_DevZbAttr* psDevNetAttr, uint8_t ui8Play);
extern int sonos_drv_set_url (const S_DevZbAttr* psDevNetAttr, const char* pchUrl, size_t tLen);
extern int sonos_drv_set_volume (const S_DevZbAttr* psDevNetAttr, uint16_t ui16Volume);

extern int sonos_net_manage_init ();
#endif
