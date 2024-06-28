#include "receiver.h"
using namespace std;
#include <vector>
#include <string>
#include <unordered_map>

class stocks
{
    public:
        string stock_name;
        int value_estimate;
        int buy_price=-1;
        int sell_price=INT32_MAX;
};

void part1()
{
    Receiver rcv;
    bool foundDollar=false;
    string last_msg="";
    //sleep(5);
    string message;
    while(!foundDollar)
    {
        message=message+rcv.readIML();
        if(message[message.length()-1]!='$')
            continue;
        last_msg="";
        vector<stocks> stock_list;
        string order;
        istringstream message_stream(message);
        while(getline(message_stream,order,'#'))
        {
            if(order=="$")
            {
                foundDollar=true;
                break;
            }
            if(order[order.length()-1]!='s' && order[order.length()-1]!='b')
            {
                last_msg=order;
                break;
            }
            istringstream order_stream(order);
            string order_name;
            order_stream>>order_name;
            int price;
            order_stream>>price;
            char type;
            order_stream>>type;   


            //searching in stock_list
            bool flag=false;
            bool flag2=false;
            for(stocks &s:stock_list)
            {
                
                if(order_name==s.stock_name)
                {
                    if(type=='b')
                    {
                        if(s.buy_price<price)
                        {
                            s.buy_price=price;
                        }
                        else
                        {
                            flag2=1;
                            flag=0;
                            break;
                        }
                        if(s.sell_price==price)
                        {
                            s.sell_price=INT32_MAX;
                            s.buy_price=-1;//-----
                            flag2=1;
                            flag=0;
                            break;
                        }
                    }
                    else
                    {
                        if(s.sell_price>price)
                        {
                            s.sell_price=price;
                        }
                        else
                        {
                            flag2=1;
                            flag=0;
                            break;
                        }
                        if(s.buy_price==price)
                        {
                            s.buy_price=-1;
                            s.sell_price=INT32_MAX;//------
                            flag2=1;
                            flag=0;
                            break;
                        }
                    }
                    flag2=true;
                    if(s.value_estimate>price && type=='s')
                    {
                        cout<<order_name<<" "<<price<<" "<<'b'<<endl;
                        s.value_estimate=price;
                        s.sell_price=INT16_MAX;
                        flag=true;
                        break;
                    }
                    if(s.value_estimate<price && type=='b')
                    {
                        cout<<order_name<<" "<<price<<" "<<'s'<<endl;
                        s.value_estimate=price;
                        s.buy_price=-1;
                        flag=true;
                        break;
                    }
                }
            }
            if(!flag && !flag2)
            {
                stocks s;
                s.stock_name=order_name;
                s.value_estimate=price;
                stock_list.push_back(s);
                char a;
                if(type=='s')
                    a='b';
                else
                    a='s';
                cout<<order_name<<" "<<price<<" "<<a<<endl;
            }
            else if(flag2 && !flag)
                cout<<"No Trade"<<endl;
        }
    }
}

//part1 end

//part2 start

class lin_com_order2
{
    public:
        bool alive=true;
        vector<pair<string,int>> quantity;
        int price;
        char type;
        int index;
};

struct  max_profit_set_2
{
    int profit=-1;
    vector<lin_com_order2> subset;
};

bool isOrderEqual2(lin_com_order2& a,lin_com_order2& b)
{
    //check only quantity vector
    if(a.quantity.size()!=b.quantity.size())
        return false;

    unordered_map<string,int> stock_size;
    for(auto ab:a.quantity)
        stock_size[ab.first]=ab.second;
    for(auto ac:b.quantity)
    {
        if(stock_size[ac.first]!=ac.second)
            return false;
    }
    return true;
}

unordered_map<string,int> stock_bal_2;
int global_profit_2=0;
int check_arbit_2(vector<lin_com_order2>& stock_list,vector<lin_com_order2>& subset,int size,max_profit_set_2& s)
{
    if(size==0)
    {
        if(subset.size()==0)
            return -1;
        
        for(auto a:stock_bal_2)
        {
            if(a.second!=0)
                return -1;
        }
        if(global_profit_2>s.profit)
        {
            s.profit=global_profit_2;
            s.subset=subset;
        }
        return global_profit_2;
    }
    int p1=check_arbit_2(stock_list,subset,size-1,s);
    int p2=-1;
    if(stock_list[size-1].alive)
    {
        subset.push_back(stock_list[size-1]);
        int coeff=1;
        if(stock_list[size-1].type=='s')
            coeff=-1;
        for(auto a:stock_list[size-1].quantity)
        {
            stock_bal_2[a.first]+=a.second*coeff;
        }
        global_profit_2+=stock_list[size-1].price*coeff;
        p2=check_arbit_2(stock_list,subset,size-1,s);
        subset.pop_back();
        for(auto a:stock_list[size-1].quantity)
        {
            stock_bal_2[a.first]-=a.second*coeff;
        }
        global_profit_2-=stock_list[size-1].price*coeff;

    }
    int profit=max(p1,p2);
    return profit;

}


