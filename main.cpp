#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace bpt = boost::property_tree;

std::string RandomWords(size_t length) {
    const char* charmap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t charmapLength = strlen(charmap);
    auto generator = [&](){ return charmap[rand()%charmapLength]; };
    std::string result;
    result.reserve(length);
    generate_n(back_inserter(result), length, generator);
    return result;
}

std::string RandomString(size_t str_length){
    int sumbol_count = rand() % 9 + 1;
    std::string result;

    for(uint i = 0; i < str_length; ++i){
        std::string word = RandomWords(sumbol_count) + " ";
        result += word;
        sumbol_count = rand() % 9 + 1;
    }
    std::cout<<result;

    return  result;
}

void MakeConfigFile(std::string filename, bpt::ptree configTree){
    try{
        bpt::write_xml(filename, configTree);

    }
    catch(boost::property_tree::xml_parser_error){
        std::cout<<"XML parser error!"<<std::endl;
        throw;
    }
}

bpt::ptree ReadXml(std::string filename){
    bpt::ptree pt;
    read_xml(filename, pt, bpt::xml_parser::trim_whitespace);
    return pt;
}

void MakeFiles(bpt::ptree configTree){
    int file_n = configTree.get<int>("config.Processes");

    for(uint i = 0; i < file_n; ++i){
        char ch = configTree.get<char>("config.Inputprefix");
        std::string filename = "file"+ std::to_string(i);
        filename.push_back(ch);
        filename +=".txt";
        std::ofstream out(filename);

        int words_n = rand() % 20 + 1;
        std::cout<<filename<<" :\n";
        out << RandomString(words_n);
        std::cout<<"\n";
        out.close();
    }
}

int CountWords(std::string filename, clock_t &time){
    std::ifstream in(filename);
    int count = 0;

    std::string str;
    clock_t startTime = clock();
    if (in.is_open()){

        while (in){
            std::string strInput;
            in >> strInput;
            if(strInput != "\0"){
                ++count;
            }

        }
    }else{
        std::cout << "Файл не может быть открыт!\n";
    }
    clock_t endTime = clock();
    time = endTime - startTime;
    return count;
};

void MakeOutFile(const int &count,const bpt::ptree pt,
                 const std::string infilename, int pid, const clock_t &time, int i ){

    char outPrefix = pt.get<char>("config.Outputprefix");
    int maxSize = pt.get<int>("config.MaxFileSize");

    std::string filename = "file" + std::to_string(i);
    filename.push_back(outPrefix);
    filename+= ".txt";
    std::ofstream out(filename);

    int size = 0;
    std::ifstream in(infilename);
    in.seekg(0,std::ios::end);
    size = in.tellg();
    std::cout<<"-----------------------------------------\n";
    std::cout<<"SIZE: "<<size<<" byte"<<"\n";

    std::string err;
    if(size > maxSize){
        err = "!!! The size of the file must be " + std::to_string(maxSize)
                + " byte or less.\n";
    }

    if(out.is_open()){
        out <<err<<"name: "<< infilename + "\n"
        << "count: "<<count <<"\n" << "pid: " << pid << "\n"
            << "time: "<<time<<"\n";
        std::cout<<err<<"name: "<< infilename + "\n"
        << "count: "<<count <<"\n" << "pid: " << pid << "\n"
                 << "time: "<<time<<"\n";
    }else{
        std::cerr<<"Failed to open file";
        return;
    }
    out.close();
}

int main(){

//    bpt::ptree configTree;
//    configTree.put("config.Processes", 10);
//    configTree.put("config.Inputprefix", 'i');
//    configTree.put("config.Outputprefix", 'o');
//    configTree.put("config.MaxFileSize", 100);
//
//    MakeConfigFile("Config.xml", configTree);
    boost::property_tree::ptree pt = ReadXml("Config.xml");
    srand(time(NULL));

    MakeFiles(pt);
    int count;
    std::string infilename;

    srand(time(NULL));
    clock_t time;

    for (int i = 0; i < pt.get<int>("config.Processes"); i++) {
        pid_t pid = fork();
        if (pid == -1) {
            printf("Невозможно создать процесс\n");
            exit(1);
        }
        else if (pid == 0) {

            infilename = "file" + std::to_string(i);
            infilename.push_back(pt.get<char>("config.Inputprefix"));
            infilename += ".txt";
            count = CountWords(infilename,time);
            MakeOutFile(count,pt,infilename,getpid(),time,i);
            exit(0);
        }
        for (int proc = 0; proc <  pt.get<int>("config.Processes"); proc++) {
            int status;
            wait(&status);
        }
    }
    return 0;
}
