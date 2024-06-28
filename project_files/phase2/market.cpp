#include "market.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

market::market(int argc, char** argv)
{
	
}


struct endOfDay
{
	int bought=0;
	int sold=0;
	int net_transfer=0;
};

struct stockNamepp
{
	string stockN;
	bool lin_comb;
    size_t hashVal=0;
	unordered_map<string,int> quant_list;
	bool operator==(const stockNamepp& a) const
	{
		if(this->stockN==a.stockN)
			return true;
		if(this->lin_comb==false || a.lin_comb==false)
			return false;
        int cnt=0;
		for(auto b:this->quant_list)
		{
            if(quant_list.count(b.first)>0)
            {
                cnt++;
                if(a.quant_list.at(b.first)!=b.second)
                    return false;
            }
            else if(b.second!=0)
                return false;
		}

        if(cnt<a.quant_list.size())
			return false;

		return true;		
	}
};

struct Hasher_stockname
{
    size_t operator()(const stockNamepp& key) const
    {
        return key.hashVal;
    }
};

struct order
{
	int timeStamp;
	string brokerName;
	string type;
	stockNamepp stockName;
	int price;
	int quantity;
	int expTime;	
};

class CmpBuy
{
public:
    bool operator()(order below, order above)
    {
        if (below.price < above.price)
            return true;
        else if (below.price == above.price && below.timeStamp > above.timeStamp) 
            return true;
        else if(below.price == above.price && below.timeStamp == above.timeStamp && below.brokerName>above.brokerName)
			return true;
        return false;
    }
};

class CmpSell
{
public:
    bool operator()(order below, order above)
    {
        if (below.price > above.price)
            return true;
        else if (below.price == above.price && below.timeStamp > above.timeStamp) 
            return true;
        else if(below.price == above.price && below.timeStamp == above.timeStamp && below.brokerName>above.brokerName)
			return true;
        return false;
    }
};

void executeTrade(unordered_map<string,endOfDay>& dayEnd,int& totMoneyTranfer,int& numTrades,int& numShares,string& buyer,string& seller,int& quantity,int& price,string& stockN)
{
	cout<<buyer<<" purchased "<<quantity<<" share of "<<stockN<<" from "<<seller<<" for $"<<price<<"/share"<<endl;
	totMoneyTranfer+=quantity*price;
	numTrades++;
	numShares+=quantity;
	dayEnd[buyer].bought+=quantity;
	dayEnd[buyer].net_transfer-=quantity*price;
	dayEnd[seller].sold+=quantity;
	dayEnd[seller].net_transfer+=quantity*price;
}

void endOfDayPrint(unordered_map<string,endOfDay>& dayEnd,int& totMoneyTranfer,int& numTrades,int& numShares)
{
	cout<<endl<<"---End of Day---"<<endl;
	cout<<"Total Amount of Money Transferred: $"<<totMoneyTranfer<<endl;
	cout<<"Number of Completed Trades: "<<numTrades<<endl;
	cout<<"Number of Shares Traded: "<<numShares<<endl;
	for(auto a:dayEnd)
	{
		cout<<a.first<<" bought "<<a.second.bought<<" and sold "<<a.second.sold<<" for a net transfer of $"<<a.second.net_transfer<<endl;
	}
}

void processLinCombs(stockNamepp& stockName,string& stockN)
{
	bool flag=true;
	string word1,word2;
	istringstream stockN_stream1(stockN);
    vector<pair<string,string>> sort;
    stockName.hashVal=0;
	while(stockN_stream1>>word1>>word2)
	{
		flag=false;
		stockName.quant_list[word1]=stoi(word2);
        sort.push_back({word1,word2});
        stockName.hashVal^=hash<string>()(word1)^hash<string>()(word2);
	}
	if(flag)
    {
		stockName.lin_comb=false;
        stockName.hashVal=hash<string>()(stockN);
    }
	else
    {
		stockName.lin_comb=true;
    }

}

