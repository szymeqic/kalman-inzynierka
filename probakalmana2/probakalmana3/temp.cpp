#include <iostream>
#include <fstream>
#include <string>

int main(){

    for(int i = 40; i>4; i--)
        std::cout <<"<option value="+std::to_string(i)+" label="+std::to_string(i)+"></option>\n";
    return 0;
}