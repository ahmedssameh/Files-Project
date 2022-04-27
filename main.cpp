#include <iostream>
#include <set>
#include "fstream"
using namespace std;
struct Block{
    int nextblock;
    int greatestKey;
    int id;
    static int numberofblock;
    set<pair<int,int>>Records;
    Block(){nextblock=-1;greatestKey=-1;id=numberofblock;}
};
int Block::numberofblock=0;
fstream SequenceSetFile;
class sequenceSet{
    int iKey;int iVal;int size;int FirstBlockFill=-1;int FirstBlockEmpty=-1;int HeaderLength=15;int M=10;int N=5;char *Filename="cIndexFile.txt";
public:
    bool CreateRecordFile(char *cIndexFile, int m,int n){
        if(M==m&&N==n&&Filename==cIndexFile){
            SequenceSetFile.open(cIndexFile,ios::out|ios::app);
            return true;
        }
        else{cout<<"no matches";return false;}
    }
    int insertVal(char *cIndexFile,int key,int val){
        //iterate in the file blocks to find which one can take the value (has space and value less than the greatest key)
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::out|ios::app);
        if(FirstBlockFill==-1&&FirstBlockEmpty==-1){
            Block::numberofblock++;
            Block B;B.greatestKey=key;B.nextblock=-1;
            B.Records.insert(make_pair(key,val));
            FirstBlockFill=1;
            FirstBlockEmpty=1;
            WriteHeader();
            WriteBlock(B.id,B);
        }
       else if(FirstBlockFill>-1){
           ReadHeader();
           int i=1;
            while (i!=-1){
               Block read= readBlock(i);
               if(read.greatestKey>key&&read.Records.size()<N-1){
                   read.Records.insert(make_pair(key,val));
                   WriteBlock(i,read);
                   i=read.nextblock;
                   //if(read.Records.size()==N)FirstBlockEmpty=read.nextblock;
                   //else FirstBlockEmpty=read.id;
                   WriteHeader();
                   break;
               }
               else if(read.greatestKey<key&&read.Records.size()<N-1){
                   if(read.nextblock==-1)
                   {
                   read.Records.insert(make_pair(key,val));
                   read.greatestKey=key;
                   //if(read.Records.size()==N)FirstBlockEmpty=read.nextblock;
                   //else FirstBlockEmpty=read.id;
                   WriteHeader();
                       WriteBlock(i,read);i=read.nextblock;break;
                   }
                   else {i= read.nextblock; continue;}
               }
               else if(read.greatestKey<key&&read.Records.size()==N-1){
                   if(read.nextblock==-1){
                   Block::numberofblock++;
                   Block newBlock;
                   read.nextblock=Block::numberofblock;
                   WriteBlock(i,read);
                   newBlock.Records.insert(make_pair(key,val));
                   newBlock.greatestKey=key;
                   WriteHeader();
                   WriteBlock(newBlock.id,newBlock);
                       i=read.nextblock;
                   break;
                   }else{
                       i=read.nextblock;
                       continue;
                   }
               }
               else if(read.greatestKey>key&&read.Records.size()==N-1){
                   //Split
                   //read.Records.insert(make_pair(key,val));
                   Block::numberofblock++;
                   Block newBlock;
                   newBlock.nextblock=read.nextblock;
                   read.nextblock=newBlock.id;
                   auto iter=read.Records.begin();
                   for(int j=(N/2)+1;j<=N-1;j++) iter++;
                   for(auto iterr=iter;iterr!=read.Records.end();iterr++){
                       newBlock.Records.insert(make_pair(iterr->first,iterr->second));
                   }
                   read.Records.erase(iter,read.Records.end());
                   auto it=--(newBlock.Records.end());
                   newBlock.greatestKey=it->first;
                   it=--(read.Records.end());
                   read.greatestKey=it->first;
                   WriteBlock(read.id,read);
                   WriteBlock(newBlock.id,newBlock);
                   //FirstBlockEmpty=read.id;
                   WriteHeader();
               }
           }
       }
       for(int i=1;i<=Block::numberofblock;i++){
           Block temp= readBlock(i);
           if(temp.Records.size()<N-1){
               FirstBlockEmpty=i;
               break;
           }
           WriteHeader();
       }
    }
    void WriteHeader(){
        SequenceSetFile.close();
        SequenceSetFile.open("cIndexFile.txt",ios::out|ios::in|ios::ate);
        SequenceSetFile.seekp(0,ios::beg);
        SequenceSetFile<<FirstBlockFill<<'$'<<FirstBlockEmpty<<'$'<<Block::numberofblock<<'$';
        //for(int i=0;i<3;i++){SequenceSetFile<<' ';}
        //when i have blocks to decide which one is filled first and first empty block then return to code here.
    }
    void ReadHeader(){
        SequenceSetFile.close();
        SequenceSetFile.open("cIndexFile.txt",ios::in|ios::app);
        SequenceSetFile.seekg(0,ios::beg);
        string temp;
        getline(SequenceSetFile,temp,'$');
        FirstBlockFill= stoi(temp);
        getline(SequenceSetFile,temp,'$');
        FirstBlockEmpty= stoi(temp);
        getline(SequenceSetFile,temp,'$');
        Block::numberofblock= stoi(temp);
    }
    void WriteBlock(int numberOfBlock,Block written){
        SequenceSetFile.close();
        SequenceSetFile.open("CIndexFile.txt",ios::out|ios::in|ios::ate);
        SequenceSetFile.seekg(9+((numberOfBlock-1)*50),ios::beg);
        //
        SequenceSetFile<<written.nextblock<<'|'<<written.greatestKey<<'|';
        for(auto record:written.Records){
            SequenceSetFile<<record.first<<'|'<<record.second<<'|';
        }
        SequenceSetFile<<'$';
        int SpaceBegin=SequenceSetFile.tellp();
        while(SpaceBegin!=9+(50*numberOfBlock)){
            SequenceSetFile<<' ';
            SpaceBegin++;
        }

    }
    Block readBlock(int blockNumber){
        SequenceSetFile.close();
        SequenceSetFile.open("CIndexFile.txt",ios::in|ios::app);
        SequenceSetFile.seekp(9+(blockNumber*50),ios::beg);
        SequenceSetFile.seekp(-50,ios::cur);
        Block temp;string t;temp.id=blockNumber;
        getline(SequenceSetFile,t,'|');
        temp.nextblock= stoi(t);
        getline(SequenceSetFile,t,'|');
        temp.greatestKey= stoi(t);
        while (!SequenceSetFile.eof()){
            char c;SequenceSetFile>>c;
            SequenceSetFile.seekg(-1,ios::cur);
            if(c=='$')break;
            else{
                int f,s;
                getline(SequenceSetFile,t,'|');
                f= stoi(t);
                getline(SequenceSetFile,t,'|');
                s= stoi(t);
                temp.Records.insert(make_pair(f,s));
            }
        }
        return temp;
    }
    bool DeleteKey (char *cIndexFile, int iToken){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        ReadHeader();
        Block read;int f=0,d=0;;
        int i=1;
        while(i!=-1){
            if(readBlock(i).greatestKey>=iToken){
                read= readBlock(i);
                f=1;
                break;
            }
            i= readBlock(i).nextblock;
        }
        if(f==0)return false;
        else{
            //cout<<"ana"<<endl;
            Block nextEmptyBlock;
            for(pair<int,int> get:read.Records){
                if(get.first==iToken&&iToken!=read.greatestKey){
                    d=1;
                    read.Records.erase(get);
                    if(read.Records.size()<N/2){
                        int x=read.nextblock;
                       nextEmptyBlock= readBlock(x);
                       for(pair<int,int> p:read.Records){
                           nextEmptyBlock.Records.insert(make_pair(p.first,p.second));
                       }
                        auto iter=nextEmptyBlock.Records.end();
                        iter--;
                        nextEmptyBlock.greatestKey=iter->first;
                        WriteBlock(nextEmptyBlock.id,nextEmptyBlock);
                        read.greatestKey=-1;
                        read.Records.clear();
                        WriteBlock(read.id,read);
                    }
                    else{
                    WriteBlock(i,read);
                    }
                    break;
                }
                else if(get.first==iToken&&iToken==read.greatestKey){
                    d=1;
                    read.Records.erase(get);
                    if(read.Records.size()<N/2){
                        int x=read.nextblock;
                        nextEmptyBlock= readBlock(x);
                        for(pair<int,int> p:read.Records){
                            nextEmptyBlock.Records.insert(make_pair(p.first,p.second));
                        }
                        auto iter=nextEmptyBlock.Records.end();
                        iter--;
                        nextEmptyBlock.greatestKey=iter->first;
                        WriteBlock(nextEmptyBlock.id,nextEmptyBlock);
                        read.greatestKey=-1;
                        read.Records.clear();
                        WriteBlock(read.id,read);
                    }
                    else{
                    auto iter=read.Records.end();
                    iter--;
                    read.greatestKey=iter->first;
                    WriteBlock(i,read);
                    }
                    break;
                }
            }
            if(d==1) return true;
            else return false;
        }
    }
    int GetKey(char *cIndexFile, int iBlock, int iRecord){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        Block read=readBlock(iBlock);
        auto iter=read.Records.begin();
        if(read.Records.size()>=iRecord) {
            for (int i = 1; i < iRecord; i++) {
                iter++;
            }
            return iter->first;
        } else return -1;
    }
    int GetVal(char *cIndexFile, int iBlock, int iRecord){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        Block read=readBlock(iBlock);
        auto iter=read.Records.begin();
        if(read.Records.size()>=iRecord) {
            for (int i = 1; i < iRecord; i++) {
                iter++;
            }
            return iter->second;
        } else return -1;
    }
    int GetBlockIndex (char *cIndexFile, int iToken){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        int i=1,flag=0;
        while(i!=-1){
            if(readBlock(i).greatestKey>=iToken){
                flag=1;
                break;
            }
            i= readBlock(i).nextblock;
        }
        if(flag==1){return i;}
        else{return -1;}
    }
    int GetRecordIndex (char *cIndexFile, int iToken){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        int i=1,flag=0;
        Block needed;
        while(i!=-1){
            if(readBlock(i).greatestKey>=iToken){
                needed= readBlock(i);
                flag=1;
                break;
            }
            i= readBlock(i).nextblock;
        }
        if(flag==1){
            auto iter=needed.Records.begin();
            for(int i=1;i<needed.Records.size();i++){
                if(iter->first==iToken){
                    return i;
                }
                iter++;
            }
        }else return -1;
    }
    int FirstEmptyBlock(char *cIndexFile){
        SequenceSetFile.close();
        SequenceSetFile.open(cIndexFile,ios::in|ios::app);
        ReadHeader();
        return FirstBlockEmpty;
    }
};
void menu(){
    cout<<"------------------------------------"<<endl;
    cout<<"1-insert key and value"<<endl;
    cout<<"2-Delete record for specific key"<<endl;
    cout<<"3-Get Key for specific Block and record"<<endl;
    cout<<"4-Get value for specific Block and record"<<endl;
    cout<<"5-Get Block index for specific key"<<endl;
    cout<<"6-Get record index for specific key"<<endl;
    cout<<"7-Get first Empty Block"<<endl;
    cout<<"8-Exit"<<endl;
    cout<<"Enter choice: "<<endl;
    cout<<"--------------------------------------"<<endl;
}
int main() {
    while(true) {
        menu();
        int choice;
        cin>>choice;
        sequenceSet obj;
        if(choice==8){
            break;
        }
        else if (choice == 1) {
            int key, val;
            cout<<"Values and keys you want to insert (insert -1 and -1 to break) "<<endl;
            while (cin >> key >> val) {
                if(key!=-1&&val!=-1){
                obj.insertVal("cIndexFile.txt", key, val);
                SequenceSetFile.close();
                }else break;
            }
        }
        else if(choice==2){
            int deleted;
            cout<<"Values and keys you want to delete (insert -1 break) "<<endl;
            while(cin>>deleted){
                if(deleted!=-1){
                bool f=obj.DeleteKey("cIndexFile.txt",deleted);
                if(f==1) cout<<deleted<<" is found and deleted "<<endl;
                else cout<<deleted<<" Cannot be found "<<endl;
                SequenceSetFile.close();
                }
                else break;
            }
        }
        else if(choice==3){
            int iBlock,iRecord;
            cout<<"Enter iBlock and irecord"<<endl;
            cin>>iBlock>>iRecord;
            cout<<obj.GetKey("cIndexFile.txt",iBlock,iRecord)<<endl;
        }
        else if(choice==4){
            int iBlock,iRecord;
            cout<<"Enter iBlock and iRecord"<<endl;
            cin>>iBlock>>iRecord;
            cout<<obj.GetVal("cIndexFile.txt",iBlock,iRecord)<<endl;
        }
        else if (choice==5){
            int key;
            cout<<"Enter the key you want to find the block index of it "<<endl;
            cin>>key;
            cout<< obj.GetBlockIndex("cIndexFile.txt",key)<<endl;

        }
        else if(choice==6){
            int key;
            cout<<"Enter the key you want to find the record index of it "<<endl;
            cin>>key;
            cout<< obj.GetRecordIndex("cIndexFile.txt",key)<<endl;
        }
        else if(choice==7){
            cout<<obj.FirstEmptyBlock("cIndexFile.txt")<<endl;
        }
        SequenceSetFile.close();
    }
}