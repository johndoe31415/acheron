#include <linux/module.h>
#include <linux/security.h>

#include "display_debug.h"
#include "sharedinfo.h"
#include "notification_buffer.h"
#include "init.h"
#include "statistics.h"

int acheron_Connect_Hook(struct socket *sock, struct sockaddr *address, int addrlen) {
	id_t Unique_ID;
	if (!acheron_Is_Active()) return 0;

	acheron_Statistic_Increment(STATISTIC_CONNECT_CNT);
	Unique_ID = acheron_GetUniqueID();
	acheron_Display_Debug(Unique_ID, "CONNECT", sock, address, addrlen);
	{
		int Accepted = acheron_Hook_Wait(Unique_ID, CONNECT, sock, address, addrlen);
		if (Accepted) {
			acheron_Statistic_Increment(STATISTIC_CONNECT_ACCEPT_CNT);
			if (acheron_Verbosity() >= 2) printk(KERN_INFO "0x%x: ACCEPT Connect.\n", Unique_ID);
			return 0;
		}
	}

	acheron_Statistic_Increment(STATISTIC_CONNECT_REJECT_CNT);
	if (acheron_Verbosity() >= 2) printk(KERN_INFO "0x%x: REJECT Connect.\n", Unique_ID);
	/* Depending on the user-land decision, reject or accept the packet */
	return 1;	/* Reject */
}

