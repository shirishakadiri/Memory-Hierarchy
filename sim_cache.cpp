#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <iterator>
using namespace std;

int blockSize, l1Size, l1Associativity, l2Size, l2Associativity, replacementPolicy, inclusionProperty;
int l1reads, l1readMisses, l1writes, l1writeMisses, l1WriteBack, l2Reads, l2ReadMisses, l2Write, l2WriteMisses, l2WriteBack, memoryTraffic;
float l1MissRate, l2MissRate;

vector<vector<int> > l1CacheData;
vector<vector<int> > l1CacheDirtyBit;
vector<vector<string> > modifiedl2Address;

vector<vector<int> > l2CacheData;
vector<vector<int> > l2CacheDirtyBit;
vector<vector<string> > l2Modifiedl2Address;

string HexToBin(string hexdec)
{
    long int i = 0;
    string binaryNumber = "";
    
    while (hexdec[i]) {
        
        switch (hexdec[i]) {
            case '0':
                binaryNumber.append("0000");
                break;
            case '1':
                binaryNumber.append("0001");
                break;
            case '2':
                binaryNumber.append("0010");
                break;
            case '3':
                binaryNumber.append("0011");
                break;
            case '4':
                binaryNumber.append("0100");
                break;
            case '5':
                binaryNumber.append("0101");
                break;
            case '6':
                binaryNumber.append("0110");
                break;
            case '7':
                binaryNumber.append("0111");
                break;
            case '8':
                binaryNumber.append("1000");
                break;
            case '9':
                binaryNumber.append("1001");
                break;
            case 'A':
            case 'a':
                binaryNumber.append("1010");
                break;
            case 'B':
            case 'b':
                binaryNumber.append("1011");
                break;
            case 'C':
            case 'c':
                binaryNumber.append("1100");
                break;
            case 'D':
            case 'd':
                binaryNumber.append("1101");
                break;
            case 'E':
            case 'e':
                binaryNumber.append("1110");
                break;
            case 'F':
            case 'f':
                binaryNumber.append("1111");
                break;
            default:
                cout << "\nInvalid hexadecimal digit "<< hexdec[i];
        }
        i++;
    }
    //cout << binaryNumber << endl;
    return binaryNumber;
}

string decimalToHex(int val){
    char hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int r;
    string hexdec="";
    while(val>0)
    {
        r = val % 16;
        hexdec = hex[r] + hexdec;
        val = val/16;
    }
    return hexdec;
}