void part2()
{
    Receiver rcv;
    bool foundDollar=false;
    string last_msg="";
    vector<lin_com_order2> stock_list;
    int total_profit=0;
    //sleep(5);
    while(!foundDollar)
    {
        std::string message = rcv.readIML();
        message=last_msg+message;
        last_msg="";
        string order;
        istringstream message_stream(message);
        while(getline(message_stream,order,'#'))
        {
            if(order=="$")
            {
                foundDollar=true;
                break;
            }
            if(order[order.length()-1]!='s' && order[order.length()-1]!='b')
            {
                last_msg=order;
                break;
            }
            istringstream order_stream(order);
            lin_com_order2 com_order;   
            string word1,word2;
            while(order_stream>>word1>>word2)
            {
                if(word2=="b" || word2=="s")
                {
                    com_order.price=stoi(word1);
                    com_order.type=word2[0];
                }
                else
                {
                    com_order.quantity.push_back({word1,stoi(word2)});
                }
            }
            com_order.index=stock_list.size();


            //handling cancellations
            char curr_type=com_order.type;
            bool dontPush=false;
            lin_com_order2 kill_order;
            kill_order.index=-1;
            for(auto& z:stock_list)
            {
                if(z.alive)
                {
                    if(isOrderEqual2(z,com_order))
                    {
                        if(z.type==curr_type)
                        {
                            if(curr_type=='b')
                            {
                                if(com_order.price>z.price)
                                    z.alive=false;
                                else
                                {
                                    dontPush=true;
                                    break;
                                }

                            }
                            if(curr_type=='s')
                            {
                                if(com_order.price<z.price)
                                    z.alive=false;
                                else
                                {
                                    dontPush=true;
                                    break;
                                }
                            }
                            
                        }
                        else if(com_order.price==z.price)
                        {
                            kill_order=z;
                        }
                    }
                }
            }

            if(dontPush)
            {
                cout<<"No Trade"<<endl;
                continue;
            }
            else
            {
                if(kill_order.index!=-1)
                    stock_list[kill_order.index].alive=false;
            }
            stock_list.push_back(com_order); 
            max_profit_set_2 s;
            vector<lin_com_order2> subset;
            global_profit_2=0;
            stock_bal_2.clear();
            int profit=check_arbit_2(stock_list,subset,stock_list.size(),s);
            if(profit==-1 || profit==0)
                cout<<"No Trade"<<endl;
            else
            {
                for(auto a:s.subset)
                {
                    stock_list[a.index].alive=false;
                    for(auto b:a.quantity)
                    {
                        cout<<b.first<<" "<<b.second<<" ";
                    }
                    if(a.type=='b')
                        cout<<a.price<<" "<<'s'<<endl;
                    else
                        cout<<a.price<<" "<<'b'<<endl;
                }
                total_profit+=profit;
            }
        }
    }
    cout<<total_profit<<endl;
}

//part2 ends

//part3 starts

bool isNumber(string str) {
  for (char c : str) 
  {
    if (!isdigit(c) && c!='-')
      return false;
  }
  return true;
}


class lin_com_order
{
    public:
        bool alive=true;
        vector<pair<string,int>> quantity;
        int price;
        char type;
        int index;
        int totalQuantity;
};

struct  max_profit_set
{
    int profit=-1;
    vector<lin_com_order> subset;
};

bool isOrderEqual(lin_com_order& a,lin_com_order& b)
{
    //check only quantity vector and price
    if(a.price!=b.price)
        return false;

    if(a.quantity.size()!=b.quantity.size())
        return false;

    unordered_map<string,int> stock_size;
    for(auto ab:a.quantity)
        stock_size[ab.first]=ab.second;
    for(auto ac:b.quantity)
    {
        if(stock_size[ac.first]!=ac.second)
            return false;
    }
    return true;
}


unordered_map<string,int> stock_bal;
int global_profit=0;

