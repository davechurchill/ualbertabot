#pragma once
#include <chrono>
#include <string>

namespace UAlbertaBot{
	class HLStatistics
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock>  startTime;
		int nodeCount;
		//int TTqueries;
		//int TTfound;
		//int TTusefull;
		//int TTcutoff;
		int branches;
		int searches;
		//int hashCol;
		int forwardLength;
		int numForwards;
	public:
		HLStatistics();
		void clear(){
			nodeCount = 0;
			//TTqueries = 0;
			//TTfound = 0;
			//TTusefull = 0;
			//TTcutoff = 0;
			branches = 0;
			searches = 0;
			//hashCol = 0;
		}

		void startTimer(){
			startTime = std::chrono::high_resolution_clock::now();
		}
		long long getRunningTimeSecs() const{
			return  std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		}
		long long getRunningTimeMilliSecs() const{
			return  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		}
		//void incHashCollisions(){
		//	hashCol++;
		//}
		//int getHashCollisions(){
		//	return hashCol;
		//}
		void incNodeCount(){
			nodeCount++;
		}
		int getNodeCount() const{
			return nodeCount;
		}
		double getNodesSec() const{
			return ((float)nodeCount) / getRunningTimeMilliSecs()*1000.0;
		}
		//void incTTquery(){
		//	TTqueries++;
		//}
		//int getTTqueries() const{
		//	return TTqueries;
		//}
		//void incTTfound(){
		//	TTfound++;
		//}
		//int getTTfound() const{
		//	return TTfound;
		//}
		//void incTTusefull(){
		//	TTusefull++;
		//}
		//int getTTusefull() const{
		//	return TTusefull;
		//}
		//void incTTcutoff(){
		//	TTcutoff++;
		//}
		//int getTTcutoff() const{
		//	return TTcutoff;
		//}

		void addFwd(int length){
			forwardLength += length;
			numForwards++;
		}
		void addBF(int nodesSearched){
			searches++;
			branches += nodesSearched;
		}
		float getAvgBf() const{

			return searches>0?((float)branches) / searches:-1.0f;
		}
		float getAvgFwd() const{

			return numForwards>0 ? ((float)forwardLength) / numForwards : -1.0f;
		}
		std::string toString() const;
		std::string header() const;

		~HLStatistics();
	};
}
