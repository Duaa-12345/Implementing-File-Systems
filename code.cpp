#include<iostream>

#include"json.hpp"

#include <stdio.h>

#include <string.h>

#include <cstring>

#include<vector>

#include<sstream>

#include<fstream>

#include<stack>



using namespace std;

using namespace nlohmann;   //this is used to include json in your code

char currentDir[]= "root";

#define TotalBlock 1000

const char* tableName = "fatTable.txt";

const char* directory = "jsonParser.txt";

const char* datablocks = "data.txt";

int blockSize = 512;

template<class T,class C>    //this is used for fat table to read memory allocated files

class row // this class works as a getter/setter for changing values in fat table

{

private:

T value;

C next;

public:

row(T f=0,C c=0)

{

value = f;

next = c;

}

T getValue()

{

return value;

}

C getNext()

{

return next;

}

void setValue(T s)

{

value = s;

}

void setNext(C c)

{

next = c;

}

};

class FileTable //used for 'fattable.txt'

{

private:

row<bool,int> table[1000]; // a table containing 1000 data blocks

public:

FileTable()

{

ifstream read;

read.open(tableName);  //open the file fattable.txt

int next;

bool value;

int i = 0;

if (read.is_open())  //check if table alreaddy exists for update use when a new file is imported

{

read >> value;

read >> next;

table[i].setValue(value);

table[i++].setNext(next);

read.close();

}

else  //otherwise for new table have empty data blocks with -1 at the end

{

for (int i = 0; i < 1000; i++)

table[i].setNext(-1);

}

}

int findFree()   //if a file is imported find the nxt free memory block to include it there

{

for (int i = 0; i < 1000; i++)

{

if (!table[i].getValue())

return i;

}

}
//getters and setter for rows of fat table

void resetNextOfRow(int i)   //if file is removed reset the memory data block reset (next)

{

if (i < 1000)

table[i].setNext(-1);

else

throw 1;

}

void setNextOfRow(int i,int n)   //setter(next)

{

if (i < 1000)

table[i].setNext(n);

else

throw 1;

}

void resetValueOfRow(int i) //if file is removed reset the memory data block reset(value)

{

if (i < 1000)

table[i].setValue(false);

else

throw 1;

}

void setValueOfRow(int i)  //setter(value)

{

if (i < 1000)

table[i].setValue(true);

else

throw 1;

}

int getNextOfRow(int i) //getter (next)

{

return table[i].getNext();

}

void writeInFile()    //this will write in table when a file is added in drive

{

ofstream write;

write.open(tableName);

for (int i = 0; i < 1000; i++)

{

write << table[i].getValue() << " " << table[i].getNext() << "\n";

}

write.close();

}

~FileTable()

{

ofstream write;

write.open(tableName);

for (int i = 0; i < 1000; i++)

{

write << table[i].getValue() << " " << table[i].getNext() << "\n";

}

write.close();

}

};

class DirectoryStructure       //this is where names of files are written in json parser.txt

{

public:

json root;

string File;

row<char*, json*>* cwd;

DirectoryStructure()

{

ifstream read;

read.open(directory);

if (read.is_open())

{

stringstream fileInStream;

while (read.peek()!=EOF)   //read json file and sent it to the parser to use it for commands

{

fileInStream << (char)read.get();

}

this->File = fileInStream.str();

read.close();

root = json::parse(File);

}

cwd = new row<char*, json*>((char*)"root", &root);



}

void saveState()    //this function writes the names that are to be saved as folders or files in json

{

ofstream write;

write.open(directory);

write << root.dump(4);

write.close();

}

~DirectoryStructure()

{

ofstream write;

write.open(directory);

write << root.dump(4);

write.close();

}



};

class FileSystem                              //Creating VHD Drive for use

