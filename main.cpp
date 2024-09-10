/*
PA4 Origin v2
2021.12.19 by MineEric
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#define MAX_STAGE 15 //�̤jStage��
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
    double Voltage[MAX_STAGE+1] = {}; //�s��`�I�q���ȡA��0�欰�q�����q��
    double Current[MAX_STAGE+1] = {}; //�s��`�I�y�V�U�@�`�I���q�y�ȡA��0�欰�`�q�y
    vector<Resistor>Map[MAX_STAGE*2]; //�s��q��struct�A�C���vector�s��@��Stage�A���ƦC��S�A�_�ƦC��P

    cout<<"*** Welcome to MySpice ***"<<endl;
    while(Type!=6){
        getType(Type); //�����J

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
//�D���Ĺq��
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
//�D�`�q��
double getTotalResistor(double &Resistance,vector<Resistor> &Map){

    for(short i = 0; i<static_cast<short>(Map.size()); i++ ){
        Resistance += Map.at(i).Resistance;
    }
    return Resistance;
}
//�ɤJ�r��λݭn�����סA�۰ʸɻ��Ů�
string fixString(string Str, short Length){

    while(Length>static_cast<short>(Str.size())){
        Str += ' ';
    }
    return Str;
}
//�ɤJ�Ʀr�B���B�e��k�O�_�j�g�B�r��ݭn�����סA�ɥX�B�z�᪺�r��
string getUnit(double Input,char Unit,bool isK,short Length){

    short Prefix = 0;
    string Output;
    //�T�O�Ʀr�b�d��
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
    //�Ʀr�ন�r��òM���h�l�p�Ʀ��
    Output = to_string(round(Input*1000)/1000);
    Output = Output.substr(0,Output.length()-3);
    //�[�W���e��
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
    Output += Unit; //�[�W���
    Output = fixString(Output,Length); //�ɻ�����
    return Output;
}
//�\����
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
//����ɮצ�m
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
//Ū���ɮ�
void loadFile(ifstream &InputFile,short &Stage,vector<Resistor> Map[],double &Voltage){

    short MapIndex = 0;
    string Name,LeftNode,RightNode,LoadStr;
    Resistor TEMP;
    cout<<"Loading the file..."<<endl;

    InputFile>>LoadStr>>Stage; //����Stage��
    InputFile>>LoadStr>>LoadStr>>LoadStr>>LoadStr;
    Voltage = static_cast<double>(stoi(LoadStr.substr(0,LoadStr.length()-1))); //�����q�����q��
    while(InputFile>>Name>>LeftNode>>RightNode>>LoadStr){

        if(Name=="end") break; //�J�쥽���Y���X�j��
        TEMP.Name = Name;
        TEMP.LeftNode = LeftNode;
        TEMP.RightNode = RightNode;
        TEMP.Resistance = static_cast<double>(stoi(LoadStr.substr(0,LoadStr.length()-1)));

        //�˴��q�����e��ô���
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

        if(Name.at(1)=='S'){ //S�s�J���ƦC�}�C
           Map[MapIndex].push_back(TEMP);
        }
        else if(Name.at(1)=='P'){ //P�s�J�_�ƦC�}�C
            Map[MapIndex+1].push_back(TEMP);
            if(RightNode=="gnd") MapIndex+=2; //�J��s��gnd���q���h��Stage������J�A���U�@��Stage
        }
    }
    cout<<"The input file successfully loaded!"<<endl;
    return;
}
//�p��
void calculation(short &Stage,vector<Resistor> Map[],double Voltage[],double Current[]){

    double Resistance = 0;
    //�S���q�����X
    if(Stage==0){
        cout<<"There is no circuit"<<endl;
        return;
    }
    cout<<"calculating... "<<endl;
    Current[0] = Voltage[0]/getEqualResistor(Stage,Map); //�p���`�q�y
    //�p��
    for(short i = 0; i<2*Stage-1; i+=2){
        Resistance = 0;
        getTotalResistor(Resistance,Map[i]); //�p��S�`�q��
        Voltage[i/2+1] = Voltage[i/2] - Current[i/2]*Resistance; //�����D�U�@�`�I�q��
        Resistance = 0;
        getTotalResistor(Resistance,Map[i+1]); //�p��P�`�q��
        Current[i/2+1] = Current[i/2] - Voltage[i/2+1]/Resistance; //�������y�hP���q���A�D�U�@�`�I�q�y
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
//�洫Stage
void swap_stage(short &Stage,vector<Resistor> Map[]){

    short Input1 = 0,Input2 = 0;
    //�S���q�����X
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
    //���O�洫���Stage��P��S
    swap(Map[2*(Input1-1)],Map[2*(Input2-1)]);
    swap(Map[2*Input1-1],Map[2*Input2-1]);

    cout<<Map[2*(Input2-1)].at(0).Name<<" is changing node from "<<Map[2*(Input2-1)].at(0).LeftNode<<" to "<<Map[2*(Input2-1)-1].at(0).LeftNode<<" ..."<<endl;
    Map[2*(Input2-1)].at(0).LeftNode = Map[2*(Input2-1)-1].at(0).LeftNode;
    if(Input1==1){ //�J��洫��Stage�䤤���@�O�Ĥ@��Stage�A���`�I�w��vdd���B�z
        cout<<Map[2*(Input1-1)].at(0).Name<<" is changing node from "<<Map[2*(Input1-1)].at(0).LeftNode<<" to "<<"vdd"<<" ..."<<endl;
        Map[2*(Input1-1)].at(0).LeftNode = "vdd";
    }
    else{
        cout<<Map[2*(Input1-1)].at(0).Name<<" is changing node from "<<Map[2*(Input1-1)].at(0).LeftNode<<" to "<<Map[2*(Input1-1)-1].at(0).LeftNode<<" ..."<<endl;
        Map[2*(Input1-1)].at(0).LeftNode =  Map[2*(Input1-1)-1].at(0).LeftNode;
    }
    if(Input2<Stage){ //�p�G�洫��A����Stage�A����Stage��Node�]�n��ۧ�s
        cout<<Map[2*(Input2)].at(0).Name<<" is changing node from "<<Map[2*(Input2)].at(0).LeftNode<<" to "<<Map[2*(Input2)-1].at(0).LeftNode<<" ..."<<endl;
        Map[2*(Input2)].at(0).LeftNode = Map[2*(Input2)-1].at(0).LeftNode;
    }
    cout<<"The stage is successfully swapped!"<<endl;
    return;
}
//�X��Stage
void merge_stage(short &Stage,vector<Resistor> Map[]){

    short Input1 = 0,Input2 = 0;
    //�S���q�����X
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
    //�X�ֹq����P,�òM����@��Stage
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
    //�N�᭱��Stage���e��
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
//��X�ɮ�
void outputFile(short &Stage,vector<Resistor> Map[],double Voltage[],double Current[]){
    //�S���q�����X
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