int check_arbit(vector<lin_com_order>& stock_list,vector<lin_com_order>& subset,int size,max_profit_set& s)
{
    if(size==0)
    {
        if(subset.size()==0)
            return -1;

        for(auto a:stock_bal)
        {
            if(a.second!=0)
                return -1;
        }
        if(global_profit>s.profit)
        {
            s.profit=global_profit;
            s.subset=subset;
        }
        return global_profit;
    }

    int p1=check_arbit(stock_list,subset,size-1,s);
    int p2=-1;
    if(stock_list[size-1].alive)
    {
        lin_com_order curr=stock_list[size-1];
        for(int i=1;i<=stock_list[size-1].totalQuantity;i++)
        {
            curr.totalQuantity=i;
            subset.push_back(curr);

            //update profit and stock bal
            int coeff=1;
            if(curr.type=='s')
                coeff=-1;
            for(auto a:curr.quantity)
            {
                stock_bal[a.first]+=a.second*coeff*curr.totalQuantity;
            }
            global_profit+=curr.price*coeff*curr.totalQuantity;
            int p3=check_arbit(stock_list,subset,size-1,s);
            subset.pop_back();

            //retract stock bal and profit
            for(auto a:curr.quantity)
            {
                stock_bal[a.first]-=a.second*coeff*curr.totalQuantity;
            }
            global_profit-=curr.price*coeff*curr.totalQuantity;
            p2=max(p2,p3);
        }
        
    }
    int profit=max(p1,p2);
    return profit;

}

void part3()
{
    Receiver rcv;
    bool foundDollar=false;
    string last_msg="";
    vector<lin_com_order> stock_list;
    int total_profit=0;
    //sleep(5);
    while(!foundDollar)
    {
        std::string message = rcv.readIML();
        message=last_msg+message;
        last_msg="";
        string order;
        istringstream message_stream(message);
        while(getline(message_stream,order,'#'))
        {
            if(order=="$")
            {
                foundDollar=true;
                break;
            }
            if(order[order.length()-1]!='s' && order[order.length()-1]!='b')
            {
                last_msg=order;
                break;
            }
            istringstream order_stream(order);
            lin_com_order com_order;   
            string word1,word2;
            while(order_stream>>word1>>word2)
            {
                if(isNumber(word1))
                {
                    com_order.price=stoi(word1);
                    com_order.totalQuantity=stoi(word2);
                    order_stream>>word1;
                    com_order.type=word1[0];
                }
                else
                {
                    com_order.quantity.push_back({word1,stoi(word2)});
                }
            }
            com_order.index=stock_list.size();

            //handling cancellations
            bool dontPush=false;
            for(auto& z:stock_list)
            {
                if(z.alive)
                {
                    if(isOrderEqual(z,com_order))
                    {
                        if(com_order.type!=z.type)
                        {
                            if(com_order.totalQuantity>z.totalQuantity)
                            {
                                com_order.totalQuantity-=z.totalQuantity;
                                z.alive=false;
                            }
                            else if(com_order.totalQuantity<z.totalQuantity)
                            {
                                com_order.totalQuantity-=z.totalQuantity;
                                z.alive=false;
                            }
                            else
                            {
                                z.alive=false;
                                dontPush=true;
                            }
                        }
                        else
                        {
                            com_order.totalQuantity+=z.totalQuantity;
                            z.alive=false;
                        }
                        break;
                    }
                }
            }

            if(!dontPush)
                stock_list.push_back(com_order); 
            max_profit_set s;
            vector<lin_com_order> subset;
            global_profit=0;
            stock_bal.clear();
            int profit=check_arbit(stock_list,subset,stock_list.size(),s);
            if(profit==-1 || profit==0)
                cout<<"No Trade"<<endl;
            else
            {
                for(auto a:s.subset)
                {
                    stock_list[a.index].totalQuantity-=a.totalQuantity;
                    if(stock_list[a.index].totalQuantity==0)
                        stock_list[a.index].alive=false;
                    for(auto b:a.quantity)
                    {
                        cout<<b.first<<" "<<b.second<<" ";
                    }
                    if(a.type=='b')
                        cout<<a.price<<" "<<a.totalQuantity<<" "<<'s'<<endl;
                    else
                        cout<<a.price<<" "<<a.totalQuantity<<" "<<'b'<<endl;
                }
                total_profit+=profit;
            }
        }
    }
    cout<<total_profit<<endl;
}

int main(int argc, char* argv[]) 
{
    if(argv[1][0] == '1')
    { 
        //part1
        part1();
    }
    if(argv[1][0] == '2')
    {
        //part2
        part2();
    }

    if(argv[1][0] == '3')
    {
        //part3  
        part3();     
    }
    return 0;
}