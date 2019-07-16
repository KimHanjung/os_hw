#include <iostream>
#include <math.h>
#include <cstdlib>
#include <sys/time.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

using namespace std;


int main()
{
    time_t r;
    struct tm * ti;

    char **argv;
    int pid, i;
    string delimiter = " ";
    string s, token;
    size_t pos = 0;

    argv = new char*[100];
    for(i = 0; i<100; i++)
        argv[i] = new char[100];

    while(true)
    {
        time(&r);
        ti = localtime(&r); // get the current time
        i=0;
        cout<<"["<< ti->tm_hour <<":"<< ti->tm_min <<":"<< ti->tm_sec <<"]"<<"$ ";
        getline(cin, s, '\n');
        while ((pos = s.find(delimiter)) != std::string::npos) // split the input by space 
        {
            strcpy(argv[i++],s.substr(0, pos).c_str());
            s.erase(0, pos + delimiter.length());
        }
        strcpy(argv[i], s.c_str());

        if(strcmp(argv[0], "exit")==0) // if input command is exit, quit the while
            break;
        else if(strcmp(argv[0], "./program1")==0)
        {
            pid = fork();
            if(pid<0)
                perror("");
            else if(pid==0)
            {
                execvp(argv[0], argv);
                exit(1);
            }
            wait((int*)0);
        }
        else if(strcmp(argv[0], "./program2")==0)
        {
            pid = fork();
            if(pid<0)
                perror("");
            else if(pid==0)
            {
                execvp(argv[0], argv);
                exit(1);
            }
            wait((int*)0);
        }
        else if(strcmp(argv[0], "./program3")==0)
        {
            pid = fork();
            if(pid<0)
                perror("");
            else if(pid==0)
            {
                execvp(argv[0], argv);
                exit(1);
            }
            wait((int*)0);
        }
    }
    
	return 0;
}
