// This is your trader. Place your orders from here


#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <queue>
using namespace std;


extern std::atomic<int> commonTimer;
extern std::mutex printMutex;

struct stocknamepp_2
{
	string stockN;
	bool lin_comb;
    size_t hashVal=0;
	unordered_map<string,int> quant_list;
	bool operator==(const stocknamepp_2& a) const
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
    size_t operator()(const stocknamepp_2& key) const
    {
        return key.hashVal;
    }
};

struct order_2
{
	int timeStamp;
	string brokerName;
	string type;
	stocknamepp_2 stockName;
	int price;
	int quantity;
	int expTime;
    int index;	
    bool alive=true;
    int lastTradedAt=-1;

};


class CmpBuy_2
{
public:
    bool operator()(order_2 below, order_2 above)
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

class CmpSell_2
{
public:
    bool operator()(order_2 below, order_2 above)
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


void processLinCombs2(stocknamepp_2& stockName,string& stockN)
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

class MedianFinder {
public:
    void addNum(int num,int size) 
    {
        // Add the number to one of the heaps
        if((maxHeap.empty() || num <= maxHeap.top().first) && maxHeap.size()<10)
        {
            maxHeapSize+=size;
            maxHeap.push({num,size});
        }
        else if(minHeap.size()<10)
        {
            minHeapSize+=size;
            minHeap.push({num,size});
        }

        // Balance the heaps
        if(maxHeapSize >= minHeapSize+2*maxHeap.top().second)
        {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
        } 
        else if(minHeapSize > maxHeapSize) 
        {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    int findMedian()
    {
        if(maxHeap.size()==0)
            return -1;
        return (maxHeap.top().first);
    }

private:
    int maxHeapSize=0;
    int minHeapSize=0;
    std::priority_queue<pair<int,int>, std::vector<pair<int,int>>, std::less<pair<int,int>>> maxHeap;    // Max-heap for the smaller half
    std::priority_queue<pair<int,int>, std::vector<pair<int,int>>, std::greater<pair<int,int>>> minHeap; // Min-heap for the larger half
};

int global_lastOrderDoneNo;
vector<order_2> order_list_2;
unordered_map<stocknamepp_2,priority_queue<order_2,vector<order_2>,CmpBuy_2>,Hasher_stockname> buy_list_2;
unordered_map<stocknamepp_2,priority_queue<order_2,vector<order_2>,CmpSell_2>,Hasher_stockname> sell_list_2;
unordered_map<stocknamepp_2,MedianFinder,Hasher_stockname> median_list;




unordered_map<string,int> stock_bal;
int global_profit=0;

int check_arbit(vector<order_2>& stock_list,vector<order_2>& subset,int size)
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
        return global_profit;
    }

