#include <linux/module.h>
#include <linux/security.h>

#include "hook_bind.h"
#include "hook_connect.h"
#include "communication.h"
#include "statistics.h"

#define DEFAULT_MAJOR 	63
#define DEVICE_NAME		"acheron"

MODULE_AUTHOR("Johannes Bauer <JohannesBauer@gmx.de>, Severin Strobl <severin@blue-neutrino.com>");
MODULE_DESCRIPTION("Socket Monitor Security Module");
MODULE_LICENSE("GPL");

static int Initialized = 0;
static int Firewall_Active = 0;
static int Firewall_Autoshutdown = 0;
static int Verbosity = 0;
static int Drop_UnknownProto = 1;
static int Drop_UnknownFamily = 0;
module_param(Firewall_Active, bool, 0640);
module_param(Firewall_Autoshutdown, bool, 0640);
module_param(Drop_UnknownProto, bool, 0640);
module_param(Drop_UnknownFamily, bool, 0640);
module_param(Verbosity, int, 0640);
MODULE_PARM_DESC(Verbosity, "0 = quiet, 1 = verbose, 2 = debug");

static struct security_operations acheron_Security_Ops = {
	.socket_connect = acheron_Connect_Hook,
	.socket_bind = acheron_Bind_Hook
};

static struct file_operations acheron_File_Ops = {
	.open = acheron_CharDev_Open,
	.release = acheron_CharDev_Close,
	.write = acheron_CharDev_Write,
	.read = acheron_CharDev_Read,
	.poll = acheron_CharDev_Poll
};

int acheron_Is_Active(void) {
	return Initialized && Firewall_Active;
}

int acheron_Has_Autoshutdown(void) {
	return Initialized && Firewall_Autoshutdown;
}

void acheron_Reset_Autoshutdown(void) {
	Firewall_Active = 0;
	Firewall_Autoshutdown = 0;
}

int acheron_Verbosity(void) {
	if (!Initialized) return 2;
	return Verbosity;
}

int acheron_Drop_UnknownFamily(void) {
	return Drop_UnknownFamily;
}

int acheron_Drop_UnknownProto(void) {
	return Drop_UnknownProto;
}

static int __init acheron_Security_Init(void) {
	int Result;
	if ((Result = register_security(&acheron_Security_Ops))) {
		printk(KERN_INFO "acheron: Failure registering socket monitor '%s' as primary security module: %d\n", KBUILD_MODNAME, Result);
		return Result;
	}
	printk(KERN_INFO "acheron: Capability LSM initialized as primary.\n");
	return 0;
}

static int __init acheron_Filesystem_Init(void) {
	int Major;
	Major = register_chrdev(DEFAULT_MAJOR, DEVICE_NAME, &acheron_File_Ops);
	if (Major < 0) {
		printk(KERN_INFO "acheron: Could not allocate acheron device (%d).\n", Major);
		return Major;
	} else if (Major != 0) {
		printk(KERN_INFO "acheron: Could not allocate acheron device with major %d, took major %d.\n", DEFAULT_MAJOR, Major);
	} else {
		printk(KERN_INFO "acheron: Character device successfully initialized.\n");
		Major = DEFAULT_MAJOR;
	}
	acheron_Statistic_Set(STATISTIC_INIT, 1);
	Initialized = 1;
	return 0;
}

security_initcall(acheron_Security_Init);
fs_initcall(acheron_Filesystem_Init);

