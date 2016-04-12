#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "shell.h"

using namespace std;

bool fromFile = false;


// command handler
int handle_command(struct command * c);



// helpers
bool handle_errors();
bool isSpacey(char * s);
bool isLegalVar(char * s);
string whiteStrip(char *s);
string toUpper(char *s);

int main(int argc, char * argv[])
{
  ifstream file;

  if( argc > 1 ) {
    if( strlen(argv[1]) ) {
      file.open(argv[1], ifstream::in);
      if( !file.good() ){
	cerr << "INIT: error opening input file '" << argv[1] << "'" << endl;
	exit(1);
      } else {
	fromFile = true;
      }
    }
  }

  string inLine;

  if( !fromFile )
    cerr << SHELL_PROMPT;

  while( getline((fromFile ? file : cin), inLine) ){
    struct command *c;
    
    if(inLine.length()) {
      c = parse_command(inLine.c_str());
      handle_command(c);
      free_command(c);
    }
    
    if( !fromFile ) cerr << SHELL_PROMPT;
  }


  return 0;
}

int handle_command(struct command * c)
{
  // take first agrument
  char * first = c->args[0];

  // indicate that this was a bogus command
  if( first == NULL || isSpacey(first) ) return -1;

  //
  // handle built in shell commands
  //

  // handle directoty changes
  if( strcmp(first, "cd") == 0 ){
    char * second = c->args[1];
    
    if( second && !isSpacey(second)  && strcmp(whiteStrip(second).c_str(), "~") != 0 ){
      int val = chdir(second);
      if( val < 0 ){
	cerr << "Could not chdir to " << second << ": No such file or directory";
	cerr << "Command returned " << val;
      } else {
	return 0;
      }
    }
    // chdir to home dir
    else {
      char * hdir = secure_getenv("HOME");

      if( hdir == NULL) {
	cerr << "ERROR: Cannot cd, $HOME not set." << endl;
	return -1;
      }
      else
	return chdir(hdir);
    }
  } 
  // handle setting and unsetting environment vars
  else if( strcmp(first, "setenv") == 0 ){
    char * var = c->args[1];
    char * value = var != NULL ? c->args[2] : NULL;

    // SET an env var
    if( var != NULL && value != NULL ){

      if( !isLegalVar(var) ){
	cerr << "Could not setenv: Invalid argument" << endl;
	return -1;
      }
      
      return setenv( toUpper(var).c_str(), value, 1 );

    } 
    // UNSET an anv var
    else if( var != NULL && value == NULL ){
      return unsetenv( toUpper(var).c_str() );
    }
    // ERROR not using setenv right
    else  {
      cerr << "Could not setenv: Invalid argument" << endl;
      return -1;
    }

  }
  // handle the exit command
  else if( strcmp(first, "exit") == 0 ){
    exit(1);
  }
  // handle all other commands
  else {

    pid_t p;
    int fdout, fdin;
    int status;
    bool rein=false, reout=false;
    
    // start new process
    p = fork();

    // child process
    if( p == 0 ){

      // if we have a valid input redirect value
      if( c->in_redir != NULL && !isSpacey(c->in_redir) ){
	rein = true;
	fdin = open( c->in_redir, O_RDONLY );
	
	if( handle_errors() )
	  rein = false;

	dup2(fdin, 0);
      }
      
      // if wee have a valid ouput redirect value
      if( c->out_redir != NULL && !isSpacey(c->out_redir) ){
	reout = true;
	//dubout = dup(1);
	fdout = open(c->out_redir,  O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

	if( handle_errors() )
	  reout = false;

	dup2(fdout, 1);
      }
      
      int ret = 0;
      
      // execute external command
      if( (ret = execvp(c->args[0], c->args)) < 0 )
	cerr << "Error: failed to execute '" << c->args[0] << "'" << endl;

      // close input if opened
      if( rein ){
	dup2(0, fdin);
	close(fdin);
      }

      // close output if opened
      if( reout ){
	dup2(1, fdout);
	close(fdout);
      }
      
      if( ret < 0 )
	exit(-1);
      else
	exit(0);
    }
    // parent process
    else if ( p > 0 ){

      // wait for the child process to exit()
      waitpid(p, &status, 0); 

      if( status ) cerr << "Commmand returned " << status;

      return status;
    }
    else {
      // fork failed
      cerr << "fork() failed!" << endl;
      return -1;
    }

  }

  return -1;
}

// deal with the errno
bool handle_errors()
{
  switch( errno ){
  case EISDIR: 
    cerr << "Error: target is directory" << endl;
    break;
  default:
    if( errno != 0 ){
      cerr << "Error: target is not accessible" << endl;
    }
    break;
  }

  return (errno == 0);
}

// see if the string is only space
bool isSpacey(char * s)
{
    bool onlySpaces = true;
    for( char i:string(s) )
      if( i > 32 )
	onlySpaces = false;
    
    if( onlySpaces )
      return true;
    return false;
}

// strip trailing and beginning whitespace
string whiteStrip(char * s)
{
  string out(s);

  while( out.at(0) == ' ' )
    out = out.erase(0, 1);
  while( out.at(out.size()-1) == ' ' )
    out.pop_back();

  return out;
}

// convert a-z to uppercase
string toUpper(char *s)
{
  string out(s);
  
  for( size_t i=0; i<out.length(); i++ ){
    if( out[i]<=122 && out[i]>=97 )
      out[i] -= 32;
  }
  
  return out;
}

// check var name
bool isLegalVar(char *s)
{
  for( char c:string(s) ){
    if( !((c>=65 && c <= 90) || (c>=97 && c<=122)) )
      return false;
  }

  return true;
}
 
