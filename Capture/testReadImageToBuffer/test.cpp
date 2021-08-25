#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <ios>      // std::ifstream

int main()
{
    std::ifstream DataFile("abc.jpeg", std::ios::binary);
    if(!DataFile.good())
        return 0;

    DataFile.seekg(0, std::ios::end);
    size_t filesize = (int)DataFile.tellg();
    DataFile.seekg(0);
	std::cout << "Size of image is: " << filesize ;
    unsigned char output[filesize];
    //or std::vector
    //or unsigned char *output = new unsigned char[filesize];
    if(DataFile.read((char*)output, filesize))
    {
        std::ofstream fout("def.jpeg", std::ios::binary);
        if(!fout.good())
            return 0;
        fout.write((char*)output, filesize);
    }

    return 0;
}
