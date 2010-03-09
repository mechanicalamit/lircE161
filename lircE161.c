/*
 * lircE161.c
 *
 * Copyright (C) 2010, Amit Dhingra <mechanicalamit@gmail.com>
 *
 */

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lirc/lirc_client.h>

#define DEBUG 1

enum ret_codes {
		LIRCE161_ERROR,
		LIRCE161_OK,
		LIRCE161_DONE
};

/* Globals */
static struct lirc_config *lirc_config;
char *lirc_configfile = "/home/dhingraa/.lircE161rc";
char command_buf[BUFSIZ];
char arg_buf[BUFSIZ] = ""; /* needs initialization because it get appended */
char composite_buf[BUFSIZ] = "";

/* Functions */

/* gets the config from the button called 'command' in the rc file */
char *getcommand(const struct lirc_config *lconf)
{
		struct lirc_config_entry *i;
		i = lconf->first;
		while(i){
				if (strcmp(i->code->button, "command") == 0 )
						return i->config->string;
				i = i->next;
		}

		fputs("Command not found\n", stderr);
		exit(EXIT_FAILURE);

}

/* processes the received input. Appends the result to the global
 * variable arg_bug */
void processstate(char *inp)
{
		int cmdlen;
#ifdef DEBUG
		printf("processedstate: Got: %s\n", inp);
		printf("processedstate: Arguments are %s\n", arg_buf);
		if (strncmp(inp, "space", 5) == 0)
						printf("got space\n");
		else if (strncmp(inp, "done", 4) == 0)
						printf("got done\n");
		else
				printf("got other\n");
#endif
		if (strncmp(inp, "done", 4) == 0)
						return;
		if (strncmp(inp, "space", 5) == 0){
				cmdlen = strlen(arg_buf);
				strcpy(arg_buf + cmdlen, "\\ ");
		}
		else{
				cmdlen = strlen(arg_buf);
				strcpy(arg_buf + cmdlen, inp);
		}
}


int getnextlircinput(void)
{
		char *code;
		char *c;
		int ret;

		if (lirc_nextcode(&code) !=0)
				return LIRCE161_ERROR;
#ifdef DEBUG
		printf("Received lirc input %s\n", code);
#endif
		while((ret = lirc_code2char(lirc_config, code, &c)) == 0 && c != NULL){
#ifdef DEBUG
				printf("Received config %s\n", c);
#endif
				processstate(c);
				if (strncmp(c, "done", 4) == 0){
						free(code);
						return LIRCE161_DONE;
						}
		}

		free(code);
		return LIRCE161_OK;
}

int main(int argc, char **argv)
{
		int retcode;

		/** Init **/
		if (lirc_init("lircE161",1)==-1) exit(EXIT_FAILURE); /* Init lirc_client */
		if (lirc_readconfig( lirc_configfile, &lirc_config, NULL) !=0 ){
				lirc_deinit();
				return EXIT_FAILURE;
		}

		/* Get the command to run on command_buf */
		strcpy(command_buf, getcommand(lirc_config));

		/* build up the arguments in arf_buf at every key press */
		while(1){
				retcode = getnextlircinput();
				if (retcode == LIRCE161_ERROR)
						goto out;
				if (retcode == LIRCE161_DONE) /* done, let get out and run */
						break;
		}

		/* Put the command together */
		sprintf(composite_buf, (const char *)command_buf, arg_buf);
#ifdef DEBUG
		printf("Command is %s\n", command_buf);
		printf("Arguments are %s\n", arg_buf);
		printf("Composite Command is %s\n", composite_buf);
#endif
		system(composite_buf);

		/* finish up */
out:
		lirc_freeconfig(lirc_config);
		lirc_deinit();

		return EXIT_SUCCESS;
}