void market::start()
{
	//start here
	unordered_map<string,endOfDay> dayEnd;
	int totMoneyTranfer=0;
	int numTrades=0;
	int numShares=0;

	vector<order> order_list;
	unordered_map<stockNamepp,priority_queue<order,vector<order>,CmpBuy>,Hasher_stockname> buy_list;
	unordered_map<stockNamepp,priority_queue<order,vector<order>,CmpSell>,Hasher_stockname> sell_list;
	string filepath="output.txt";
	fstream file(filepath);
	if(file.is_open())
	{
		string line;
		string last_line="";
		while(getline(file, line))
		{
			line=last_line+line;
			if(line=="!@")
				break;
			if(line=="TL")
				continue;
			istringstream line_stream(line);
			string word,word2,word3;
			order curr_order;
			if(!(line_stream>>word>>curr_order.brokerName>>curr_order.type))
			{
				last_line=line;
				continue;
			}
			curr_order.timeStamp=stoi(word);
			if(!getline(line_stream,word,'$'))
			{
				last_line=line;
				continue;
			}
            curr_order.stockName.stockN=word.substr(1,word.length()-2);
			//check if its a lin_comb
			processLinCombs(curr_order.stockName,curr_order.stockName.stockN);
			if(!(line_stream>>word>>word2>>word3))
			{
				last_line=line;
				continue;
			}
			curr_order.price=stoi(word);
			curr_order.quantity=stoi(word2.substr(1));
			curr_order.expTime=stoi(word3);	
			if(curr_order.expTime==-1)
				curr_order.expTime=INT16_MAX;
			else
				curr_order.expTime+=curr_order.timeStamp;
			int curr_time=curr_order.timeStamp;
			dayEnd[curr_order.brokerName];
			last_line="";


			if(curr_order.type=="BUY")
			{
				label2:
				if(!sell_list[curr_order.stockName].empty())
				{
					order best_order=sell_list[curr_order.stockName].top();
					if(curr_time>best_order.expTime)
					{
						sell_list[curr_order.stockName].pop();
						goto label2;
					}
					if(curr_order.price>=best_order.price)
					{
						sell_list[curr_order.stockName].pop();
						if(curr_order.quantity<=best_order.quantity)
						{
							best_order.quantity-=curr_order.quantity;
							if(best_order.quantity!=0)
								sell_list[curr_order.stockName].push(best_order);
							
							executeTrade(dayEnd,totMoneyTranfer,numTrades,numShares,curr_order.brokerName,best_order.brokerName,curr_order.quantity,best_order.price,curr_order.stockName.stockN);

						}
						else
						{
							curr_order.quantity-=best_order.quantity;
							
							executeTrade(dayEnd,totMoneyTranfer,numTrades,numShares,curr_order.brokerName,best_order.brokerName,best_order.quantity,best_order.price,curr_order.stockName.stockN);
							goto label2;
						}
					}
					else
						buy_list[curr_order.stockName].push(curr_order);
				}
				else
					buy_list[curr_order.stockName].push(curr_order);
			}
			else
			{
				label1:
				if(!buy_list[curr_order.stockName].empty())
				{
					order best_order=buy_list[curr_order.stockName].top();
					if(curr_time>best_order.expTime)
					{
						buy_list[curr_order.stockName].pop();
						goto label1;
					}
					if(curr_order.price<=best_order.price)
					{
						buy_list[curr_order.stockName].pop();
						if(curr_order.quantity<=best_order.quantity)
						{
							best_order.quantity-=curr_order.quantity;
							if(best_order.quantity!=0)
								buy_list[curr_order.stockName].push(best_order);
							
							executeTrade(dayEnd,totMoneyTranfer,numTrades,numShares,best_order.brokerName,curr_order.brokerName,curr_order.quantity,best_order.price,curr_order.stockName.stockN);
						}
						else
						{
							curr_order.quantity-=best_order.quantity;

							executeTrade(dayEnd,totMoneyTranfer,numTrades,numShares,best_order.brokerName,curr_order.brokerName,best_order.quantity,best_order.price,curr_order.stockName.stockN);
							goto label1;
						}
					}
					else
						sell_list[curr_order.stockName].push(curr_order);
				}
				else
					sell_list[curr_order.stockName].push(curr_order);
			}

		}
		//EndOfDayPrint
		endOfDayPrint(dayEnd,totMoneyTranfer,numTrades,numShares);

	}
	file.close();
}
