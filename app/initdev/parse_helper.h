#ifndef _PARSE_HELPER_H_INCLUDED_
#define _PARSE_HELPER_H_INCLUDED_

#include "core.h"

typedef struct parser_t {
	FILE *fp;
	char *data;
	char *line, *nline;
	size_t line_alloc, nline_alloc;
	int lineno;
} parser_t;

static int bb_ascii_isalnum(unsigned char a)
{
	unsigned char b = a - '0';
	if (b <= 9)
		return (b <= 9);
	b = (a|0x20) - 'a';
	return b <= 'z' - 'a';
}


static unsigned long long ret_ERANGE(void)
{
	errno = ERANGE; /* this ain't as small as it looks (on glibc) */
	return ULLONG_MAX;
}

static unsigned long long handle_errors(unsigned long long v, char **endp)
{
	char next_ch = **endp;

	/* errno is already set to ERANGE by strtoXXX if value overflowed */
	if (next_ch) {
		/* "1234abcg" or out-of-range? */
		if (bb_ascii_isalnum(next_ch) || errno)
			return ret_ERANGE();
		/* good number, just suspicious terminator */
		errno = EINVAL;
	}
	return v;
}


static int bb_strtoull(const char *arg, char **endp, int base){
	unsigned long long v;
	char *endptr;
	int index = 0;
	if (!endp) endp = &endptr;
	*endp = (char*) arg;
	/* strtoul("  -4200000000") returns 94967296, errno 0 (!) */

	/* I don't think that this is right. Preventing this... */

	if (!bb_ascii_isalnum(arg[0])){
		if('-' == arg[0]){
			index = 1;
		}else{
			return ret_ERANGE();
		}
	}

	/* not 100% correct for lib func, but convenient for the caller */
	errno = 0;
	v = strtoull(&arg[index], endp, base);
	v = handle_errors(v, endp);
	return index?-v:v;
}

static inline int input_str_is_empty(char *str)
{
	return !strlen(str);
}

static FILE*  fopen_for_read(const char *path)
{
	return fopen(path, "r");
}

parser_t* config_open2(const char *filename, FILE* (*fopen_func)(const char *path));

int check_run_title(parser_t *parser, const char * title);
int found_title(parser_t *parser, const char *title1);
int config_read(parser_t *parser, char **tokens, unsigned int ntokens, const char *delims);

int st_file_list_parse(char *str_list,char ***tk, int *tk_no);
int st_file_parse(const char *cf_path, struct stat_node_s *stat);




#endif /* _PARSE_HELPER_H_INCLUDED_ */