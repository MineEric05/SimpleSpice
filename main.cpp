/*
PA4 Origin v2
2021.12.19 by MineEric
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#define MAX_STAGE 15 //最大Stage數
using namespace std;

typedef struct resistor{
    string Name;
    string LeftNode;
    string RightNode;
    double Resistance;
}Resistor;

//Tool function
double getEqualResistor(short &,vector<Resistor> []);
double getTotalResistor(double &,vector<Resistor> &);
string fixString(string , short );
string getUnit(double ,char ,bool ,short);

//Major function
void getType(short &);
void getFile(ifstream &);
void loadFile(ifstream &,short &,vector<Resistor> [],double &);
void calculation(short &,vector<Resistor> [],double [],double []);
void swap_stage(short &,vector<Resistor> []);
void merge_stage(short &,vector<Resistor> []);
void outputFile(short &,vector<Resistor> [],double [],double []);

int main(){
    ifstream InputFile;
    short Type = 0, Stage = 0;
    double Voltage[MAX_STAGE+1] = {}; //存放節點電壓值，第0格為電壓源電壓
    double Current[MAX_STAGE+1] = {}; //存放節點流向下一節點的電流值，第0格為總電流
    vector<Resistor>Map[MAX_STAGE*2]; //存放電阻struct，每兩個vector存放一個Stage，偶數列為S，奇數列為P

    cout<<"*** Welcome to MySpice ***"<<endl;
    while(Type!=6){
        getType(Type); //獲取輸入

        switch(Type){
            case 1:
                getFile(InputFile);
                loadFile(InputFile,Stage,Map,Voltage[0]);
                break;
            case 2:
                calculation(Stage,Map,Voltage,Current);
                break;
            case 3:
                swap_stage(Stage,Map);
                break;
            case 4:
                merge_stage(Stage,Map);
                break;
            case 5:
                outputFile(Stage,Map,Voltage,Current);
                break;
            default:
                break;
        }
    }
    cout<<"Bye~"<<endl;
    return 0;
}
//求等效電阻
double getEqualResistor(short &Stage,vector<Resistor> Map[]){

    double Resistance = 0;
    double Resistance_last = 0;

    getTotalResistor(Resistance_last,Map[2*Stage-1]);
    getTotalResistor(Resistance_last,Map[2*Stage-2]);

    for(short i = 2*(Stage-1)-1; i>=0; i-=2){
        getTotalResistor(Resistance,Map[i]);
        Resistance = (Resistance_last*Resistance)/(Resistance_last+Resistance);
        getTotalResistor(Resistance,Map[i-1]);
        Resistance_last = Resistance;
        Resistance = 0;
    }
    return Resistance_last;
}
//求總電阻
double getTotalResistor(double &Resistance,vector<Resistor> &Map){

    for(short i = 0; i<static_cast<short>(Map.size()); i++ ){
        Resistance += Map.at(i).Resistance;
    }
    return Resistance;
}
//導入字串及需要的長度，自動補齊空格
string fixString(string Str, short Length){

    while(Length>static_cast<short>(Str.size())){
        Str += ' ';
    }
    return Str;
}
//導入數字、單位、前綴k是否大寫、字串需要的長度，導出處理後的字串
string getUnit(double Input,char Unit,bool isK,short Length){

    short Prefix = 0;
    string Output;
    //確保數字在範圍內
    while(Input<1 || Input>999.999){
        if(Prefix<-2 || Prefix>3) break;
        else if(Input>999.999){
            Prefix++;
            Input /= 1000;
        }
        else if(Input<1){
            Prefix--;
            Input *= 1000;
        }
    }
    //數字轉成字串並清除多餘小數位數
    Output = to_string(round(Input*1000)/1000);
    Output = Output.substr(0,Output.length()-3);
    //加上單位前綴
    switch(Prefix){
        case -2:
            Output += "u";
            break;
        case -1:
            Output += "m";
            break;
        case 0:
            break;
        case 1:
            if(isK) Output += "K";
            else Output += "k";

            break;
        case 2:
            Output += "Meg";
            break;
        case 3:
            Output += "G";
            break;
        default:
            break;
    }
    Output += Unit; //加上單位
    Output = fixString(Output,Length); //補齊長度
    return Output;
}
//功能選單
void getType(short &Type){

    cout<<"=========================================="<<endl;
    cout<<"Please type 1 to input file"<<endl;
    cout<<"Please type 2 to calculate voltage and current"<<endl;
    cout<<"Please type 3 to swap stage"<<endl;
    cout<<"Please type 4 to merge stage"<<endl;
    cout<<"Please type 5 to output file"<<endl;
    cout<<"Please type 6 to exit"<<endl;
    cout<<"Enter your selection Here:";

    while(1){
        cin>>Type;
        if( cin.fail() || Type>6 || Type<0 ){
            cin.clear();
            cin.sync();
            cout<<"Wrong input,please enter again: ";
            continue;
        }
        else{
            cout<<"=========================================="<<endl;
            break;
        }
    }
    return;
}
//獲取檔案位置
void getFile(ifstream &InputFile){

    string InputFile_Name;

    cout<<"Please enter the name of the input file: ";
    cin>>InputFile_Name;
    InputFile.open(InputFile_Name,ios::in);

    while(!InputFile){
        cout<<"The file is not found."<<endl;
        cout<<"Please enter the name of the input file again: ";
        cin>>InputFile_Name;
        InputFile.open(InputFile_Name,ios::in);
    }

    return;
}
//讀取檔案
void loadFile(ifstream &InputFile,short &Stage,vector<Resistor> Map[],double &Voltage){

    short MapIndex = 0;
    string Name,LeftNode,RightNode,LoadStr;
    Resistor TEMP;
    cout<<"Loading the file..."<<endl;

    InputFile>>LoadStr>>Stage; //接收Stage數
    InputFile>>LoadStr>>LoadStr>>LoadStr>>LoadStr;
    Voltage = static_cast<double>(stoi(LoadStr.substr(0,LoadStr.length()-1))); //接收電壓源電壓
    while(InputFile>>Name>>LeftNode>>RightNode>>LoadStr){

        if(Name=="end") break; //遇到末尾即跳出迴圈
        TEMP.Name = Name;
        TEMP.LeftNode = LeftNode;
        TEMP.RightNode = RightNode;
        TEMP.Resistance = static_cast<double>(stoi(LoadStr.substr(0,LoadStr.length()-1)));

        //檢測電阻單位前綴並換算
        switch(LoadStr.at(LoadStr.length()-1)){
            case 'u':
                TEMP.Resistance *= 0.000001;
                break;
            case 'm':
                TEMP.Resistance *= 0.001;
                break;
            case 'K':
                TEMP.Resistance *= 1000;
                break;
            case 'k':
                TEMP.Resistance *= 1000;
                break;
            case 'g':
                TEMP.Resistance *= 1000000;
                break;
            case 'G':
                TEMP.Resistance *= 1000000000;
                break;
        }

        if(Name.at(1)=='S'){ //S存入偶數列陣列
           Map[MapIndex].push_back(TEMP);
        }
        else if(Name.at(1)=='P'){ //P存入奇數列陣列
            Map[MapIndex+1].push_back(TEMP);
            if(RightNode=="gnd") MapIndex+=2; //遇到連結gnd的電阻則此Stage結束輸入，換下一個Stage
        }
    }
    cout<<"The input file successfully loaded!"<<endl;
    return;
}
//計算
void calculation(short &Stage,vector<Resistor> Map[],double Voltage[],double Current[]){

    double Resistance = 0;
    //沒有電路跳出
    if(Stage==0){
        cout<<"There is no circuit"<<endl;
        return;
    }
    cout<<"calculating... "<<endl;
    Current[0] = Voltage[0]/getEqualResistor(Stage,Map); //計算總電流
    //計算
    for(short i = 0; i<2*Stage-1; i+=2){
        Resistance = 0;
        getTotalResistor(Resistance,Map[i]); //計算S總電阻
        Voltage[i/2+1] = Voltage[i/2] - Current[i/2]*Resistance; //降壓求下一節點電壓
        Resistance = 0;
        getTotalResistor(Resistance,Map[i+1]); //計算P總電阻
        Current[i/2+1] = Current[i/2] - Voltage[i/2+1]/Resistance; //扣掉分流去P的電阻，求下一節點電流
    }

    cout<<"The result is successfully calculated!"<<endl;
    cout<<"*********************************"<<endl;
    cout<<"**         <<voltage>>         **"<<endl;
    cout<<"**  "<<fixString("vdd",10)<<getUnit(Voltage[0],'v',1,17)<<"**"<<endl;
    for(short i = 1; i<=Stage; i++){
        cout<<"**  "<<fixString(Map[2*i-2].at(Map[2*i-2].size()-1).RightNode,10)<<getUnit(Voltage[i],'v',1,17)<<"**"<<endl;
    }
    cout<<"**         <<current>>         **"<<endl;
    for(short i = 0; i<2*Stage-1; i+=2){

        for(short j = 0; j<static_cast<short>(Map[i].size()); j++){
            cout<<"**  "<<fixString(Map[i].at(j).Name,10)<<getUnit(Current[i/2],'A',1,17)<<"**"<<endl;
        }
        for(short j = 0; j<static_cast<short>(Map[i+1].size()); j++){
            cout<<"**  "<<fixString(Map[i+1].at(j).Name,10)<<getUnit(Current[i/2]-Current[i/2+1],'A',1,17)<<"**"<<endl;
        }
    }
    cout<<"**                             **"<<endl;
    cout<<"**            myspice2021      **"<<endl;
    cout<<"*********************************"<<endl;
    return;
}
//交換Stage
void swap_stage(short &Stage,vector<Resistor> Map[]){

    short Input1 = 0,Input2 = 0;
    //沒有電路跳出
    if(Stage==0){
        cout<<"There is no circuit"<<endl;
        return;
    }
    while(1){
        cout<<"Please enter the first stage you want to swap: ";
        cin>>Input1;
        if(Input1<=Stage&&Input1>=0) break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }
    while(1){
        cout<<"Please enter the second stage you want to swap: ";
        cin>>Input2;
        if(Input2<=Stage&&Input2>=0) break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }
    //分別交換兩個Stage的P跟S
    swap(Map[2*(Input1-1)],Map[2*(Input2-1)]);
    swap(Map[2*Input1-1],Map[2*Input2-1]);

    cout<<Map[2*(Input2-1)].at(0).Name<<" is changing node from "<<Map[2*(Input2-1)].at(0).LeftNode<<" to "<<Map[2*(Input2-1)-1].at(0).LeftNode<<" ..."<<endl;
    Map[2*(Input2-1)].at(0).LeftNode = Map[2*(Input2-1)-1].at(0).LeftNode;
    if(Input1==1){ //遇到交換的Stage其中之一是第一個Stage，左節點針對vdd做處理
        cout<<Map[2*(Input1-1)].at(0).Name<<" is changing node from "<<Map[2*(Input1-1)].at(0).LeftNode<<" to "<<"vdd"<<" ..."<<endl;
        Map[2*(Input1-1)].at(0).LeftNode = "vdd";
    }
    else{
        cout<<Map[2*(Input1-1)].at(0).Name<<" is changing node from "<<Map[2*(Input1-1)].at(0).LeftNode<<" to "<<Map[2*(Input1-1)-1].at(0).LeftNode<<" ..."<<endl;
        Map[2*(Input1-1)].at(0).LeftNode =  Map[2*(Input1-1)-1].at(0).LeftNode;
    }
    if(Input2<Stage){ //如果交換後，後續有Stage，後續Stage的Node也要跟著更新
        cout<<Map[2*(Input2)].at(0).Name<<" is changing node from "<<Map[2*(Input2)].at(0).LeftNode<<" to "<<Map[2*(Input2)-1].at(0).LeftNode<<" ..."<<endl;
        Map[2*(Input2)].at(0).LeftNode = Map[2*(Input2)-1].at(0).LeftNode;
    }
    cout<<"The stage is successfully swapped!"<<endl;
    return;
}
//合併Stage
void merge_stage(short &Stage,vector<Resistor> Map[]){

    short Input1 = 0,Input2 = 0;
    //沒有電路跳出
    if(Stage==0){
        cout<<"There is no circuit"<<endl;
        return;
    }
    while(1){
        cout<<"Please enter the first stage you want to merge: ";
        cin>>Input1;
        if(Input1<=Stage&&Input1>=0) break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }
    while(1){
        cout<<"Please enter the second stage you want to merge: ";
        cin>>Input2;
        if(Input2 <= Stage&&Input2 >= 0) break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }

    double Resistance_1 = 0,Resistance_2 = 0;
    getTotalResistor(Resistance_1,Map[2*Input2-1]);
    getTotalResistor(Resistance_1,Map[2*Input2-2]);
    getTotalResistor(Resistance_2,Map[2*Input1-1]);
    Resistance_1 = (Resistance_1*Resistance_2)/(Resistance_1+Resistance_2);
    //合併電阻至P,並清除後一個Stage
    cout<<"Resistors are merged into RP"<<to_string(Input1)<<"."<<endl;
    Resistor TEMP;
    TEMP.Name = "RP"+to_string(Input1)+"  ";
    TEMP.LeftNode = Map[2*Input1-1].at(0).LeftNode;
    TEMP.RightNode = "gnd";
    TEMP.Resistance = Resistance_1;
    Map[2*Input1-1].clear();
    Map[2*Input1-1].push_back(TEMP);

    string Output = getUnit(Resistance_1,' ',0,17);
    for(short i = 0; i<static_cast<short>(Output.size()); i++){
        if(Output.at(i)==' '){
            Output.at(i) = Output.at(i-1);
            Output.at(i-1) = ' ';
            break;
        }
    }
    cout<<"=> RP"<<to_string(Input1)<<" "<<TEMP.LeftNode<<" "<<TEMP.RightNode<<" "<<Output<<endl;
    //將後面的Stage往前移
    Stage--;
    for(short i = 2*Input1; i<2*Stage-3; i++){
        Map[i] = Map[i+2];
        Map[i+2].clear();
    }
    if(Input2<Stage){
        Map[2*Input1].at(0).LeftNode = Map[2*Input1-1].at(0).LeftNode;
    }
    cout<<"The stage is Successfully merged!"<<endl;
    return;
}
//輸出檔案
void outputFile(short &Stage,vector<Resistor> Map[],double Voltage[],double Current[]){
    //沒有電路跳出
    if(Stage==0){
        cout<<"There is no circuit"<<endl;
        return;
    }
    ofstream OutputFile;
    string OutputFile_Name;

    cout<<"Please enter the name of the output file:";
    cin>>OutputFile_Name;
    OutputFile.open(OutputFile_Name,ios::out);

    cout<<"Exporting the file... "<<endl;
    OutputFile<<"***circuits***"<<endl;
    OutputFile<<"stage "<<Stage<<endl<<endl;
    OutputFile<<"v1 vdd gnd "<<int(Voltage[0])<<"V"<<endl<<endl;
    for(short i = 0; i<2*Stage-1; i+=2){
        for(short j = 0; j<static_cast<short>(Map[i].size()); j++){
            OutputFile<<fixString(Map[i].at(j).Name,8)<<fixString(Map[i].at(j).LeftNode,8)<<fixString(Map[i].at(j).RightNode,8)<<getUnit(Map[i].at(j).Resistance,' ',1,6)<<endl;
        }
        for(short j = 0; j<static_cast<short>(Map[i+1].size()); j++){
            OutputFile<<fixString(Map[i+1].at(j).Name,8)<<fixString(Map[i+1].at(j).LeftNode,8)<<fixString(Map[i+1].at(j).RightNode,8)<<getUnit(Map[i+1].at(j).Resistance,' ',1,6)<<endl;
        }
    }
    OutputFile<<endl<<"***calculation results***"<<endl;
    OutputFile<<"**************************************"<<endl;
    OutputFile<<"**			<<voltage>>				**"<<endl;
    OutputFile<<"**  "<<fixString("vdd",12)<<getUnit(Voltage[0],'v',1,20)<<"**"<<endl;
    for(int i = 1; i<=Stage; i++){
        OutputFile<<"**  "<<fixString(Map[2*i-2].at(Map[2*i-2].size()-1).RightNode,12)<<getUnit(Voltage[i],'v',1,20)<<"**"<<endl;
    }
    OutputFile<<"**			<<current>>				**"<<endl;
    for(short i = 0; i<2*Stage-1; i+=2){

        for(short j = 0; j<static_cast<short>(Map[i].size()); j++){
            OutputFile<<"**  "<<fixString(Map[i].at(j).Name,12)<<getUnit(Current[i/2],'A',1,20)<<"**"<<endl;
        }
        for(short j = 0; j<static_cast<short>(Map[i+1].size()); j++){
            OutputFile<<"**  "<<fixString(Map[i+1].at(j).Name,12)<<getUnit(Current[i/2]-Current[i/2+1],'A',1,20)<<"**"<<endl;
        }
    }
    OutputFile<<"**									**"<<endl;
    OutputFile<<"**					myspice2021		**"<<endl;
    OutputFile<<"**************************************"<<endl;
    cout<<"The output file successfully exported!"<<endl;
    return;
}
