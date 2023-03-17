#include <iostream>
class Page{
    int pageId=-1;
    public:
    Page(){}
    int GetPageId(){
        return pageId;
    }
};
int main(){
    Page* page = new Page[3];
    std::cout<<page[2].GetPageId()<<std::endl;
    return 0;
}