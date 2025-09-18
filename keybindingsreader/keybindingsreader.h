#ifndef FRACTAL_CONFIGREADER_H
#define FRACTAL_CONFIGREADER_H

#include <stdio.h>

enum desc {
		None = -1,
		Initialization,
		Reading,
		Done
};




struct KeybindingsReader {
		FILE* pfile;
		struct kbreader_keybind {
				int key;
				void* value;
				void* when;
				char* helpmsg;
		}* dest;
		size_t ndest;
		enum desc state;
};


extern int keybindsreader_init(struct KeybindingsReader* conf, const char* filename);
extern int keybindsreader_parse(struct KeybindingsReader* conf, const char* values_chr[], void** values_assoc,
								const char* whens_accepted[], void** whens_assoc);
extern int keybindsreader_free(struct KeybindingsReader* conf);


#endif // FRACTAL_CONFIGREADER_H

