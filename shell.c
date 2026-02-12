#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define _POSIX_C_SOURCE 200809L

#define COLOR_GREEN  "\001\033[1;32m\002"
#define COLOR_BLUE   "\001\033[1;34m\002"
#define COLOR_RESET  "\001\033[0m\002"
#define COLOR_ORANGE "\001\033[38;5;208m\002"

char shell_prompt[256] = "$ ";
const char *builtin[]= {"exit" , "type" ,"echo", "cd", "pwd" , "prompt", NULL};


int parse_input(char *input, char **args){
       int arg_count =0;
       char *token = malloc(strlen(input)+1);
       int token_len = 0;
       
       enum{NORMAL, SINGLE, DOUBLE, ESCAPE} state = NORMAL, prev_state;
       
       for (const char *p = input; *p; p++){
         char c =*p;
            switch(state){
               case NORMAL:
                     if (c == ' ' || c == '\t'){
                        if (token_len > 0){
                          token[token_len] = '\0';
                          args[arg_count++] = strdup(token);
                          token_len = 0;                        
                        }
                     }
                     else if (c == '\''){
                     state = SINGLE;}
                     else if (c == '"'){
                     state = DOUBLE;}
                     else if (c == '\\'){
                     prev_state = state;
                     state = ESCAPE;}
                     else {
                     token[token_len++] = c;}
                     break;
            case SINGLE:
                if (c == '\'') {
                    state = NORMAL;
                    continue;
                } else {
                    token[token_len++] = c; 
                }
                break;

            case DOUBLE:
                if (c == '"') {
                    state = NORMAL;
                    continue;
                } else if (c == '\\') {
                     prev_state = state;
                    state = ESCAPE;
                } else {
                    token[token_len++] = c;
                }
                break;

            case ESCAPE:
                token[token_len++] = c;
                // return to previous state
                state = prev_state;
                break;
        }
    }

    
    if (token_len > 0) {
        token[token_len] = '\0';
        args[arg_count++] = strdup(token);
    }

    args[arg_count] = NULL;
    free(token);
    return arg_count;
}
                     
                    
            
            
            
            
            
            
            
       
       
       
       
       
	
