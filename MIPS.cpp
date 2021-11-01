#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize 65536


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      // TODO: implement!
      if (WrtEnable.to_ulong() == 1){  //Enable bit controls whether to write or not We might not want to write all the time, if 0, we don't write, if 1, we write
          Registers[WrtReg.to_ulong()] = WrtData; //we use to_ulong long here to trun bitset content to a long integer
      }

      ReadData1 = Registers[RdReg1.to_ulong()]; //ReReg1 is the first source register which select one of 32 registers and connect it to the output ReadData1
      ReadData2 = Registers[RdReg2.to_ulong()]; //ReReg2 is the second source register which select one of 32 registers and connect it to the output ReadData2
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {

        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j] << endl;
        }

      }
      else cout<<"Unable to open file";
      cout<<"RFtester"<<endl;  //debug parameter
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        unsigned int result;
      // TODO: implement!
      if(ALUOP.to_ulong() == ADDU){
          result = oprand1.to_ulong() + oprand2.to_ulong();
      }
      else if(ALUOP.to_ulong() == SUBU){
          result = oprand1.to_ulong() - oprand2.to_ulong();}
      else if(ALUOP.to_ulong() == AND){
          result = oprand1.to_ulong() & oprand2.to_ulong();}
      else if(ALUOP.to_ulong() == OR){
          result = oprand1.to_ulong() | oprand2.to_ulong();}
      else if(ALUOP.to_ulong() == NOR){
          result = ~(oprand1.to_ulong() | oprand2.to_ulong());}

      return ALUresult = bitset<32>(result);
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {      
      IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line.substr(0,8));
          i++;
        }

      }
      else cout<<"Unable to open file imem";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      // (Read the byte at the ReadAddress and the following three byte).
      string a;
      for (int i=0; i<4; i++){
          a += IMem[ReadAddress.to_ulong() + i].to_string();
      }
      bitset<32> Inst(a);
      Instruction = Inst;
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line.substr(0,8));
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      // TODO: implement!

      if ((readmem.to_ulong() == 1 ) && (writemem.to_ulong() == 1)){
          cout<<"Read and write cannot execute together"<<endl;
      }
      else if ((readmem.to_ulong()==1) && (writemem.to_ulong() == 0)){
          string data;
          for (int i = 0; i<4; i++){
              data += DMem[Address.to_ulong() + i].to_string();
          }
          bitset<32> Data(data);
          readdata = Data;
      }
      else if ((readmem.to_ulong() == 0) && (writemem.to_ulong() == 1)){
          for (int i = 0; i < 32; i += 8){
              string result = WriteData.to_string().substr(i,8);
              bitset<8> finalResult(result);

              DMem[Address.to_ulong() + i/8] = finalResult;


          }
      }
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
        ///Users/yingcanchen/Desktop/lab1_Testcases/1/
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  
bitset<5> acquirebitsininstr(bitset<32> instruction, int start, int end);


