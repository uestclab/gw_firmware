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