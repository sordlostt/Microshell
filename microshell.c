#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <errno.h>
#define PATH_LIM 1000

char *HELP = "Microshell designed by Jan Nowakowski - 2019.\nList of commands:\ncd\necho\nexit";
char* input_line = NULL;
char* path;
char* path_org;
char* last_dir;

void ScanInput()
{
    input_line = calloc(1,sizeof *input_line);
    char ch;
    int i = 0;
    while(1)
    {
        scanf("%c",&ch);
        input_line = realloc(input_line,(i+1) * sizeof *input_line);
        if(ch == '\n')
        {
            input_line[i++] = '\0';
            break;
        }else
        input_line[i++] = ch;
    }
}

char** SeparateParams(char* input_line_org)
{
    char** new_params = malloc(sizeof(char*));
    char* item = strtok(input_line_org," ");
    new_params[0] = item;
    int i = 1;
    while (item != NULL)
    {
        item = strtok(NULL," ");
        if(item != NULL)
        {
            new_params = realloc(new_params,(i + 1) * sizeof(char*));
            new_params[i] = item;
            i++;
        }
    }
    new_params = realloc(new_params,(i + 1) * sizeof(char*));
    new_params[i] = NULL;
    return new_params;
}

void ReadInput()
{
    char* input_line_org = malloc(strlen(input_line) * sizeof(char));
    input_line_org = strcpy(input_line_org,input_line);
    char* input_command = strtok(input_line," ");
    if(input_command != NULL)
    {
        if(!strcmp("exit",input_command))
        {
            exit(0);
        }else
        if(!strcmp("help",input_command))
        {
            printf("%s\n",HELP);
        }else
        if(!strcmp("cd",input_command))
        {
            char* input_param = strtok(NULL," ");
            if(input_param != NULL)
            {
                if(!strcmp("~",input_param))
                {
                    last_dir = getcwd(NULL,0);
                    chdir(getenv("HOME"));
                }else
                if(!strcmp("-",input_param))
                {
                    char* last_dir_temp = getcwd(NULL,0);
                    chdir(last_dir);
                    last_dir = strcpy(last_dir,last_dir_temp);
                    free(last_dir_temp);
                }
                else
                {
                last_dir = getcwd(NULL,0);
                if((chdir(input_param)) != 0)
                    perror("Error");
                }
            }
        }else
        if(!strcmp("echo",input_command))
        {
            char* input_params = strtok(NULL,"");
            if(input_params != NULL)
            {
                printf("%s\n",input_params);
            }
        }else
        {
            pid_t id = fork();
            char** input_params = SeparateParams(input_line_org);
            if(id == 0)
            {
                execvp(input_command,input_params);
                perror("Error");
                exit(0);
            }else
            {
                wait(NULL);
            }
            
            free(input_params);
        }
    }
    free(input_line_org);
}

int main()
{
    last_dir = getcwd(NULL,0);
    while(1)
    {
        path = getcwd(NULL,0);
        printf("[%s:%s] \n$ ",getlogin(),path);
        ScanInput();
        ReadInput();
        free(input_line);
        free(path);
    }
}
