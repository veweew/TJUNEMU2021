#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}
static int cmd_si(char* args){
    int steps=0;
    int i;
    for(i=0;i<strlen(args);i++){
        steps=steps*10+(args[i]-48);
    }
    cpu_exec(steps);
    return 0;
}

static int cmd_info(char * args){
    if(*args=='r'){
        printf("Printing the value in the registers\n");
        printf("eax:0x%06x\n",cpu.eax);
        printf("ecx:0x%06x\n",cpu.ecx);
        printf("edx:0x%06x\n",cpu.edx);
        printf("ebx:0x%06x\n",cpu.ebx);
        printf("esp:0x%06x\n",cpu.esp);
        printf("ebp:0x%06x\n",cpu.ebp);
        printf("esi:0x%06x\n",cpu.esi);
        printf("edi:0x%06x\n",cpu.edi);
    }
    return 0;
}

static int cmd_scan(char *args){
    swaddr_t addrToScan;
    uint32_t length;
    uint32_t step;
    sscanf(args,"%d 0x%x",&length,&addrToScan);
    for(step=0;step<length;step++)
        printf("[%x]\n",swaddr_read(addrToScan+4*step, 4));
    return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Step continue the program", cmd_si},
    {"info", "Print some value", cmd_info},
    {"x", "scan the memory", cmd_scan}
    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
