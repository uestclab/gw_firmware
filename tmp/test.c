#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <wait.h>

int process(char * command);
int main(int argc, char ** argv) {
	int c, status = 0;
	int runLevel = 3;
	while ((c = getopt(argc, argv, "r:")) != EOF) {
		switch (c) {
		case 'r':
			runLevel = atoi(optarg);
			break;
		case '?':
		default:
			status = 1;
			break;

		}
	}

	// check if we have invalid parameters or multiple filenames
	if (optind + 1 != argc || status) {
		fprintf(stderr, "usage: %s [-r runlevel] file\n", argv[0]);
		return (1);
	}
	// open the configuration file
	char * filename = argv[optind];
	FILE *fp = fopen(filename, "r");
	if (fp != NULL) {
		char line [500];
		int numLines = 0;
		int respawnNum = 0;
		char * respawnProcs[500];
		char * p;
		while (fgets(line, sizeof(line), fp)) {
			// check if more than 100 entries
			if(numLines > 99){
				fprintf(stderr, "usage: %s [-r runlevel] file\n", argv[0]);
				return(1);
			}

			// remove # and newline and discard blank lines
			if ((p = strchr(line, '#'))) {
				* p = '\0';
			}
			if ((p = strchr(line, '\n'))) {
				* p = '\0';
			}
			if (line[0] == '\0')
				continue;

			// get next colon
			if ((p = strchr(line, ':'))) {
				char * q = line;
				int runLevelFound = 0;
				if (q[0] == ':') {
					runLevelFound = 1;
				}
				// determine if process is in run level
				while (*q != *p) {
					if ((q[0] - '0') == runLevel) {
						runLevelFound = 1;
					}
					q++;
				}
				// if the run level was not found, discard the line
				if (runLevelFound == 0) {
					continue;
				}
				// now get the next colon
				q++;
				p++;
				// determine respawn behaviour (once or respawn)
				int behaviour;
				if (strlen(q) > 4 && strstr(q, "once:")) {
					behaviour = 0;
				}
				else if (strlen(q) > 7 && strstr(q, "respawn:")) {
					behaviour = 1;
				} else {
					// invalid format of file, so print fatal error
					fprintf(stderr, "usage: %s [-r runlevel] file\n", argv[0]);
					return (1);
				}
				// now get the command that we are going to execute
				if ((p = strchr(p, ':'))) {
					p++;
					/// p is the command, execute it
					process(p);

					// if we have a respawning process store it in respawn array
					if (behaviour == 1) {
						respawnProcs[respawnNum] = strdup(p);
						respawnNum++;
					}

				}
			}
			numLines++;
		}

		// now take care of the respawning processes
		while (respawnNum > 0) {
			for (int i = 0; i < respawnNum; i++) {
				process(respawnProcs[i]);
			}
		}

		return (0);

	} else {
		perror(filename);
		return (1);
	}


}

int process(char * command) {
	// create the process
	pid_t x = fork();
	if (x < 0) {
		perror("fork() error");
		return (1);
	}
	if (x != 0) {
		// parent process
		// if respawn process than call this method again
		wait(NULL);
		return (0);
	} else {
		// child process, execute command
		execl("/bin/sh", "sh", "-c", command, (char*)NULL);
		perror(command);
		return (1);
	}
	return (0);
}
