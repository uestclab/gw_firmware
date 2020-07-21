#include "common.h"
#include "parse_helper.h"

static int get_line_with_continuation(parser_t *parser)
{
	ssize_t len, nlen;
	char *line;

	len = getline(&parser->line, &parser->line_alloc, parser->fp);
	if (len <= 0){
		return len;
    }

	line = parser->line;
	for (;;) {
		parser->lineno++;
		if (line[len - 1] == '\n')
			len--;
		if (len == 0 || line[len - 1] != '\\')
			break;
		len--;

		nlen = getline(&parser->nline, &parser->nline_alloc, parser->fp);
		if (nlen <= 0)
			break;

		if (parser->line_alloc < len + nlen + 1) {
			parser->line_alloc = len + nlen + 1;
			line = parser->line = xrealloc(line, parser->line_alloc);
		}
		memcpy(&line[len], parser->nline, nlen);
		len += nlen;
	}

	line[len] = '\0';
	return len;
}

int st_file_list_parse(char *str_list,char ***tk, int *tk_no)
{
	int ret;
	int i;
	int tk_cnt;
	char **ptr;

	if((NULL == str_list) || (NULL == tk_no) || (NULL == tk)){
		ret = -EINVAL;
		goto exit;
	}

	*tk_no = str_c_count(str_list, ',') + 1;

	ptr = (char**)xmalloc(sizeof(char**) * (*tk_no));
	if(NULL == ptr){
		ret = -ENOMEM;
		goto exit;
	}
	
	
	for(i=0; i<*tk_no; i++){
		ptr[i] = skip_spec_c(str_list, ',', NULL);
		if(',' == str_list[0]){
			str_list[0] = '\0';
		}
		str_list = strchrnul(str_list, ',');
	}

	*tk = ptr;
exit:
	return ret;
}

/* external interface */

int config_read(parser_t *parser, char **tokens, unsigned int ntokens, const char *delims)
{
	int ret = 0;
	char *line = NULL;
	int t;

	do{
		ret = get_line_with_continuation(parser);
		if(ret < 0){
			ret = 0;
			goto exit;
		}
		line = skip_whitespace(parser->line);
        printf("config_read->parser->line : %s \n", parser->line);
	}while(line[0] == '\0' || line[0] == delims[0] );

	if(line[0] == delims[2]){
		ret = 0;
		goto exit;
	}
	memset(tokens, 0, sizeof(tokens[0]) * ntokens);

	t = 0;
	do{
		tokens[t] = line;
			
		if (t != (ntokens-1)) {
			line += strcspn(line, delims + 1);
		} else {
			line = strchrnul(line, delims[0]);
		}

		if (*line == delims[0]){
			*line = '\0'; /* ends with comment char: this line is done */
		}
		else if (*line != '\0'){
			*line = '\0'; /* token is done, continue parsing line */
			line++;
			line = skip_whitespace(line);
		}

		t++;
	} while (*line && *line != delims[0] && t < ntokens);

	if(t < ntokens){
		ret = -EINVAL;
		//log_error("%d\n",t);
	}
exit:
	return ret;
}

int check_run_title(parser_t *parser, const char * title){
    char* line = NULL;
    line = strstr(parser->line, title);
    if(line){
        return 0;
    }else{
        return -1;
    }
}

int found_title(parser_t *parser, const char *title)
{
	int ret = -1;
	char *line1;

    do{
        ret = get_line_with_continuation(parser);
        if(ret < 0){
            ret = -1;
            goto exit;
        }
        printf("get_line_with_continuation: 0x%x = %s \n", parser->line,parser->line);
        line1 = strstr(parser->line, title);
    }while( (NULL == line1));
	
	if(line1){
		ret = 0;
    }
	
exit:
	return ret;
}

parser_t* config_open2(const char *filename, FILE* (*fopen_func)(const char *path))
{
	FILE* fp;
	parser_t *parser;

	fp = fopen_func(filename);
	if (!fp)
		return NULL;
	parser = xzalloc(sizeof(*parser));
	parser->fp = fp;
	return parser;
}



int read_config_file(const char *path, char **file_buf)
{
	int fd, ret;
	struct stat buf;

	if(NULL == path){
		ret = -EINVAL;
		goto exit;
	}
	
	ret = fd =  open(path,(O_RDONLY | O_SYNC), 0666);
	if(ret < 0){
		//log_error("open(%s)err",path);
		goto exit;
	}
	fstat(fd, &buf);
	
	*file_buf = (char*)xzalloc(buf.st_size + 1);
	if(NULL == *file_buf){
		ret = -ENOMEM;
		goto exit;
	}
	
	ret = read(fd,*file_buf,buf.st_size);
	close(fd);
exit:
	return ret;
}



int st_file_parse(const char *cf_path, struct stat_node_s *stat)
{
	int ret;
	char *file_buf = NULL;
	
	if(cf_path){
		ret = read_config_file(cf_path, &file_buf);
	}else{
		ret = -EINVAL;
		goto exit;
	}

	//ret = get_file_json_data(file_buf, stat);
	
exit:
	if(file_buf){
		xfree(file_buf);
	}
	return ret;
}