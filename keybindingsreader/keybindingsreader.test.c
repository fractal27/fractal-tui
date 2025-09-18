#include "keybindingsreader.h"

int main(){
		struct KeybindingsReader kbreader;
		int ret;
		if((ret = keybindsreader_init(&kbreader,"test.conf"))) return ret;
		if((ret = keybindsreader_parse(&kbreader))) return ret;
		return keybindsreader_free(&kbreader);
}
