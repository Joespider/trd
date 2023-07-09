#include <iostream>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <stdio.h>
#include <thread>
#include <vector>
#include <fstream>

//print Macro for cout
#define print(x); std::cout << x << std::endl

//error Macro for cerr
#define error(x); std::cerr << x << std::endl

//Convert std::string to String
#define String std::string

//Str for to_strin()
#define Str(x) std::to_string(x)

static void help();
bool StartsWith(String Str, String Start);
void shell(String threadNum, String command, bool BeQuiet, bool ToLog, bool ToLogWithTrd);
int len(std::vector<String> Vect);

static void help()
{
	String TheName = "trd";
	String Version = "0.1.03";
	print("Author: Dan (DJ) Coffman");
	print("Program: \"" << TheName << "\"");
	print("Version: " << Version);
	print("Purpose: \"run multiple commands at once\"");
	print("Usage: " << TheName << " <args>");
	print("{REQUIRED}")
	print("\t-c <command/script>\t\t: run commands or script");
	print("\t--command <command/script>\t: run commands or script");
	print("");
	print("{OPTIONAL}")
	print("\t-q\t\t\t\t: hide output");
	print("\t--quiet\t\t\t\t: hide output");
	print("");
	print("\t--log\t\t\t\t: save output to a log (each thread is its own log file)");
	print("\t\t\t\t\t: -c \"ls\" would save output in \"ls.log\"");
	print("");
	print("\t--log-trd\t\t\t: save output to a log (each thread is its own log file)");
	print("\t\t\t\t\t: -c \"ls\" would save output in \"{trd[<number>] \"ls\"}.log\"");
	print("");
	print("{EXAMPLE} Run three commands at once");
	print("\t$ " << TheName << " -c ls -c \"echo hi\" -c \"whoami\"");
	print("\t{trd[1]} \"ls\"} Desktop");
	print("\t{trd[1]} \"ls\"} Documents");
	print("\t{trd[1]} \"ls\"} Downloads");
	print("\t{trd[1]} \"ls\"} Music");
	print("\t{trd[1]} \"ls\"} Pictures");
	print("\t{trd[1]} \"ls\"} Public");
	print("\t{trd[1]} \"ls\"} Videos");
	print("\t{trd[2]} \"echo hi\"} hi");
	print("\t{trd[3]} \"whoami\"} user");
}

//Check if string begins with substring
bool StartsWith(String Str, String Start)
{
	bool ItDoes = false;
	if (Str.rfind(Start, 0) == 0)
	{
		ItDoes = true;
	}
	return ItDoes;
}

int len(std::vector<String> Vect)
{
	int StrLen = Vect.size();
	return StrLen;
}

void shell(String threadNum, String command, bool BeQuiet, bool ToLog, bool ToLogWithTrd)
{
	std::ofstream myfile;
	char buffer[128];

	// Open pipe to file
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe)
	{
		error("popen failed!");
	}

	//Save command output to a log file
	if ((ToLog == true) || (ToLogWithTrd == true))
	{
		String filename = "";
		if (ToLog == true)
		{
			//Generate File name for each thread
			filename = command+".log";
		}
		else if (ToLogWithTrd == true)
		{
			//Generate File name for each thread
			filename = "trd["+threadNum+"] \""+command+"\".log";
		}
		myfile.open(filename.c_str());
	}

	// read till end of process:
	while (!feof(pipe))
	{
		// use buffer to read and add to result
		if (fgets(buffer, 128, pipe) != NULL)
		{
			if ((ToLog == true) || (ToLogWithTrd == true))
			{
				myfile << buffer;
			}
			if (BeQuiet == false)
			{
				std::cout << "{trd["+threadNum+"] \""+command+"\"} ";
				std::cout << buffer;
			}

		}
	}

	if (ToLog == true)
	{
		myfile.close();
	}

	pclose(pipe);
}

//C++ Main...with cli arguments
int main(int argc, char** argv)
{
	std::vector<String> myCommands;
	std::vector<std::thread> myThreads;
	int numOfThreads;
	String ThreadNum;
	String out = "";
	String value = "";
	int next = 0;
	bool Quiet = false;
	bool SaveToLog = false;
	bool SaveToLogWithTrd = false;

	bool IsNotOk = true;
	//Args were given
	if (argc > 1)
	{
		//Loop through Args
		for (int i = 1; i < argc; i++)
		{
			out = String(argv[i]);
			if ((out == "-c") || (out == "--command"))
			{
				next = i+1;
				if (next < argc)
				{
					value = String(argv[next]);
                                        IsNotOk = StartsWith(value,"-");
                                        if (IsNotOk == false)
					{
						myCommands.push_back(value);
					}
				}
			}
			else if (out == "--log")
			{
				SaveToLog = true;
				SaveToLogWithTrd = false;
			}
			else if (out == "--log-trd")
			{
				SaveToLog = false;
				SaveToLogWithTrd = true;
			}
			else if ((out == "-q") || (out == "--quiet"))
			{
				Quiet = true;
			}
		}

		numOfThreads = len(myCommands);
		//Lets get our threads started
		if (numOfThreads > 0)
		{
			for (int lp = 0; lp != numOfThreads; lp++)
			{
				int TrdN = lp + 1;
				ThreadNum = Str(TrdN);
				std::thread ThreadName(shell,ThreadNum,myCommands[lp],Quiet,SaveToLog,SaveToLogWithTrd);
				myThreads.push_back(move(ThreadName));
			}
			for (int lp = 0; lp != numOfThreads; lp++)
			{
				myThreads[lp].join();
			}
		}
		else
		{
			help();
		}
	}
	else
	{
		help();
	}

	return 0;
}
