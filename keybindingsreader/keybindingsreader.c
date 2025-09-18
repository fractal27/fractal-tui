#ifndef FRACTAL_KEYBINDINGSREADER_C
#define FRACTAL_KEYBINDINGSREADER_C
#include "../fractal.h"
#include "keybindingsreader.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>

#define MAXSIZE 1024

void
trim(char* s)
{
		size_t start = 0;
		size_t end_p = strlen(s), end;

		while(s[start++] == ' ');
		memmove(s, s + start-1, strlen(s + start) + 1);

		for(end = strlen(s); end > 0 && s[end-1] == ' '; end--);
		s[end] = '\0';
}


int
keybindsreader_init(struct KeybindingsReader* pkbreader, const char* filename){
		/* returns 1 if error else 0 */
		assert(pkbreader != NULL && "keybindingsreader: struct KeybindingsReader* kbreader cannot be null.");

		if((pkbreader->pfile = fopen(filename, "r")) == NULL) {
				fprintf(stderr, "file `%s` cannot be opened.\n", filename);
				return errno;
		}

		pkbreader->dest = malloc(MAXSIZE*sizeof(struct kbreader_keybind));
		pkbreader->state = Initialization;
		pkbreader->ndest = 0;
		return 0;
}


int
keybindsreader_parse(struct KeybindingsReader* conf, const char* values_accepted[], void** values_assoc,
					const char* whens_accepted[], void** whens_assoc)
{
		char key[128], val[128], helpmsg[128];
		char* v;
		char line[256+1];
		size_t line_n = 0;
		size_t i = 0;

		while (fgets(line,sizeof(line),conf->pfile)){
				line_n++;

				if(line[0] == '\n' || line[0] == '#' || line[0] == '\0')
						continue;

				if(sscanf(line, "%127[^|]|%127[^|]|%127[^\n]", key, val, helpmsg) == 3) {
						assert(!strchr(key,'#') && "key unfinished(part of key commented with '#')");
						assert(!strchr(val,'#') && "value unfinished(part of value commented with '#')");

						if((v=strchr(helpmsg,'#'))){
								*v='\0'; // comment 
						}
						trim(key);
						trim(val);
						trim(helpmsg);

						conf->dest[conf->ndest].key = !strcmp("KEY_MOUSE",key)?KEY_MOUSE:*key;
						conf->dest[conf->ndest].helpmsg = strdup(helpmsg);
						
						for(i = 0; i < sizeof(whens_accepted)/sizeof(const char*);i++){
								if(whens_assoc[i] && !strcmp(val,whens_assoc[i])){
										conf->dest[conf->ndest].when = whens_assoc[i];
										break;
								} else if(!whens_assoc[i])
										conf->dest[conf->ndest].when = whens_assoc[i];
						}

						for(i = 0; i < sizeof(values_accepted)/sizeof(const char*);i++){
								if(!strcmp(val,values_accepted[i])){
										conf->dest[conf->ndest].value = values_assoc[i];
										break;
								}
						}

						conf->ndest++;
				} else {
						printf("malformed key pair at %zu\n",line_n);
				}
		}
		return 0;
}

int 
keybindsreader_free(struct KeybindingsReader* conf)
{
		fclose(conf->pfile);
		free(conf->dest);
		return 0;
}




#endif // FRACTAL_CONFIGREADER_C


