#include <stdio.h>
#include <stdlib.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>

using namespace std;
using namespace boost;

class hoge {
public:
	hoge(boost::mutex* lock){count_ = 0; flag_stop_ = false; lock_ = lock;}
	void run(){
		thread_ = boost::make_shared<boost::thread>(boost::bind(&hoge::execute, this));
	}
	void execute(){
		while(!flag_stop_){
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
			boost::mutex::scoped_lock lock(*lock_);
			++count_;
		}
	}
	int get_count(){
		boost::mutex::scoped_lock lock(*lock_);
		return count_;
	}

	int count_;
	bool flag_stop_;
	boost::mutex* lock_;
	boost::shared_ptr<boost::thread> thread_;
};

int main(int argc, char** argv)
{
	if(argc < 2) return 1;
	int num_thread = atoi(*(argv+1));
	boost::mutex lock;
	vector<boost::shared_ptr<hoge> > hoges;
	for(int i=0;i<num_thread;++i){
		boost::shared_ptr<hoge> h = boost::make_shared<hoge>(&lock);
		hoges.push_back(h);
		h->run();
	}

	while(true){
		int count = 0;
		for(auto ite = hoges.begin();ite != hoges.end();++ite){
			count += (*ite)->get_count();
		}
		printf("%d\n", count);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	}

	for(auto ite = hoges.begin();ite != hoges.end();++ite){
		(*ite)->flag_stop_ = true;
	}
	for(auto ite = hoges.begin();ite != hoges.end();++ite){
		(*ite)->thread_->join();
	}
	return 0;
}
