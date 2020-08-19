#include "core.h"
#include <limits.h>

char* skip_whitespace(const char *s)
{
	/* In POSIX/C locale (the only locale we care about: do we REALLY want
	 * to allow Unicode whitespace in, say, .conf files? nuts!)
	 * isspace is only these chars: "\t\n\v\f\r" and space.
	 * "\t\n\v\f\r" happen to have ASCII codes 9,10,11,12,13.
	 * Use that.
	 */
	while (*s == ' ' || (unsigned char)(*s - 9) <= (13 - 9))
		s++;

	return (char *) s;
}

char* skip_spec_c(const char *s, char c, int *found)
{
	while (*s == c || *s == ' ' || (unsigned char)(*s - 9) <= (13 - 9)){
		s++;
		if(found && (*s == c)){
			*found++;
		}
	}

	return (char *) s;

}

int str_c_count(const char *s, char c)
{
	int cnt = 0;

	for( ;*s != '\0';s++){
		if(*s == c){
			cnt++;
		}
	}

	return cnt;
}


