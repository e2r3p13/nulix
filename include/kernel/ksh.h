

#ifndef KSH_H
#define KSH_H

#define KSH_BUFSIZE 1024

struct builtin {
	char *name;
	void (*exec)(char **argv);
};

void ksh();

#endif
