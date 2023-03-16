#include "BC.hpp"


InstructionBC::InstructionBC(int _id, string _instruction_argument) 
    : id(_id), instruction_argument(_instruction_argument) {}
InstructionBC::~InstructionBC() {}
void InstructionBC::print(ofstream* outStream) const {
    *outStream << instruction_argument << "\n";
}
int InstructionBC::getID() {
    return this->id;
}

void InstructionBC::stdio_out() {
    cout << instruction_argument << "\n";
}

string& InstructionBC::getInstructionArgument() {
    return this->instruction_argument;
}

MethodBC::MethodBC(vector<string> &_variables, vector<InstructionBC*> &_instructions, string _block_name) 
    : variables(_variables), instructions(_instructions), block_name(_block_name) {}
MethodBC::MethodBC(string _block_name) : block_name(_block_name) {}
MethodBC::~MethodBC() {}
void MethodBC::print(ofstream* outStream) const {
    for (auto const& instruction : instructions) {
        instruction->print(outStream);
    }
}
void MethodBC::set_name(string newBlockName) {
    /*Sets block name*/
    this->block_name = newBlockName;
}

string MethodBC::get_name() {
    return block_name;
}

vector<InstructionBC*>& MethodBC::getInstructions() {
    return this->instructions;
}

void MethodBC::addInstruction(InstructionBC* instr) {
    this->instructions.push_back(instr);
}

ProgramBC::ProgramBC() : index(0) {}
ProgramBC::~ProgramBC() {}

string ProgramBC::first_part_string(string orig) {
    string first_half;
    stringstream ss(orig);
    while(ss >> first_half) {
        return first_half;
    }
}

void ProgramBC::print() const {
    std:ofstream outStream;
	char* filename = "program.out"; // Outfile name
	outStream.open(filename);
    int bc_vec_sz = bc_methods.size();
    int qual_meth_vec = qual_method_names.size();
    if (bc_vec_sz == qual_meth_vec) {
        for (int i = 0; i < bc_vec_sz; ++i) {
            string func_name_label = qual_method_names[i].first + "." + qual_method_names[i].second;
            outStream << func_name_label << "\n";
            bc_methods[i]->set_name(func_name_label); // change block_name to func_name_label
            bc_methods[i]->print(&outStream);
        }
    }
    else {
        cout << "ERROR!!! bc_vec_sz and qual_meth_vec is not of same size" << endl;
    }
    outStream.close();
}


void ProgramBC::add_method(MethodBC* meth) {
    this->bc_methods.push_back(meth);
}

void ProgramBC::add_qual_name(pair<string, string> name_pair) {
    this->qual_method_names.push_back(name_pair);
}

const vector<MethodBC*>& ProgramBC::getBCmethods() const {
    return this->bc_methods;
}

MethodBC* ProgramBC::getNextBCmethod() {
    /*
        Gets the next BC method in the vector
    */
    this->index += 1;
    return this->bc_methods[index];
}

void ProgramBC::readline(string line) {
    /*
        Given a line of program code. Add instruction or method to programbc
    */
   string methodRegex = "(.*)\\.(.*)";
   string blockRegex = "(Block_)(.*)";

   if (regex_match(line, regex(methodRegex))) {
        /*
            1. create new method
            2. Add method to the programs methods
            3. set the programs current method to the new method
        */ 
        cout << line << endl;
        MethodBC* method_bc = new MethodBC(line);
        this->add_method(method_bc);
        this->currentMethod = method_bc;
    }
    else if (regex_match(line, regex(blockRegex))) {
        /*
            1. create label instruction
            2. Add to current method
        */
       cout << line << endl;
       InstructionBC* instr = new InstructionBC(19, line);
       currentMethod->addInstruction(instr);
    }
   else {
        // Must be InstructionBC
        /*
            1. Get first part of instruction
            2. Check type of instruction
            3. Create instruction bc
        */
       string first_part = this->first_part_string(line);
       cout << first_part << endl;
       string regiadd = "";
       int id;
       // get first part of string
       if (regex_match(first_part, regex(regiadd))) {

       }
   }
}