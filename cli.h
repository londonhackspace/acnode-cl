#ifndef _CLI_H_
#define _CLI_H_

void mrlprint(const char * str);
void print_help();
bool ishex(char c);
uint8_t fromhex(char c);
int mrlexecute (int argc, const char * const * argv);
char ** mrlcomplete(int argc, const char * const * argv);

#endif

