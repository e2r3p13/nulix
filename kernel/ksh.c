

#include <kernel/string.h>
#include <kernel/print.h>
#include <kernel/screenbuf.h>
#include <kernel/ksh.h>
#include <kernel/keyboard.h>
#include <kernel/builtins.h>

extern struct screenbuf *sb;
extern int sb_index;

char ksh_buf[KSH_BUFSIZE];
char *ksh_cmd[KSH_BUFSIZE / 2];
int ksh_bufindex;

struct builtin builtin[] = {
	{"reboot", reboot},
	{"poweroff", poweroff},
	{"key_us", key_us},
	{"key_fr", key_fr},
	{"color_red", color_red},
	{"color_reset", color_reset},
	{"clear", clear},
	{"help", help},
	{"int", int_divide},
};


static void ksh_prompt()
{
	kprintf("$> ");
}

static void ksh_newline()
{
	memset(ksh_buf, 0 , KSH_BUFSIZE);
	memset(ksh_cmd, 0 , KSH_BUFSIZE / 2);
	ksh_bufindex = 0;
	ksh_prompt();
}

void ksh_createcmd()
{
	char c;
	int ibuf = 0;
	int icmd = 0;
	while (ibuf < ksh_bufindex) {
		c = ksh_buf[ibuf];
		if (c != ' ' && c != 0) {
			ksh_cmd[icmd++] = ksh_buf + ibuf;
			while (ksh_buf[ibuf] != ' ' && ksh_buf[ibuf] != 0)
				ibuf++; 
		} else if (c == ' ') {
			ksh_buf[ibuf++] = 0;
			while (ksh_buf[ibuf] == ' ')
				ibuf++; 
		}
	}
}

void ksh_execcmd()
{
	int n = sizeof(builtin) / sizeof(struct builtin);
	for (int i = 0; i < n; i++) {
		if (strcmp(ksh_cmd[0], builtin[i].name) == 0) {
			builtin[i].exec((char **)ksh_cmd);
			break;
		}
	}
}

void ksh_getkeyevent(struct kbd_event *evt)
{
	while (1) {
		while (!KBD_poll());
		if (KBD_getevent(evt) == 0 && evt->type == KEY_PRESSED)
			return ;
	}
}

void ksh_shortcut(struct kbd_event *evt)
{
	switch (evt->key) {
		case KEY_CURSOR_UP:
			sb_scrollup(sb + sb_index, 1);
			break;
		case KEY_CURSOR_DOWN:
			sb_scrolldown(sb + sb_index, 1);
			break;
	}
}

void ksh_addchar(char c)
{
	if (c == '\n') {
		kprintf("\n");
		if (ksh_bufindex != 0) {
			ksh_createcmd();
			ksh_execcmd();
		}
		ksh_newline();
	} else if (c == '\b') {
		if (ksh_bufindex > 0) {
			ksh_buf[--ksh_bufindex] = 0;
			kprintf("\b");
		}
	} else {
		if (ksh_bufindex < KSH_BUFSIZE - 1) {
			ksh_buf[ksh_bufindex++] = c;
			kprintf("%c", c);
		}
	}
}

void ksh()
{
	struct kbd_event evt;
	char c;

	ksh_newline();
	while (1) {
		ksh_getkeyevent(&evt);
		if ((c = KBD_getchar(&evt)))
			ksh_addchar(c);
		else
			ksh_shortcut(&evt);
	}
}
