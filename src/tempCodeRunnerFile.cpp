#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

using namespace std;

enum class TokenType{
    _return,
    int_lit,
    semi
};

struct Token{
    TokenType type;
    optional<string> value {};
};

vector<Token> tokenize(const string& str){
    vector<Token> tokens;

    string buf;

    for(int i = 0; i < str.length(); i++){
        char c = str.at(i);
        if(isalpha(c)){
            buf.push_back(c);
            i++;
            while(isalnum(str.at(i))){
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            if(buf == "return"){
                tokens.push_back({.type = TokenType::_return});
                buf.clear();
                continue;
            } else {
                cerr<<"You messed up"<<endl;
                exit(EXIT_FAILURE);
            }
        }else if(isdigit(c)){
            buf.push_back(c);
            i++;
            while(isdigit(str.at(i))){
                buf.push_back(str.at(i));
                i++;
            }
            i--;

            tokens.push_back({.type = TokenType::int_lit, .value = buf});
            buf.clear();
        }else if (c == ';'){
            tokens.push_back({.type = TokenType::semi});
        }
        else if(isspace(c)){
            continue;
        }
        
    }
    
}


int main(int argc, char* argv[]) {
    if(argc < 2){
        cerr<<"Incorrect Usage"<<endl;
        cerr<<"Usage: ./main <input.hy>"<<endl;
        return EXIT_FAILURE;
    }

    string content_stream;
    {   
        fstream input(argv[1], ios::in);
        stringstream fileStream;
        fileStream << input.rdbuf();
        content_stream = fileStream.str();
    }
    // input.close();

    tokenize(content_stream);
    


    cout<<argv[1]<< "\n";
    return EXIT_SUCCESS;
}
