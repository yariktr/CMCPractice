#include <iostream>
#include <string>

class Rectangle {
protected:
    double width;
    double length;
    double square;
public:
    static int Num;

    Rectangle() : width(0), length(0) {
        square = width * length;
        Num++;
    }
    Rectangle(const double wid,const double len) : width(wid), length(len) {   
        square = width * length;
        Num++;
    }
    Rectangle(const Rectangle& rect) : width(rect.width), length(rect.length) {       
        square = width * length;
        Num++;
    }
    virtual ~Rectangle() {    
        Num--;
    }
    virtual double getSquare() const {
        return square;
    }
    virtual int compare(Rectangle& rect) const {  
        double diff = this->square - rect.square;
        if(diff > 0)
           return 1;
        else if(diff < 0) 
           return 2;
        else
           return 3;
    }
    Rectangle& operator=(const Rectangle& other) { 
        if (this != &other) {
            width = other.width;
            length = other.length;
            square = width * length;
        }
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const Rectangle& rect) {
        os << "Площадь: " << rect.square;
        return os;
    }
};

int Rectangle::Num = 0;

int main() {
    Rectangle rectangle1(1, 7);
    Rectangle rectangle2(1, 7);
    std::cout << rectangle1 << std::endl;
    std::cout << rectangle2 << std::endl;
    int id = rectangle1.compare(rectangle2);
    if (id != 3)
       std::cout << "Больше площадь прямоугольника: " << id << std::endl;
    else
       std::cout << "Пощади равны" << std::endl;
    Rectangle rectangle0 = rectangle1;
    //rectangle0.~Rectangle();
    std::cout << "Количество прямоугольников: " << Rectangle::Num << std::endl;
    return 0;
}