#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[]){

	std::string string;
	std::string line;
	std::ifstream file;

    file.open(argv[1], std::ifstream::in);	
    if( file.fail() ) { 
    	std::cerr << "Error: Read file failed. Is the path correct?" << std::endl;
    	exit(1);
    }

	while(std::getline(file, line)){	//an thes mia mono grammi, peirakse edo
		string += line; 
    }

  	std::cout << "Hooray!" << std::endl << string << std::endl;

  	return 0;
}