int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  int PC = 0;
  bitset<32> programCounter=0; // 32 bit register program counter initialized to 0
  bitset<32> instruction;

  bitset<5> Rs;
  bitset<5> Rt;
  bitset<5> Rd;
  bitset<5> shamt;
  bitset<6> funct;
  bitset<3> ALUop;
  bitset<32> result;
  bitset<16> Immidiate;
  string signimmediate;

  //bitset<3> add(ADDU);
  unsigned long counter = 0;
  while (1)
  {

    // Fetch
    instruction = myInsMem.ReadMemory(programCounter.to_ulong());
    cout<<"Current instruction"<<instruction<<endl;
    // If current insturciton is "11111111111111111111111111111111", then break;
    if (myInsMem.ReadMemory(programCounter.to_ulong()) == 0xFFFFFFFF)break;

    // decode(Read RF)
    string opcode = instruction.to_string().substr(0,6);
    // Use opcode to determine the type of instruction
    if (opcode == "000000")
        cout<<"R instruction"<<endl;
    else if (opcode == "000010")
        cout<<"J instruction"<<endl;
    else if (opcode == "111111")
        cout<<"halt instruction"<<endl;
    else if (opcode == "001001")
        cout<<"addiu"<<endl;
    else if (opcode == "000100")
        cout<<"beq"<<endl;
    else if (opcode == "100011")
        cout<<"lw"<<endl;
    else if (opcode == "101011")
        cout<<"sw"<<endl;

    if (opcode == "000000"){ //execute R-type instruction
      string rs = instruction.to_string().substr(6,5); //R type instruction has 5 bits rs source operand registers
      string rt = instruction.to_string().substr(11,5); //R type instruction has 5 bits rt source operand registers
      string rd = instruction.to_string().substr(16,5); //R type instruction has 5 bits rd destination operand registers
      string funct = instruction.to_string().substr(26,6); // R type instruction has 5 bits sub-opcode identifier
      string aluop = instruction.to_string().substr(29,3); //THe last three bits of the R type instruction can be used to determine the operation

      Rs = bitset<5>(rs);
      Rt = bitset<5>(rt);
      Rd = bitset<5>(rd);
      ALUop = bitset<3>(aluop);
      if (ALUop==1)
          cout<<"addu instruction"<<endl;
      else if(ALUop==3)
          cout<<"subu instruction"<<endl;
      else if(ALUop==4)
          cout<<"and instruction"<<endl;
      else if(ALUop==5)
          cout<<"or instruction"<<endl;
      else if(ALUop==7)
          cout<<"nor instruction"<<endl;
      myRF.ReadWrite(Rs,Rt,Rd,0,0);
        cout << "R[rs]=R["<<Rs.to_ulong()<<"]= " << myRF.ReadData1 << endl;
        cout << "R[rt]=R["<<Rt.to_ulong()<<"]= " << myRF.ReadData2 << endl;
      result = myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
      myRF.ReadWrite(0,0,Rd,result,1);
      programCounter = bitset<32>(programCounter.to_ulong()+4);
    }
    else if (opcode == "000010" || opcode == "111111"){  //J-Type instructions  or halt instruction
        if (opcode == "000010") {
            string jaddress = instruction.to_string().substr(6,26);
            cout << "jump instruction" << endl;
            bitset<26> JAddress(instruction.to_string().substr(6,26)); //Obtain 26 bits address from the "target" field of the J-type instruction

            bitset<32> pc(programCounter.to_ulong() + 4);
            programCounter = pc;

            bitset<4> pc4HOB(programCounter.to_string().substr(0, 4));// Obtain the 4 highest order bits from program counter
            bitset<32> newPC(pc4HOB.to_string() + JAddress.to_string() +"00");//Combine the highest order bits, 26 bits address and add two extra zeros for alignment
            cout << "jump to new address"<<newPC<<endl;
            newPC << 2;
            programCounter = newPC;
        }
        else if (opcode == "111111"){
            cout<<"halt instruction received"<<endl;
            break;
        }
    }
    else { //Execute I-type instruction
        cout<<"I-type instruction"<<endl;
        string rs = instruction.to_string().substr(6, 5); // I type register have 5 bits for source operand register rs
        string rt = instruction.to_string().substr(11, 5); // I type register have 5 bits for "target" operand register rt
        string immediate = instruction.to_string().substr(16, 16); // I type register have 16 bits for immediate to store data constant or address
        Rs = bitset<5>(rs);
        Rt = bitset<5>(rt);

        Immidiate = bitset<16>(immediate);

        myRF.ReadWrite(Rs, Rt, NULL, NULL, 0);
        cout << "R[rs]=R["<<Rs.to_ulong()<<"]= " << myRF.ReadData1 << endl;
        cout << "R[rt]=R["<<Rt.to_ulong()<<"]= " << myRF.ReadData2 << endl;
        cout << "Immediate"<< Immidiate.to_ulong()<<endl;

        signimmediate = Immidiate.to_string();

        for(int j = 0; j < 16; j++){
            signimmediate = Immidiate.to_string().at(0) + signimmediate;
        }


        cout<<"OffsetImmediate"<<signimmediate<<endl;
        bitset<32> SImm(signimmediate);

        result = myRF.ReadData1.to_ulong() + SImm.to_ulong();
        if(opcode != "000100") {
            //Read/Write MEM, lw, addiu and sw instruction will either update register data or up date data stored in memory
            if (opcode == "100011") {//lw instruction
                cout<<"lw instruction"<<endl;
                cout<<"lw, update register data"<<endl;
                bitset<32> operand1 = myDataMem.MemoryAccess(result, NULL, 1, 0);
                myRF.ReadWrite(Rs, Rd, Rt, operand1, 1);


            } else if (opcode == "001001") { //addiu instruction
                cout<<"addiu instruction"<<endl;

                result = myRF.ReadData1.to_ulong() + SImm.to_ulong();
                myRF.ReadWrite(Rs, Rd, Rt, result, 1);

            } else if (opcode == "101011") { //sw instruction
                cout<<"sw instruction"<<endl;
                cout<<"update data stored in memory"<<endl;
                bitset<32> storetomemory = myDataMem.MemoryAccess(result, myRF.ReadData2, 0, 1);

            }
            programCounter = bitset<32>(programCounter.to_ulong() + 4);
        }
        else if (opcode == "000100") {  //beq instruction Branch is equal
             //the instruction branches the Program counter if the first source register's contents and the second source register's contents are equal
             cout<<"beq instruction"<<endl;
             cout<<"rs:"<<myRF.ReadData1.to_ulong()<<endl;
             cout<<"rt:"<<myRF.ReadData2.to_ulong()<<endl;
            if (myRF.ReadData1.to_ulong() == myRF.ReadData2.to_ulong()) {

                cout<<"equaltester"<<endl;
                signimmediate = SImm.to_string().substr(2, 30);
                cout<<"Offset1Immediate"<<signimmediate<<endl;
                bitset<32> signExtImm(signimmediate);
                signExtImm << 2;//shift by 2
                cout<<"a"<<signExtImm.to_ulong()<<endl;
                cout<<"b"<<signExtImm.to_ulong()*4<<endl;

                bitset<32> pc(programCounter.to_ulong() + 4 + signExtImm.to_ulong()*4); //


                cout<<"PC"<<pc.to_ulong()<<endl;
                programCounter = pc;
            } else {
                programCounter = bitset<32>(programCounter.to_ulong() + 4);
            }
        }
    }


    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
