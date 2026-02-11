// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  string line;
  
  //the variable of type ifstream:
  ifstream myfile ("example.txt");
  
  //check to see if the file is opened:
  if (myfile.is_open())
  {
    //while there are still lines in the
    //file, keep reading:
    while (! myfile.eof() )
    {
      //place the line from myfile into the
      //line variable:
      getline (myfile,line);

      //display the line we gathered:
      cout << line << endl;

      string token;
      while(token != line)
      {
          token = line.substr(0,line.find_first_of(" "));
          line = line.substr(line.find_first_of(" ") + 1);
          printf("%s\n",token.c_str());
       }
    }

    //close the stream:
    myfile.close();
  }

  else cout << "Unable to open file";

  return 0;
}