void execute_command(char **args){

  char *input_file = NULL;
    char *output_file = NULL;
    char *error_file = NULL;

    int append_out = 0;
    int append_err = 0;

    char *clean_args[100];
    int j = 0;

    
    for (int i = 0; args[i] != NULL; i++) {

        
        if (strcmp(args[i], "<") == 0 || strcmp(args[i], "0<") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "syntax error\n");
                return;
            }
            input_file = args[i+1];
            i++;
        }

       
        else if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "syntax error\n");
                return;
            }
            output_file = args[i+1];
            append_out = 0;
            i++;
        }

        
        else if (strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "syntax error\n");
                return;
            }
            output_file = args[i+1];
            append_out = 1;
            i++;
        }

       
        else if (strcmp(args[i], "2>") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "syntax error\n");
                return;
            }
            error_file = args[i+1];
            append_err = 0;
            i++;
        }

       
        else if (strcmp(args[i], "2>>") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "syntax error\n");
                return;
            }
            error_file = args[i+1];
            append_err = 1;
            i++;
        }

        else {
            clean_args[j++] = args[i];
        }
    }

    clean_args[j] = NULL;

    if (clean_args[0] == NULL)
        return;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {

      
        if (input_file != NULL) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror(input_file);
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

       
        if (output_file != NULL) {
            int flags = O_WRONLY | O_CREAT;
            flags |= (append_out ? O_APPEND : O_TRUNC);

            int fd = open(output_file, flags, 0644);
            if (fd < 0) {
                perror(output_file);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        
        if (error_file != NULL) {
            int flags = O_WRONLY | O_CREAT;
            flags |= (append_err ? O_APPEND : O_TRUNC);

            int fd = open(error_file, flags, 0644);
            if (fd < 0) {
                perror(error_file);
                exit(1);
            }
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        execvp(clean_args[0], clean_args);

        if (errno == ENOENT)
            fprintf(stderr, "%s: command not found\n", clean_args[0]);
        else
            perror("exec failed");

        exit(127);
    }

    else {
        waitpid(pid, NULL, 0);
    }
}


























int is_builtin(const char *cmd){


   for(int i = 0; builtin[i] != NULL; i++){
      if (strcmp(cmd, builtin[i]) == 0){
           return 1;
           }
          
		            
   
   
   
   }
   return 0;



}

void resolve_path(const char *cmd){

  if (strchr(cmd , '/')){
      if (access(cmd , X_OK) == 0)
           printf("%s is %s\n " , cmd, cmd);
      else 
            printf("%s not recognized\n" , cmd);
      return;           
  }
  
 char *path_env = getenv("PATH");
 if (!path_env){
    printf("%s not recognized\n" , cmd);
    return;
 }
  
char *path_copy = strdup(path_env);
if (!path_copy){
   perror("strdup");
   return;
}

char *dir = strtok(path_copy , ":");
while (dir != NULL){
  char fullpath[1024];
   snprintf(fullpath, sizeof(fullpath), "%s/%s" , dir , cmd);
   
   if (access(fullpath, X_OK) == 0){
      printf("%s is %s\n" , cmd, fullpath);
      free(path_copy);
      return;
      }
      
      dir = strtok(NULL, ":");
      
      
   
   
   
   }

free (path_copy);
printf("%s not found\n" , cmd);










}



int main(int argc, char *argv[]) {
bool def_flag = false;

 char prompt_buffer[512];    	
setbuf(stdout, NULL);

while (true) {
     if (def_flag) {

        char cwd[128];
        if (!getcwd(cwd, sizeof(cwd)))
            strcpy(cwd, "?");

        char *user = getenv("USER");
        if (!user) user = "user";

        snprintf(prompt_buffer, sizeof(prompt_buffer),
                 COLOR_BLUE "[%s@myshell %s]" COLOR_RESET "$ ",
                 user, cwd);

    } else {

        snprintf(prompt_buffer, sizeof(prompt_buffer),COLOR_ORANGE"%s"COLOR_RESET, shell_prompt);
    }
    char *command = readline(prompt_buffer);

      
    if (command == NULL)
        break;

    if (*command)
        add_history(command);
    

    if (strcmp(command, "exit") == 0) {
        break;
    }
   
    /*else if (strncmp(command , "echo " , 5) == 0){
    
   char *echo_args[100];
   int arg_num = parse_input(command , echo_args);
   if (arg_num > 0){
     for (int j=1; j< arg_num; j++){
     printf("%s" , echo_args[j]);
     if (j < arg_num -1) printf(" ");
     
     
     }
     printf("\n");
     
   }
    
   for (int i = 0; i < arg_num; i++)
    free(echo_args[i]);
     
    continue;
    }*/
    
    else if (strncmp(command , "type " , 5) == 0){
          char *cmd = command + 5;
          if (is_builtin(cmd)) {
             printf("%s is a shell builtin\n" , cmd);
           
          }
          else {resolve_path(cmd);}
          
          continue;
          
        /*  else {
               printf("%s: not found\n" , cmd);
          } */
    
    
    
    
    }
   
   
   
  /*  else {
        printf("%s: command not found\n", command);
    } */

    else if (strncmp(command , "pwd" , 4) == 0){

     char cwd[PATH_MAX];
      if (getcwd(cwd,sizeof(cwd)) != NULL){
       printf("%s\n" , cwd);
        }
       else {
        perror("pwd");
      }
      
      continue;
    }


      
    else if (strncmp(command , "cd " , 3) == 0){

     char *cd_args[100];
      parse_input(command,cd_args);
      char *dir;

      if (cd_args[1] == NULL){
       dir = getenv("HOME");
        if (dir == NULL){

         fprintf(stderr, "cd: HOME not set\n");


        }

       

      }
       else if (strcmp(cd_args[1], "~") == 0) {
        dir = getenv("HOME");
        if (dir == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            continue;
        }      
        }
         
        else {
        dir = cd_args[1];
      }
       if (chdir(dir) != 0){

        printf("cd: %s: No such file or directory\n",dir);
      }
      for (int i = 0; cd_args[i] != NULL; i++)
      free(cd_args[i]);
      continue;






    }


else if (strncmp(command, "prompt ", 7) == 0){
  def_flag = false;
 char *new_prompt = command + 7 ;
 snprintf(shell_prompt, sizeof(shell_prompt),COLOR_ORANGE "%s "COLOR_RESET,  new_prompt);
 continue;


}
else if (strcmp(command, "default") == 0) {

    /* char cwd[128];
    if (!getcwd(cwd, sizeof(cwd))) {
         strcpy(cwd, "?");
}

    char *user = getenv("USER");
    if (!user) user = "user";

    snprintf(shell_prompt, sizeof(shell_prompt),
             COLOR_BLUE "[%s@myshell %s]" COLOR_RESET "$ ",
             user, cwd); */
def_flag = true;
    continue;
}


    



    char *args[100];
    parse_input(command, args);
    execute_command(args);
    for (int i = 0; args[i] != NULL; i++)
    free(args[i]);


}



return 0;
}
