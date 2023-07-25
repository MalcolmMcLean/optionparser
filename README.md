# optionparser
A commandline options parser in portable ANSI C

The option parser is designed to allow calling programmers
to parse options cleanly and easily in the main() function,
taking most of the error checking and parse logic out of
the calling program.
 
## Public functions

### options

    OPTIONS *options(int argc, char **argv, char *flags);

Start the options parser. Pass it argc and argv from the arguments to 
main, and a list of single character flags e.g. "lbw" for options
"-l", "-b", -"w". You don't need to pass non-single character options.

### killoptions

    void killoptions(OPTIONS *opt);

Destroys the options object. 

### opt_get

    int opt_get(OPTIONS *opt, char *name, char *fmt, ...);

Get an option. 

    name - "-option -o -opt -OPT" - list of alternative names
    fmt - "%d%32s%f" - list of option parameters
          %d - an integer - argument int *
          %f - a real - argument double *
          %s - a string - argument char *
          strings should take a buffer length qualifer (default 256)

Returns the number of arguments consumed. However it's not normally
necessary to check for errors. Call opt_error after parsing all the
options.


### opt_error

    int opt_error(OPTIONS *opt, FILE *fp);

Returns non-zero if the options parser encountered an error. It then
writes an human-readable error report to the file. So normally you would
pass stderr as the fp argument. It can be null if you don't want to
report the error to the user.

### opt_Nargs

    int opt_Nargs(OPTIONS *opt); 

This gets the number of arguments left after otions have been parsed.

### opt_arg

    char *opt_arg(OPTIONS *opt, int index);

Retrieves the commandline non-option argument (most frequently file 
names). You must call after parsing the options.

## Example Usage

    int main(int argc, char *argv)
    {
       int width; // an integer ootion
       char name[32]; // a string option
       double quality; // a floating point option 
       char *filename; // a file argument

       OPTIONS *opt = options(argc, argv, "");
       if (!opt)
          exit(EXIT_FAILURE);

       opt_get(opt, "-width -w", "%d", &width);
       opt_get(opt, "-name", "%32s", name);
       opt_get(opt, "-quality -q", "%f", &quality);

       if (opt_Nargs != 1)
           usage();          // user hasn't entered a file name

        filename = opt_arg(opt, 0);

       if (opt_error(opt, stderr))
          exit(EXIT_FAILURE);  // error message should be anough to tell
             // user what is wrong.
     
        killoptions(opt);
        opt = 0;

        if (width <= 0)
           fprintf(stderr, "width must be positive\n"); // It won't sanity
              // test options for you

        dowork(filename, width, name, quality);

        return 0;   
    }

It's very clean and easy to use.
