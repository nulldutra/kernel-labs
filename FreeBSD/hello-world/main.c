#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int load(struct module *module, int cmd, void *arg) {
	unsigned int error = 0;

	switch(cmd) {
		case MOD_LOAD:
			uprintf("Hello, FreeBSD kernel\n");
			break;

		case MOD_UNLOAD:
			uprintf("By, FreeBSD :(\n");
			break;

		default:
			error = EOPNOTSUPP;
			break;
	}

	return error;
}

static moduledata_t hello_mod = {
	"hello",	// module name
	load,		// event handler function
	NULL		// extra data
};

DECLARE_MODULE(hello, hello_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

