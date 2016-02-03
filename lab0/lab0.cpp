#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

static void processCommand(int argc, char* argv[], std::string* readFile, std::string* writeFile, std::string* dictionary, std::string* searchWord){
    int i = 1;
    try{
        while (i < argc){
            //dictionary name
            //we dont want to have both the search word and dictionary file
            if (strcmp(argv[i],"-f") == 0 && *searchWord == ""){
                i++;
                *dictionary = std::string(argv[i]);
            //write filename
            }else if (strcmp(argv[i], "-w") == 0 && *writeFile == ""){
                i++;
                *writeFile = std::string(argv[i]);
            }else{
                //if there is no dictionary, then it must be the word we need to search
                if (*dictionary == "" && *searchWord == ""){
                    *searchWord = std::string(argv[i]);
                //readfile has no argument available
                }else if (*readFile == ""){
                    *readFile = std::string(argv[i]);
                }else if (*readFile != ""){
                    throw -1;
                }
            }
            i++;
        }
        //if there are more arguments after the readfile name, throw error
        if (i < argc){
            throw -1;
        }
    }catch(...){
        std::cout << "INCORRECT ARGUMENT\nPROGRAM TERMINATED" << std::endl;
        exit(0);
    }
    if (*readFile == "" || (*searchWord == "" && *dictionary == "")){
        std::cout << "INCORRECT ARGUMENT\nPROGRAM TERMINATED" << std::endl;
        exit(0);
    }
}

//extract words from the dict
static std::vector<std::pair<std::string,int>> prepareSearchWords(std::string* dictionary, std::string* searchWord){
    std::vector<std::pair<std::string,int>> dict;
    std::ifstream file(*dictionary);
    //if we cant open text file, then we must only have single word to search
    if (!file){
        if (*searchWord == ""){
            std::cout << "DICT FAILED TO OPEN\nPROGRAM TERMINATED" << std::endl;
            exit(0);
        }
        std::pair<std::string,int> word_count(*searchWord,0);
        dict.push_back(word_count);
        return dict;
    }

    std::string word = "";
    std::pair<std::string,int> word_count;
    while (file >> word){
        word_count = std::make_pair(word,0);
        dict.push_back(word_count);
    }
    return dict;
}

//output words either on the console or into output file
static void outputResult(std::vector<std::string>outcome, std::string* writeFile){
    //writeFile pointer points to empty, we are going to print outcome on the consle
    if (*writeFile == ""){
        for (std::vector<std::string>::iterator it = outcome.begin(); it != outcome.end(); it++){
            std::cout << *it << std::endl;
        }
    }else{
        std::ofstream file(*writeFile);
        for (std::vector<std::string>::iterator it = outcome.begin(); it != outcome.end(); it++){
            file << *it;
            file << "\n";
        }
        file.close();
    }
}

int main(int argc, char* argv[]){
    //process argument
    std::string *readFile = new std::string();
    std::string *writeFile = new std::string();
    std::string *dictionary = new std::string();
    std::string *searchWord = new std::string();
    processCommand(argc, argv, readFile, writeFile, dictionary, searchWord);
    //std::cout << "Read From:" << *readFile << ";\nWrite To:" << *writeFile << ";\nDictionary:" << *dictionary << ";\nSearch Word:" << *searchWord << std::endl;

    //read dict files
    std::vector<std::pair<std::string,int>> dict = prepareSearchWords(dictionary, searchWord);

    //read file begins
    std::ifstream file(*readFile);
    if (!file){
        std::cout << "CANT OPEN FILE: " << *readFile << std::endl;
        return -1;
    }
    std::string word = "";
    while (file >> word){
        std::string precise_word = "";
        int length = word.length();
        int previousLetter = 0;
        for (int i = 0; i < length; i++){
            char thisLetter = word.at(i);
            //if is letter
            if (isalpha(thisLetter)){
                if (i - previousLetter != 1 && previousLetter != 0)
                    break;
                precise_word += thisLetter;
                previousLetter = i;
            }else if (int(thisLetter) == 39 && previousLetter != 0 && i <= length -2){
                if (word.at(i+1) == 's'){
                    precise_word += thisLetter;
                    previousLetter = i;
                }
            }else if (int(thisLetter) == -30 && previousLetter != 0 && i <= length -4){
                if (word.at(i+3) == 's'){
                    precise_word += thisLetter;
                    precise_word += char(-128);
                    precise_word += char(-103);
                    i += 2;
                    previousLetter = i;
                }
            }
        }
        //now we get the whole word without any comma, brackets etc.
        //so we compare each word with the dict
        for (std::vector<std::pair<std::string, int>>::iterator it = dict.begin(); it != dict.end(); it++){
            std::string thisWordInDict = (*it).first;
            //if is in the dict, we increase the count in the pair
            if (thisWordInDict.compare(precise_word) == 0){
                //std::cout << word << std::endl;
                (*it).second ++;
                break;
            }
        }
    }

    //reformat the outcome to become csv
    std::vector<std::string> outputString;
    for (std::vector<std::pair<std::string, int>>::iterator it = dict.begin(); it != dict.end(); it++){
        //std::cout << (*it).first << "," << (*it).second << std::endl;
        std::string temp = (*it).first + "," + std::to_string((*it).second);
        outputString.push_back(temp);
    }

    outputResult(outputString,writeFile);

    return 0;
}

