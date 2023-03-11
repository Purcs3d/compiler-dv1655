#ifndef BB_HPP
#define BB_HPP


#include "Tac.hpp"
#include <vector>
#include <fstream>

using namespace std;



class BB {
private: 
    std::string name;
    std::vector<Tac*> tac_instructions;
    Tac* condition; // the state of the block, use to later determine to to take true or false path
    BB* false_exit;
    BB* true_exit;

public:
    BB(int _id);
    ~BB();

    int id;
    /* Getters and setters */
    string getName();
    std::vector<Tac*>& getInstructions();
    Tac* getCondition();
    BB* getFalse();
    BB*  getTrue();
    void setFalse(BB* falseChild);
    void setTrue(BB* trueChild);

    void add_Tac(Tac* tac);
    void generate_BB();
    string generate_name(int id) const;


};

#endif // BB_HPP