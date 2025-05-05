#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <net/pfil.h>

#define TRIGGER "hooked"

static pfil_hook_t icmp_hook = NULL;

static pfil_return_t
icmp_packet_hook(struct mbuf **mp, struct ifnet *ifp, int dir, void *ctx, struct inpcb *inpi) {
    	struct mbuf *m = *mp;
    	struct ip *ip;
    	struct icmp *icmpdata;
    	int iphlen;

    	if (m == NULL)
        	return 0;

    	if (m->m_len < sizeof(struct ip))
        	return 0;

    	ip = mtod(m, struct ip *);

    	// check if the protocol is ICMP 
    	if (ip->ip_p != IPPROTO_ICMP)
        	return 0;

    	iphlen = ip->ip_hl << 2;

    	if (m->m_len < iphlen + sizeof(struct icmp))
        	return 0;

    	icmpdata = (struct icmp *)((caddr_t)ip + iphlen);

    	char buffer[7];  // 6 + '\0'
    	strncpy(buffer, (char *)icmpdata->icmp_data, 6);
    	buffer[6] = '\0';

    	if (buffer[0] != '\0') {
        	printf("ICMP payload: %s\n", buffer);
    	}

    	if (strncmp(icmpdata->icmp_data, TRIGGER, 6) == 0) {
        	printf("ICMP trigger!\n");
    	}

    	return 0;
}

static int
load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
    case MOD_LOAD:
    {
	struct pfil_hook_args pha = {
    		.pa_version = PFIL_VERSION,
    		.pa_flags = PFIL_IN | PFIL_OUT,
    		.pa_ruleset = NULL,
    		.pa_modname = "icmp_hook",
		.pa_mbuf_chk = icmp_packet_hook
	};

        icmp_hook = pfil_add_hook(&pha);
        if (icmp_hook == NULL) {
            printf("Error on load hook.\n");
            return 1;
        }

        printf("Loaded hook.\n");
        break;
    }

    case MOD_UNLOAD:
        if (icmp_hook != NULL) {
            pfil_remove_hook(icmp_hook);
            printf("unload icmp hook.\n");
            icmp_hook = NULL;
        }
        break;

    default:
        error = EOPNOTSUPP;
        break;
    }

    return error;
}

static moduledata_t icmp_pfil_mod = {
    "icmp_pfil_hook",
    load,
    NULL
};

DECLARE_MODULE(icmp_pfil_hook, icmp_pfil_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

