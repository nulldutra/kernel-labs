#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysproto.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>

// The syscall arguments
struct syscall_example_args {
	char *str;
};

// The syscall function
static int syscall_example(struct thread *td, void *syscall_args) {
	struct syscall_example_args *uap;
	uap = (struct syscall_example_args *)syscall_args;

	printf("syscall argument: %s\n", uap->str);
	return 0;
}

// The sysent for the new system call
static struct sysent syscall_example_sysent = {
	.sy_narg = 1,			// number of arguments
	.sy_call = syscall_example	// syscall function
};

static int offset = NO_SYSCALL;

// function called at load/unload
static int load(struct module *module, int cmd, void *arg) {
	unsigned int error = 0;

	switch(cmd) {
	case MOD_LOAD:
		uprintf("system call loaded at offset %d\n", offset);
		break;
	case MOD_UNLOAD:
		uprintf("system call unloaded from offset %d\n", offset);
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return error;
}

SYSCALL_MODULE(syscall_example, &offset, &syscall_example_sysent, load, NULL);