    int p1=check_arbit(stock_list,subset,size-1);
    int p2=-1;
    if(stock_list[size-1].alive)
    {
        order_2 curr=stock_list[size-1];
        for(int i=1;i<=stock_list[size-1].quantity;i++)
        {
            curr.quantity=i;
            subset.push_back(curr);

            //update profit and stock bal
            int coeff=1;
            if(curr.type=="SELL")
                coeff=-1;
            for(auto a:curr.stockName.quant_list)
            {
                stock_bal[a.first]+=a.second*coeff*curr.quantity;
            }
            global_profit+=curr.price*coeff*curr.quantity;
            int p3=check_arbit(stock_list,subset,size-1);
            subset.pop_back();

            //retract stock bal and profit
            for(auto a:curr.stockName.quant_list)
            {
                stock_bal[a.first]-=a.second*coeff*curr.quantity;
            }
            global_profit-=curr.price*coeff*curr.quantity;
            p2=max(p2,p3);
        }
        
    }
    int profit=max(p1,p2);
    return profit;

}






int reader(int time)
{
    //std::cout << time << " 22B3909_22B2217 SELL AMD $1 #32 1" << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(200));

    //process output.txt here
    
    //median storing
    
    string lastBrokerName="";
	string filepath="output.txt";
	fstream file(filepath);
	if(file.is_open())
	{
        bool DidItGoIn=false;
		string line;
        int lineNo=0;
        string last_line="";
		while(getline(file, line))
		{
            DidItGoIn=true;
            line=line+last_line;
            lineNo++;
            if(lineNo<=global_lastOrderDoneNo)
                continue;
            //cerr<<line<<" <------------------"<<endl;
			if(line=="!@")
				return 1;
			if(line=="TL")
				continue;
			istringstream line_stream(line);
			string word,word2,word3;
			order_2 curr_order;
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
			processLinCombs2(curr_order.stockName,curr_order.stockName.stockN);
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
            curr_order.index=order_list_2.size();
            last_line="";
            lastBrokerName=curr_order.brokerName;


			if(curr_order.type=="BUY")
			{
				label2:
				if(!sell_list_2[curr_order.stockName].empty())
				{
					order_2 best_order=sell_list_2[curr_order.stockName].top();
					if(curr_time>best_order.expTime)
					{
						sell_list_2[curr_order.stockName].pop();
						goto label2;
					}
					if(curr_order.price>=best_order.price)
					{
						sell_list_2[curr_order.stockName].pop();
						if(curr_order.quantity<=best_order.quantity)
						{
							best_order.quantity-=curr_order.quantity;
                            order_list_2[best_order.index].quantity-=curr_order.quantity;
							if(best_order.quantity!=0)
                            {
								sell_list_2[curr_order.stockName].push(best_order);
                                
                            }
                            else
                                order_list_2[best_order.index].alive=false;

							
                            
                            median_list[curr_order.stockName].addNum(best_order.price,curr_order.quantity);

						}
						else
						{
							curr_order.quantity-=best_order.quantity;
							best_order.alive=false;
                            median_list[curr_order.stockName].addNum(best_order.price,best_order.quantity);
							goto label2;
						}
					}
					else
                    {
						buy_list_2[curr_order.stockName].push(curr_order);
                        order_list_2.push_back(curr_order);
                    }
				}
				else
                {
					buy_list_2[curr_order.stockName].push(curr_order);
                    order_list_2.push_back(curr_order);
                }
			}
			else
			{
				label1:
				if(!buy_list_2[curr_order.stockName].empty())
				{
					order_2 best_order=buy_list_2[curr_order.stockName].top();
					if(curr_time>best_order.expTime)
					{
						buy_list_2[curr_order.stockName].pop();
						goto label1;
					}
					if(curr_order.price<=best_order.price)
					{
						buy_list_2[curr_order.stockName].pop();
						if(curr_order.quantity<=best_order.quantity)
						{
							best_order.quantity-=curr_order.quantity;
                            order_list_2[best_order.index].quantity-=curr_order.quantity;
							if(best_order.quantity!=0)
                            {
								buy_list_2[curr_order.stockName].push(best_order);
                            }
                            else
                                order_list_2[best_order.index].alive=false;
							
                            median_list[curr_order.stockName].addNum(best_order.price,curr_order.quantity);
						}
						else
						{
							curr_order.quantity-=best_order.quantity;
                            best_order.alive=false;
                            median_list[curr_order.stockName].addNum(best_order.price,curr_order.quantity);
							goto label1;
						}
					}
					else
                    {
						sell_list_2[curr_order.stockName].push(curr_order);
                        order_list_2.push_back(curr_order);
                    }
				}
				else
                {
					sell_list_2[curr_order.stockName].push(curr_order);
                    order_list_2.push_back(curr_order);
                }
			}

            //1.check for recent most median sell/buy deceosion
            
		}
        global_lastOrderDoneNo=lineNo;
        bool chck=true;
        if(DidItGoIn && lastBrokerName!="22B3909_22B2217")
        {
            for(int i=order_list_2.size()-1;i>=0;i--)
            {
                {
                    time=commonTimer.load();
                }
                if(order_list_2[i].lastTradedAt==time)
                    continue;
                if(order_list_2[i].timeStamp<time-1)
                    break;
                if(order_list_2[i].brokerName=="22B3909_22B2217")
                    continue;
                if(order_list_2[i].expTime<time)
                    continue;
                if(!order_list_2[i].alive)
                    continue;
                if(order_list_2[i].type=="SELL" && (median_list[order_list_2[i].stockName].findMedian()!=-1 && median_list[order_list_2[i].stockName].findMedian()>order_list_2[i].price))
                {
                    std::lock_guard<std::mutex> lock(printMutex);
                    std::cout << time << " 22B3909_22B2217 BUY "<<order_list_2[i].stockName.stockN<<" $"<<order_list_2[i].price<<" #"<<order_list_2[i].quantity<<" "<<0 << std::endl;
                    chck=false;
                    order_list_2[i].lastTradedAt=time;
                    
                }
                else if(order_list_2[i].type=="BUY" && (median_list[order_list_2[i].stockName].findMedian()!=-1 && median_list[order_list_2[i].stockName].findMedian()<order_list_2[i].price))
                {
                    std::lock_guard<std::mutex> lock(printMutex);
                    std::cout << time << " 22B3909_22B2217 SELL "<<order_list_2[i].stockName.stockN<<" $"<<order_list_2[i].price<<" #"<<order_list_2[i].quantity<<" "<<0 << std::endl;
                    order_list_2[i].lastTradedAt=time;
                    chck=false;
                    
                }


                
            }
        }
        else
            chck=false;

        //computing arbitrages
        //commenting out arbitage part increases profit


        int depth=4;
        
        if(chck)
        {
            global_profit=0;
            stock_bal.clear();
            vector<order_2> subset;
            int reach=4;
            if(order_list_2.size()<depth)
                reach=order_list_2.size();
            int profit=check_arbit(order_list_2,subset,reach);
            {
                time=commonTimer.load();
            }
            if(profit==-1 || profit==0){}
            else
            {
                for(auto a:subset)
                {
                    if(a.type=="BUY")
                    {
                        std::lock_guard<std::mutex> lock(printMutex);
                        std::cout << time << " 22B3909_22B2217 SELL "<<a.stockName.stockN<<" $"<<a.price<<" #"<<a.quantity<<" "<<0 << std::endl;
                        order_list_2[a.index].lastTradedAt=time;
                    }
                    else
                    {
                        std::lock_guard<std::mutex> lock(printMutex);
                        std::cout << time << " 22B3909_22B2217 BUY "<<a.stockName.stockN<<" $"<<a.price<<" #"<<a.quantity<<" "<<0 << std::endl;
                        order_list_2[a.index].lastTradedAt=time;
                    }
                }
            }
        }


        //arbitrage part end



	}
	file.close();


    return 0;
}

int trader(std::string *message)
{
    return 1;
}

void* userThread(void* arg)
{
    int thread_id = *(int*)arg;
    while(true)
    {
        int currentTime;
        {
            currentTime = commonTimer.load();
        }
        int end = reader(currentTime);
        if (end) break;
    }
    return nullptr;
}

void* userTrader(void* arg)
{
    return nullptr;
}