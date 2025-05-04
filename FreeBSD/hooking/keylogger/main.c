#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>

static int sys_read_hook(struct thread *td, void *syscall_args);
static int load(struct module *module, int cmd, void *arg);

static int sys_read_hook(struct thread *td, void *syscall_args) {
	struct read_args *uap;
	uap = (struct read_args *)syscall_args;
	int error;
	size_t done;
	char buf[1];

	error = sys_read(td, syscall_args);
	if (error || (!uap->nbyte) || (uap->nbyte > 1) || (uap->fd != 0))
		return error;
	
	error = copyinstr(uap->buf, buf, 1, &done);
	if (error != 0) {
		return error;
	}
	printf("%c\n", buf[0]);

	return 0;
}

static int load(struct module *module, int cmd, void *arg) {
	int error = 0;

	switch(cmd) {
	case MOD_LOAD:
		sysent[SYS_read].sy_call = (sy_call_t *)sys_read_hook;
		break;
	case MOD_UNLOAD:
		sysent[SYS_read].sy_call = (sy_call_t *)sys_read;
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

static moduledata_t read = {
	"sys_read_hook",
	load,
	NULL
};

DECLARE_MODULE(sys_read_hook, read, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