{

private:

FileTable F; //Class filetable that creates a Fat table of 1000 memory and keeps track of it.

DirectoryStructure paths; //Class DirectoryStructure that stores your Virtaul hark Drive and the names all of its files and folders in json.

stack<row<char*, json*>*> track; //to use json pattern

char name[50]; //to get name of Virtual hard drive from the user when the programs runs for the very first time!



public:

FileSystem() //this constructors check whether there is already a hard drive made or a new one has to be created.

{

ifstream fin;


fin.open(datablocks); //A datablock file would have been made if there is already a virtual hard drive present in your project code directory


if (fin.is_open()) //if the datablock file is present then u access your present hard drive that you made during your first run

{

cout << "Welcome back to your drive" <<endl;

fin.close();



}

else { //if not then this is the first time you are running this code and you need to create your own hard drive

//these are required for setting up  a virtual harddisk, the blocksize can be in range of 1000 since we are going to have a fat table of 1000 size independent of this blocksize

cout << "Enter Block Size\n";  

cin>>blockSize;


cout << "Name your Drive\n";

cin.ignore();

cin.getline(name,50);


cout << "File system successfully mounted\n";

//after fullfing the required we create a datablock file as 'data.txt' to knw that we have a virtual drive now availaable for our use whenever we run this code

ofstream write(datablocks);

write.close();

}

}

void ls()   //this command will print the names of files and folders present in current directory of virtual hardrive

{

json* pwd = paths.cwd->getNext(); //this statement is used to fetch those names that are present in current directory

for (auto obj = pwd->begin(); obj != pwd->end(); ++obj) //a loop that print the names in the directory that have been fetched from json file from start to end

cout << obj.key() << "\n";

}

void mkdir(string arg) //this mkdir will create folders in your virtual harddrive

{

json* pwd = (paths.cwd->getNext()); //this will get the current directory

if (pwd->find(arg) == pwd->end()) //this will find if folder name already exist and/or take the pointer to the end of the directory so new folder can be added their

{

(*pwd)[arg] = nullptr;

this->paths.saveState(); //this will save the directory into paths 'jsonparser.txt';

}

else

cout << "duplicate files are not allowed\n"; //duplicate folders will not be allowed



}

void cd(string arg) //to change dir path ..similiar to ls

{

json* pwd = (paths.cwd->getNext());

if (arg.compare("..") == 0)

{

if (!track.empty())

{

paths.cwd = track.top();

track.pop();

}



}

else if (pwd->find(arg) != pwd->end())

{

if ((*pwd)[arg].is_number())

{

cout << "File can not be mounted\n";

}

else

{

track.push(paths.cwd);

char* buffer = new char[arg.size() + 1];

arg.copy(buffer, arg.size() + 1);

buffer[arg.size()] = '\0';

paths.cwd = new row<char*, json*>(buffer, &(*pwd)[arg]);

}



}

else

cout << "Directory not found\n";

}

void fetch(string arg)    //this is for import function

{

json* pwd = (paths.cwd->getNext()); //first we will take the path where the file needs to be imported

if (pwd->find(arg) == pwd->end()) //if the imported file is already present before then say duplicate files are not allowed otherwise start importing

{

ifstream read;

read.open(arg); //this the arg passed will also be a file so we open it to import

if (read.is_open()) //if file to be imported exists then open else show error

{

stringstream fileInStream;

while (read.peek() != EOF) //read untill eof and store in stringstream

{

fileInStream << (char)read.get();

}

string file = fileInStream.str(); //this will store the whole string in file

ofstream write;

write.open(datablocks, std::fstream::app);  //now open data.txt to keep record of imported

int i = 0;

int head = this->F.findFree(); //this will point to the next free memory locaation in Fat Table to fill it with the next file imported

int current = head;

while (i < file.size())   //this will write all data of imported file in data.txt to keep record

{

F.setValueOfRow(current); //busy bit is set

for(int j=0;j<blockSize && i<file.size();j++)

write << file[i++];

if (i < file.size())

{

F.setNextOfRow(current, F.findFree());

current = F.getNextOfRow(current);

}

else

{

F.setNextOfRow(current, -1);

}

}

(*pwd)[arg] = head;

this->paths.saveState(); //this will store name of tile imported in jsonparser.txt

}

else

cout << "fatal error: file not found\n";

}

else

cout << "duplicate files are not allowed\n";

}

void removeDir(json& root) //will be called throuugh rmdir

{

for (auto obj = root.begin(); obj != root.end(); obj++)

{

if (!obj.value().is_number()) {  

if (obj.value() != nullptr)  //if folder is empty remove the folder

{

removeDir(root[obj.key()]);

}

}

else

removeFile(obj.value()); //otherwise remove files in it then folder



}





}

void removeFile(int block)   //will be called through rmdir

{

int current = block;

F.resetValueOfRow(current);    //reset fat table when a file is removed

while (F.getNextOfRow(current) != -1)

{

int next = F.getNextOfRow(current);

F.resetNextOfRow(current);    

current = next;

F.resetValueOfRow(current);

}

}

void rmdir(string arg) //this is for removing folder/file

{

json* pwd = (paths.cwd->getNext()); //getting the current folder details in which the req folder/file is to be deleted

if (pwd->find(arg) != pwd->end())

{

if ((*pwd)[arg].is_number())

{

removeFile((*pwd)[arg]);    //this will remove file with the given arg

}

else

{

removeDir((*pwd)[arg]);     //this will remove folder with the given arg

}

(*pwd).erase(arg);        //refresh arg

this->paths.saveState();  //save drive after removing the required file/folder



}

else

cout << "Directory or file not found\n";

}

static void Parser(FileSystem& f)

{

cout << f.paths.cwd->getValue() << "$ ";  //to print root$ as our default path as it is stored in json as our default directory

char buffer[80];

cin.getline(buffer, 80);

std::istringstream ss(buffer);

std::string token;

string command;

string argu;

string extra;

std::getline(ss, command, ' ');    //to get command i,e. ls

std::getline(ss, argu, ' '); //to get commands that requres arg like mkdir usamsa..... mkdir will be store in command and usama in arg

std::getline(ss, extra, ' '); //extra to be used such as mkdir usama -a


// this will run all the commands required for the program



if (!extra.empty()) //if you to try pass unnecssary arguments after the commands this will be called

cout << "too many arguments\n";

else if (argu.empty() && (command.compare("ls") != 0)&&(command.compare("quit") != 0))  //to make sure 'ls' and  'quit' does not have argument and every other command have argument

cout << "too few arguments\n";


else if (command.compare("ls") == 0)

{

f.ls();

}

else if (command.compare("rmdir") == 0)

{

f.rmdir(argu);

}

else if (command.compare("cd") == 0)

{

f.cd(argu);

}

else if (command.compare("mkdir") == 0)

{

f.mkdir(argu);

}

else if (command.compare("import") == 0)

{

f.fetch(argu);

}

else if (command.compare("quit") == 0)

{

return;

}

else

cout << "Command not found\n";

Parser(f);

}



static void main(FileSystem& f)

{

Parser(f);       //second step to run commands // f will have the VHD given from terminal //this will run to get commands from the user to do something in your virtual hard drive

}

~FileSystem()

{



}

};



int main()

{

FileSystem mySystem; //Start from here! Go to this class firstly the constructor will run

FileSystem::main(mySystem); //After running constructor this is first step

return 0;



}