int binaryStringToDecimal(string n)
{
    string num = n;
    int dec_value = 0, base = 1, len = num.length();
    for (int i=len-1;i>=0;i--)
    {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
    return dec_value;
}

int decToBinary(int n)
{
    int binaryNumber = 0;
    int remainder, i = 1, step = 1;
    if(n == 3){
        return 11;
    }
    else{
        while (n!=0)
        {
            remainder = n%2;
            
            n /= 2;
            binaryNumber += remainder*i;
            i *= 10;
        }
        return binaryNumber;
    }
}

//inclusion property
//evict the bloack from l1 if evicted from l2
void inclusionEviction(int l2Index, int l2Col, int l1Index){
    //l1WriteBack = l1WriteBack + 1;
    int l2Add = binaryStringToDecimal(l2Modifiedl2Address[l2Index][l2Col]);
    
    cout << setbase(16);
    //cout <<"inclusion :" << l2Add <<endl;
    
    vector<int> tempData;
    tempData = l1CacheData[l1Index];
    
    vector<int> tempDirtyBit;
    tempDirtyBit = l1CacheDirtyBit[l1Index];
    
    vector<string> templ2Address;
    templ2Address = modifiedl2Address[l1Index];
    
    string l2addCompare = l2Modifiedl2Address[l2Index][l2Col];
    
    for(int i = 0; i < l1Associativity ; i++){
        //cout << binaryStringToDecimal(modifiedl2Address[l1Index][i]) <<endl;
        if (l2addCompare == modifiedl2Address[l1Index][i]){
            tempData.erase(tempData.begin()+i);
            l1CacheData[l1Index] = tempData;
            
            tempDirtyBit.erase(tempDirtyBit.begin()+i);
            l1CacheDirtyBit[l1Index] = tempDirtyBit;
            
            templ2Address.erase(templ2Address.begin()+i);
            modifiedl2Address[l1Index] = templ2Address;
            
        }
    }
}

//update LRU of L2 of evicted block
//update corresponding dirty bit
void evitionFromL1ToL2(int index, int l1Tag, int dirtyUpdate){
    l2Write = l2Write + 1;
    
    //calculate l2 address
    cout <<setbase(16);
    //cout << l1Tag <<endl;
    int indexForTrack;
    for(int i = 0 ; i < l1Associativity ; i++ ){
        if(l1CacheData[index][i] == l1Tag){
            indexForTrack = i;
        }
    }
    
    //cout << modifiedl2Address[index][indexForTrack] <<endl;
    string binaryString = modifiedl2Address[index][indexForTrack];
    cout << setbase(10);
    
    int noOfSetsL2 = l2Size / (blockSize * l2Associativity);
    int noOfBlocksL2 = l2Associativity;
    int noOfOffsetBits = log2(blockSize);
    int noOfIndexBits = log2(noOfSetsL2);
    //cout << noOfIndexBits<<endl;
    
    string indexString = binaryString.substr(binaryString.length() - (noOfOffsetBits + noOfIndexBits), noOfIndexBits);
    int indexNumberL2 = binaryStringToDecimal(indexString);
    //cout << indexNumberL2 << endl;
    
    string tagString = binaryString.substr(0, binaryString.length() - (noOfOffsetBits + noOfIndexBits));
    int tagDecimalL1 = binaryStringToDecimal(tagString);
    cout << setbase(16);
    //cout <<"L2 write : ( tag " <<tagDecimalL1 << ", index ";
    cout <<setbase(10);
    //cout <<indexNumberL2 << ")" <<endl;
    
    vector<int> tempData;
    tempData = l2CacheData[indexNumberL2];
    
    vector<int> tempDirtyBit;
    tempDirtyBit = l2CacheDirtyBit[indexNumberL2];
    int tempDirty;
    tempDirty = l1CacheDirtyBit[index][indexForTrack];
    
    vector<string> tempL2Address;
    tempL2Address = l2Modifiedl2Address[indexNumberL2];
    string tempAdd = "";
    
    int countCheck = 0;
    for(int i = 0;i < noOfBlocksL2; i ++){
        if(l2CacheData[indexNumberL2][i] == tagDecimalL1){
            //cout <<"L2 hit" <<endl;
            //cout << "L2 update LRU" << endl;
            if(replacementPolicy == 0){
                tempData.erase(tempData.begin()+i);
                tempData.insert(tempData.begin(),tagDecimalL1);
                l2CacheData[indexNumberL2] = tempData;
                
                //cout << "L2 set dirty" <<  endl;
                tempDirtyBit.erase(tempDirtyBit.begin()+i);
                tempDirtyBit.insert(tempDirtyBit.begin(), dirtyUpdate);
                l2CacheDirtyBit[indexNumberL2] = tempDirtyBit;
                //cout << "dirty : " << l2CacheDirtyBit[indexNumberL2][0] <<endl;
                
                tempAdd = l2Modifiedl2Address[indexNumberL2][i];
                tempL2Address.erase(tempL2Address.begin()+i);
                tempL2Address.insert(tempL2Address.begin(), tempAdd);
                l2Modifiedl2Address[indexNumberL2] = tempL2Address;
            }
            else if(replacementPolicy == 1){
                l2CacheDirtyBit[indexNumberL2][i] = dirtyUpdate;
            }
            break;
        }
        else{
            countCheck = countCheck + 1;
        }
    }
    
    if(countCheck == noOfBlocksL2){
        l2WriteMisses = l2WriteMisses + 1;
        //cout << "L2 miss"<< endl;
        if(l2CacheData[indexNumberL2][noOfBlocksL2 - 1] == 0){
            //cout << "L2 victim : none" <<endl;
        }
        else{
            cout << setbase(16);
            //cout << "L2 victim eviction: "<< l2CacheData[indexNumberL2][noOfBlocksL2 - 1] <<endl;
            if(l2CacheDirtyBit[indexNumberL2][noOfBlocksL2 - 1] == 1){
                //cout << " , dirty" << endl;
                l2WriteBack = l2WriteBack + 1;
                
            }
            if(inclusionProperty == 1){
                inclusionEviction(indexNumberL2,noOfBlocksL2 - 1, index);
            }
            
            else{
                //cout << " , clean" << endl;
            }
            
            //l2WriteBack = l2WriteBack + 1;
        }
        //[{
        if((replacementPolicy == 0) || (replacementPolicy == 1)){
            tempData.pop_back();
            tempData.insert(tempData.begin(), tagDecimalL1);
            l2CacheData[indexNumberL2] = tempData;
            //cout << "L2 update LRU"<<endl;
            //l2CacheDirtyBit[indexNumberL2][0] = tempDirty;
            
            tempDirtyBit.pop_back();
            tempDirtyBit.insert(tempDirtyBit.begin(), dirtyUpdate);
            l2CacheDirtyBit[indexNumberL2] = tempDirtyBit;
            
            tempL2Address.pop_back();
            tempL2Address.insert(tempL2Address.begin(), binaryString);
            l2Modifiedl2Address[indexNumberL2] = tempL2Address;
        }
    }
    
}

void writeToL2(string binaryString, int l1Index, int dirtyUpdate){
    l2Reads = l2Reads + 1;
    cout << setbase(10);
    
    int noOfSetsL2 = l2Size / (blockSize * l2Associativity);
    int noOfBlocksL2 = l2Associativity;
    int noOfOffsetBits = log2(blockSize);
    int noOfIndexBits = log2(noOfSetsL2);
    //cout << noOfIndexBits<<endl;
    
    string indexString = binaryString.substr(binaryString.length() - (noOfOffsetBits + noOfIndexBits), noOfIndexBits);
    int indexNumberL2 = binaryStringToDecimal(indexString);
    //cout << indexNumberL2 << endl;
    
    string tagString = binaryString.substr(0, binaryString.length() - (noOfOffsetBits + noOfIndexBits));
    int tagDecimalL1 = binaryStringToDecimal(tagString);
    cout << setbase(16);
    //cout <<"L2 read : ( tag " <<tagDecimalL1 << ", index ";
    cout <<setbase(10);
    //cout <<indexNumberL2 << ")" <<endl;
    
    vector<int> tempData;
    tempData = l2CacheData[indexNumberL2];
    
    vector<int> tempDirtyBit;
    tempDirtyBit = l2CacheDirtyBit[indexNumberL2];
    int tempDirty;
    
    vector<string> tempL2Address;
    tempL2Address = l2Modifiedl2Address[indexNumberL2];
    string tempAdd = "";
    
    int countCheck = 0;
    for(int i = 0;i < noOfBlocksL2; i ++){
        if(l2CacheData[indexNumberL2][i] == tagDecimalL1){
            //cout <<"L2 hit" <<endl;
            //cout << "L2 update LRU" << endl;
            if(replacementPolicy == 0){
                tempData.erase(tempData.begin()+i);
                tempData.insert(tempData.begin(),tagDecimalL1);
                l2CacheData[indexNumberL2] = tempData;
                
                //cout << "L2 set dirty" <<  endl;
                tempDirty = l2CacheDirtyBit[indexNumberL2][i];
                tempDirtyBit.erase(tempDirtyBit.begin()+i);
                tempDirtyBit.insert(tempDirtyBit.begin(), tempDirty);
                l2CacheDirtyBit[indexNumberL2] = tempDirtyBit;
                
                tempAdd = l2Modifiedl2Address[indexNumberL2][i];
                tempL2Address.erase(tempL2Address.begin()+i);
                tempL2Address.insert(tempL2Address.begin(), tempAdd);
                l2Modifiedl2Address[indexNumberL2] = tempL2Address;
            }/*
            else if(replacementPolicy == 1){
                l2CacheDirtyBit[indexNumberL2][i] = tempDirty;
            }*/
            break;
        }
        else{
            countCheck = countCheck + 1;
        }
    }
    //exclusive
    if(inclusionProperty == 2){
        
    }
    //inclusive and non-inclusive
    if((inclusionProperty == 0) || inclusionProperty == 1){
        if(countCheck == noOfBlocksL2){
            l2ReadMisses = l2ReadMisses + 1;
            //cout << "L2 miss"<< endl;
            if(l2CacheData[indexNumberL2][noOfBlocksL2 - 1] == 0){
                //cout << "L2 victim : none" <<endl;
            }
            else{
                cout <<setbase(16);
                //cout << "L2 victim : "<< l2CacheData[indexNumberL2][noOfBlocksL2 - 1] ;
                if(l2CacheDirtyBit[indexNumberL2][noOfBlocksL2 - 1] == 1){
                    //cout << " , dirty" << endl;
                    l2WriteBack = l2WriteBack + 1;
                }
                
                if(inclusionProperty == 1){
                    inclusionEviction(indexNumberL2,noOfBlocksL2 - 1, l1Index);
                }
              
                else{
                    //cout << " , clean" << endl;
                }
            }
            //
            if((replacementPolicy == 0) || (replacementPolicy == 1)){
                tempData.pop_back();
                tempData.insert(tempData.begin(), tagDecimalL1);
                l2CacheData[indexNumberL2] = tempData;
                //cout << "L2 update LRU"<<endl;
                
                tempDirtyBit.pop_back();
                tempDirtyBit.insert(tempDirtyBit.begin(), 0);
                l2CacheDirtyBit[indexNumberL2] = tempDirtyBit;
                
                tempL2Address.pop_back();
                tempL2Address.insert(tempL2Address.begin(), binaryString);
                l2Modifiedl2Address[indexNumberL2] = tempL2Address;
            }
            
        }
    }
}


void readFromL1(int tag, int index, string l2AddressDecimal){
    
    int checkCount = 0;
    vector<int> tempData;
    tempData = l1CacheData[index];
    l1reads = l1reads + 1;
    vector<int> tempDirtyBit;
    tempDirtyBit = l1CacheDirtyBit[index];
    
    vector<string> templ2Address;
    templ2Address = modifiedl2Address[index];
    
    int tempDirty;
    for(int i = 0 ; i < l1Associativity ; i++){
        if(l1CacheData[index][i] == tag){
            //cout<< "L1 hit" << endl;
            //cout<< "L1 update LRU" << endl;
            if(replacementPolicy == 0){
                
                tempData.erase(tempData.begin()+i);
                tempData.insert(tempData.begin(),tag);
                l1CacheData[index] = tempData;
                
                tempDirty = l1CacheDirtyBit[index][i];
                tempDirtyBit.erase(tempDirtyBit.begin()+i);
                tempDirtyBit.insert(tempDirtyBit.begin(), tempDirty);
                l1CacheDirtyBit[index] = tempDirtyBit;
                
                string tempUpdateL2Address = modifiedl2Address[index][i];
                templ2Address.erase(templ2Address.begin()+i);
                templ2Address.insert(templ2Address.begin(), tempUpdateL2Address);
                modifiedl2Address[index] = templ2Address;
            }
            
            break;
        }
        else{
            checkCount++;
        }
    }
  
    //inclusive and non-inclusive
    if((inclusionProperty == 0) || inclusionProperty == 1){
        tempData = l1CacheData[index];
        tempDirtyBit = l1CacheDirtyBit[index];
        //read Miss
        if(checkCount == l1Associativity){
            l1readMisses = l1readMisses + 1;
            //cout << "L1 miss" <<endl;
            if(tempData[l1Associativity - 1] == 0){
                //cout << "L1 victim: none"<< endl;
            }
            else{
                //cout << "L1 victim: ( tag "<< tempData[l1Associativity - 1] <<endl;
                if(l1CacheDirtyBit[index][l1Associativity - 1] == 1){
                    l1WriteBack = l1WriteBack + 1;
                    if(l2Size > 0){
                        evitionFromL1ToL2(index, tempData[l1Associativity - 1], tempDirtyBit[l1Associativity - 1]);
                    }
                }
            }
            
            if(l2Size > 0){
                writeToL2(l2AddressDecimal, index, l1CacheDirtyBit[index][l1Associativity - 1]);
            }
            
            //cout << "L1 update LRU "<< endl;
            //add data
            if((replacementPolicy == 0) || (replacementPolicy == 1)){
                tempData.pop_back();
                tempData.insert(tempData.begin(),tag);
                l1CacheData[index] = tempData;
                
                tempDirtyBit.pop_back();
                tempDirtyBit.insert(tempDirtyBit.begin(),0);
                l1CacheDirtyBit[index] = tempDirtyBit;
                
                templ2Address.pop_back();
                templ2Address.insert(templ2Address.begin(), l2AddressDecimal);
                modifiedl2Address[index] = templ2Address;
            }
        }
    }
    cout<< setbase(16);
    //cout << "L1 read : ( tag " << tag << " , index ";
    cout << setbase(10);
    //cout<< index << " )" << endl;
}

void writeToL1(int tag, int index, string l2AddressDecimal){
    //cout << "write start" <<endl;
    l1writes = l1writes + 1;
    int checkCount = 0;
    vector<int> tempData;
    tempData = l1CacheData[index];
    
    vector<int> tempDirtyBit;
    tempDirtyBit = l1CacheDirtyBit[index];
    int tempDirty;
    
    vector<string> templ2Address;
    templ2Address = modifiedl2Address[index];
    
    for(int i = 0 ; i < l1Associativity ; i++){
        if(l1CacheData[index][i] == tag){
            //cout<< "L1 hit" << endl;
            //cout<< "L1 update LRU" << endl;
            if(replacementPolicy == 0){
                tempData.erase(tempData.begin()+i);
                tempData.insert(tempData.begin(),tag);
                l1CacheData[index] = tempData;
                //cout << "L1 set dirty" <<endl;
                tempDirtyBit.erase(tempDirtyBit.begin()+i);
                tempDirtyBit.insert(tempDirtyBit.begin(),1);
                l1CacheDirtyBit[index] = tempDirtyBit;
                
                string tempUpdateL2Address = modifiedl2Address[index][i];
                templ2Address.erase(templ2Address.begin()+i);
                templ2Address.insert(templ2Address.begin(), tempUpdateL2Address);
                modifiedl2Address[index] = templ2Address;
            }
            else if(replacementPolicy == 1){
                l1CacheDirtyBit[index][i] = 1;
            }
            break;
        }
        else{
            checkCount++;
        }
    }
    //cout << setbase(16);
    if(checkCount == l1Associativity){
        l1writeMisses = l1writeMisses + 1;
        cout<< setbase(16);
        //cout << "L1 write : ( tag " << tag << " , index ";
        cout << setbase(10);
        //cout<< index << " )" << endl;
        //cout << "L1 miss" <<endl;
        if(tempData[l1Associativity - 1] == 0){
            //cout << "L1 victim: none"<< endl;
        }
        else{
            //cout << "L1 victim: ( tag "<< tempData[l1Associativity - 1] <<endl;
            
            if(l1CacheDirtyBit[index][l1Associativity - 1] == 1){
                l1WriteBack = l1WriteBack + 1;
                if(l2Size > 0){
                    evitionFromL1ToL2(index, tempData[l1Associativity - 1], tempDirtyBit[l1Associativity - 1]);
                }
            }
        }
        if(l2Size > 0){
            writeToL2(l2AddressDecimal,index, l1CacheDirtyBit[index][l1Associativity - 1]);
        }
        
        //cout << "L1 update LRU "<< endl;
        
        //add data
        if((replacementPolicy == 0) || (replacementPolicy == 1)){
            tempData.pop_back();
            tempData.insert(tempData.begin(),tag);
            l1CacheData[index] = tempData;
            //add dirty bit
            //cout << "L1 set dirty"<< endl;
            
            tempDirtyBit.pop_back();
            tempDirtyBit.insert(tempDirtyBit.begin(),1);
            l1CacheDirtyBit[index] = tempDirtyBit;
            
            templ2Address.pop_back();
            templ2Address.insert(templ2Address.begin(), l2AddressDecimal);
            modifiedl2Address[index] = templ2Address;
        }
    }
    
}

void l1CacheImplementation(string traceFile){
    
    int noOfSetsL1 = l1Size / (blockSize * l1Associativity);
    int noOfBlocksL1 = l1Associativity;
    int noOfSetsL2, noOfBlocksL2;
    
    //initialize values L1
    
    for(int e = 0;e< noOfSetsL1; e++){
        vector<int> temp;
        for(int j = 0 ; j < noOfBlocksL1; j++){
            temp.push_back(0);
        }
        l1CacheData.push_back(temp);
    }
    
    for(int e = 0;e< noOfSetsL1; e++){
        vector<int> temp;
        for(int j = 0 ; j < noOfBlocksL1; j++){
            temp.push_back(0);
        }
        l1CacheDirtyBit.push_back(temp);
    }
    
    for(int e = 0;e< noOfSetsL1; e++){
        vector<string> temp;
        for(int j = 0 ; j < noOfBlocksL1; j++){
            temp.push_back("");
        }
        modifiedl2Address.push_back(temp);
    }
    
    
    if(l2Size > 0){
        
        noOfSetsL2 = l2Size / (blockSize * l2Associativity);
        noOfBlocksL2 = l2Associativity;
        
        for(int e = 0;e< noOfSetsL2; e++){
            vector<int> temp;
            for(int j = 0 ; j < noOfBlocksL2; j++){
                temp.push_back(0);
            }
            l2CacheData.push_back(temp);
        }
        
        for(int e = 0;e< noOfSetsL2; e++){
            vector<int> temp;
            for(int j = 0 ; j < noOfBlocksL2; j++){
                temp.push_back(0);
            }
            l2CacheDirtyBit.push_back(temp);
        }
        
        for(int e = 0;e< noOfSetsL2; e++){
            vector<string> temp;
            for(int j = 0 ; j < noOfBlocksL2; j++){
                temp.push_back("");
            }
            l2Modifiedl2Address.push_back(temp);
        }
        
    }
    
    //opening file
    ifstream file;
    string line;
    //cout << "----------------------------------------" << endl;
    file.open(traceFile);
    int i = 1;
    getline(file, line);
    while(!file.eof()){
        
        string hexadecimal = "";
        string binaryString = "";
        string offsetString = "";
        string indexString = "";
        string tagString = "";
        int tagNumber = 0;
        int noOfOffsetBits = 0;
        int noOfIndexBits = 0;
        int decimalNumber = 0;
        int offsetNumber = 0;
        int indexNumberL1 = 0;
        int offset = 0;
        int index = 0;
        int tag = 0;
        istringstream ss(line);
        string word;
        ss >> word;
        char hexValue[10];
        
        std::istream_iterator<std::string> beg(ss), end;
        std::vector<std::string> tokens(beg, end);
        hexadecimal = tokens[0];
        //cout << "# " << i << " : " << word << " " <<hexadecimal <<endl;
        
        binaryString = HexToBin(hexadecimal);
        
        //offset
        noOfOffsetBits = log2(blockSize);
        offsetString = binaryString.substr(noOfOffsetBits, binaryString.length() - noOfOffsetBits);
        offsetNumber = binaryStringToDecimal(offsetString);
        
        //Index
        noOfIndexBits = log2(noOfSetsL1);
        indexString = binaryString.substr(binaryString.length() - (noOfOffsetBits + noOfIndexBits), noOfIndexBits);
        indexNumberL1 = binaryStringToDecimal(indexString);
        string l2Address = binaryString.substr(0,binaryString.length() - noOfOffsetBits);
        for(int a = 0 ; a < noOfOffsetBits; a++){
            l2Address = l2Address.append("0");
        }
        //int l2AddressDecimal = binaryStringToDecimal(l2Address);
        //cout<<setbase(16);
        //cout << "L2 address: " << l2AddressDecimal <<endl;
        
        //tag
        tagString = binaryString.substr(0, binaryString.length() - (noOfOffsetBits + noOfIndexBits));
        int tagDecimalL1 = binaryStringToDecimal(tagString);
        
        if("w" == word){
            writeToL1(tagDecimalL1, indexNumberL1, l2Address);
        }
        else if(word == "r"){
            readFromL1(tagDecimalL1, indexNumberL1, l2Address);
        }
        
        //cout << "----------------------------------------" << endl;
        
        i = i+1;
        getline(file, line);
    }
    
    cout << "===== L1 contents =====" <<endl;
    for(int l = 0; l < noOfSetsL1; l++){
        cout << setbase(10);
        cout<< "Set \t "<<l<<": \t";
        for(int k = noOfBlocksL1 - 1;k >= 0; k--){
            cout << setbase(16);
            cout<< l1CacheData[l][k];
            if(l1CacheDirtyBit[l][k] == 1){
                cout << " D ";
            }
            else{
                cout << "   ";
            }
        }
        cout<< endl;
    }
    if(l2Size > 0){
        cout << "===== L2 contents =====" <<endl;
        for(int l = 0; l < noOfSetsL2; l++){
            cout << setbase(10);
            cout<< "Set \t "<<l<<": \t";
            for(int k = noOfBlocksL2 - 1;k >= 0; k--){
                cout << setbase(16);
                cout<< l2CacheData[l][k];
                if(l2CacheDirtyBit[l][k] == 1){
                    cout << " D ";
                }
                else{
                    cout << "   ";
                }
            }
            cout<< endl;
        }
    }
    cout << setbase(10);
    cout << "===== Simulation results (raw) =====" <<endl;
    cout << "a. number of L1 reads:        " << l1reads <<endl;
    cout << "b. number of L1 read misses:  " << l1readMisses <<endl;
    cout << "c. number of L1 writes:       " << l1writes <<endl;
    cout << "d. number of L1 write misses: " << l1writeMisses <<endl;
    l1MissRate = float((l1readMisses + l1writeMisses))/float((l1reads + l1writes));
    cout << "e. L1 miss rate:              " << l1MissRate <<endl;
    cout << "f. number of L1 writebacks:   " << l1WriteBack << endl;
    if(l2Size > 0){
        cout << "g. number of L2 reads:        " << l2Reads <<endl;
        cout << "h. number of L2 read misses:  " << l2ReadMisses << endl;
        cout << "i. number of L2 writes:       " << l2Write <<endl;
        cout << "j. number of L2 write misses: " << l2WriteMisses <<endl;
        l2MissRate = float(l2ReadMisses)/float(l2Reads);
        cout << "k. L2 miss rate:              " << l2MissRate <<endl;
        cout << "l. number of L2 writebacks:   " << l2WriteBack << endl;
    }
    if(l2Size > 0){
        memoryTraffic = l2ReadMisses + l2WriteMisses + l2WriteBack;
        cout << "m. total memory traffic:      " << memoryTraffic << endl;
    }
    else{
        memoryTraffic = l1readMisses + l1writeMisses + l1WriteBack;
        cout << "m. total memory traffic:      " << memoryTraffic << endl;
    }
    
    
}

int main(int argc, char *argv[]){
    blockSize = atoi(argv[1]);
    l1Size = atoi(argv[2]);
    l1Associativity = atoi(argv[3]);
    l2Size = atoi(argv[4]);
    l2Associativity = atoi(argv[5]);
    replacementPolicy = atoi(argv[6]);
    inclusionProperty = atoi(argv[7]);
    string traceFile = argv[8];
    
    printf("===== Simulator configuration =====\n");
    printf("BLOCKSIZE:\t%d\n", blockSize);
    printf("L1_SIZE:\t%d\n", l1Size);
    printf("L1_ASSOCIATIVITY:\t%d\n", l1Associativity);
    printf("L2_SIZE:\t%d\n", l2Size);
    printf("L2_ASSOCIATIVITY:\t%d\n", l2Associativity);
    if(replacementPolicy == 0){
        printf("REPLACEMENT POLICY:\tLRU\n");
    }
    else if(replacementPolicy == 1){
        printf("REPLACEMENT POLICY:\tFIFO\n");
    }
    else if(replacementPolicy == 2){
        printf("REPLACEMENT POLICY:\tOptimal\n");
    }
    if(inclusionProperty == 0){
        printf("INCLUSION PROPERTY:\tnon-inclusive\n");
    }
    else if(inclusionProperty == 1){
        printf("INCLUSION PROPERTY:\tinclusive\n");
    }
    else if(inclusionProperty == 2){
        printf("INCLUSION PROPERTY:\texclusive\n");
    }
    if(!strcmp(argv[8], "gcc_trace.txt")){
        printf("trace_file:\tgcc_trace.txt\n");
    }
    
    l1CacheImplementation(traceFile);
    
    return 0;
}
//./sim_cache 16 1024 2 0 0 0 0 gcc_trace.txt




