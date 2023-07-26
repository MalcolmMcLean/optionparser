/*
   Test program for the options parser. Also use as a guide for
   how to use it.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

/*
   Don't use for general purposes.
   Just a quick function to enable the test program to enter lines
     conveniently;
*/
char **commandlinetoargv(const char *commandline, int *argc)
{
   char **answer = 0;
   char *buff = 0;
   char *token;
   char **temp = 0;
   int N = 0;

   buff = malloc(strlen(commandline) + 1);
   if (!buff)
      goto error_exit;
   strcpy(buff, commandline);

   token = strtok(buff, " ");
   while(token)
   {
      temp = answer;
      answer = realloc(answer, (N +2) * sizeof(char *));
      if (!answer)
        goto error_exit;
      temp = 0;
      answer[N++] = token;
      token = strtok(NULL, " ");
   }
   answer[N] = 0;
   
   *argc =  N;
   return answer;

error_exit:
    *argc = 0;
    free(buff);
    free(answer);
    free(temp);
    return 0;
}

void killarguments(char **argv)
{
   if (argv)
   {
     free(argv[0]);
     free(argv);
   }
}

/*
   accepts flags a, b, c with a filename
*/
int flagsmain(int argc, char **argv)
{
   OPTIONS *opt;
   int flag_a = 0;
   int flag_b = 0;
   int flag_c = 0;
   char *filename = 0;
   int N;
   int good = 1;

   opt = options(argc, argv, "abc");
   if (!opt)
   {
      fprintf(stderr, "Constructor failed\n");
      exit(EXIT_FAILURE);
   }
   flag_a = opt_get(opt, "-a", 0);
   flag_b = opt_get(opt, "-b", 0);
   flag_c = opt_get(opt, "-c", 0);
   N = opt_Nargs(opt);
   if (N == 1)
     filename = opt_arg(opt, 0);
   else
     good = 0;
   if (opt_error(opt, stderr))
     good = 0;

   if (good)
   {
     printf("flagsmain successful\n");
     printf("a %d b %d c %d filename %s\n", flag_a, flag_b, flag_c, 
filename);
   }
   else
   {
     printf("flagsmain commandline contained errors\n");
   }
   printf("\n");

   killoptions(opt);
   free(filename);

   return 0;
}

int longoptionsmain(int argc, char **argv)
{
   OPTIONS *opt;
   int opt_boolean = 0;
   int opt_integer = 42; // integer option, default 42
   double opt_real = 42.0; // real option, default 42.0
   char opt_string[9] = "Fred"; // short string option max 8 chars
   int opt_x = 0;  // double option
   int opt_y = 0;
   int N;  // number of non-option arguments
   int i;
   char *filename = 0; // non-option argument 
   int good = 1;

   opt = options(argc, argv, NULL);
   if (!opt)
     fprintf(stderr, "Constructor failed\n");
   opt_boolean = opt_get(opt, "-boolean", NULL);
   opt_get(opt, "-integer", "%d", &opt_integer);
   opt_get(opt, "-real", "%f", &opt_real);
   // because forgetting the nul is so likely, pass 9 for an 8-character 
   // string.
   opt_get(opt, "-string -str -s", "%9s", opt_string);
   opt_get(opt, "-coords", "%d%d", &opt_x, &opt_y);
   N = opt_Nargs(opt);
   for (i =0; i < N; i++)
   {
      filename = opt_arg(opt, i);
      printf("arg %d:%s\n", i, filename);
      free(filename);
   }
   if (N < 1)
      good = 0;
   if (opt_error(opt, stderr))
      good = 0;

   if (good)
   {
      printf("boolean %d integer %d real %f string %s coords %d, %d\n",
        opt_boolean, opt_integer, opt_real, opt_string, opt_x, opt_y); 
      printf("longoptionsmain succeeeded\n");
   }
   else
   {
     printf("longoptionsmain commandline contained errors\n");
   }
   printf("\n");

   killoptions(opt);
  
   return 0;
}

int main(int argc, char **argv)
{ 
   char **argv2;
   int argc2;

   /* OK */
   argv2 = commandlinetoargv("programname fredthefile.txt", &argc2);
   flagsmain(argc2, argv2);
   killarguments(argv2);

   /* OK */
   argv2 = commandlinetoargv("programname -a fredthefile.txt", &argc2);
   flagsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, unrecognised flag x */
   argv2 = commandlinetoargv("programname -ax fredthefile.txt", &argc2);
   flagsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, duplicate flag a */
   argv2 = commandlinetoargv("programname -aba fredthefile.txt", &argc2);
   flagsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, argument  starts with a - */
   argv2 = commandlinetoargv("programname -fredthefile.txt", &argc2);
   flagsmain(argc2, argv2);
   killarguments(argv2);

   /* OK */
   argv2 = commandlinetoargv("programname fredthefile.txt", &argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* OK */
   argv2 = commandlinetoargv("programname -boolean -integer 1 -real 2.0 -string 12345678 -coords 10 12 fredthefile.txt", &argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, string too long */
   argv2 = commandlinetoargv("programname -s 123456789 fredthefile.txt", 
&argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, argument not an integer */
   argv2 = commandlinetoargv("programname -integer 3.14 fredthefile.txt", 
&argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, stupidly large integer */
   argv2 = commandlinetoargv("programname -integer 1000000000000000000000 fredthefile.txt", &argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, argument not a real */
   argv2 = commandlinetoargv("programname -real three fredthefile.txt", 
&argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, not enough args for coords */
   argv2 = commandlinetoargv("programname -coords 10 fredthefile.txt", 
&argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);

   /* fails, unrecognised option */
   argv2 = commandlinetoargv("programname -error fredthefile.txt", 
&argc2);
   longoptionsmain(argc2, argv2);
   killarguments(argv2);


   /* Call with user arguments */
   longoptionsmain(argc, argv);
   return 0;
   
}
  
