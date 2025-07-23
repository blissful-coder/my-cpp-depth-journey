#include <iostream>
#include <string>


class Person{
    int age;
    std::string name;

    public:
        Person(int age, std::string name){
            this->age = age;
            this->name = name;
        }
        void show(){
            std::cout<<"Age: " << this->age <<'\n';
            std::cout<<"Name: " << this->name <<'\n';
        }
};

int main(){
    Person anurag(32,"Anurag");
    anurag.show();
}
