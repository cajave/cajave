// Banker.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <algorithm>

// 避免死锁银行家算法
using namespace std;
// 银行家算法数据结构
class BankerAlgorithm {
	using uint = unsigned int;
	using eletype = vector<int>::value_type;
public:
	BankerAlgorithm() : BankerAlgorithm(8, 10) {}
	BankerAlgorithm(uint proc_num, uint res_num)
		: m_proc_num(proc_num), m_res_num(res_num) {}
	virtual ~BankerAlgorithm() {}
	BankerAlgorithm& initialize() {
		initialize_prevs();
		return *this;
	}
	BankerAlgorithm& set_cur_proc_req(int proc_idx, const vector<int>& request) {
		m_cur_proc_idx = proc_idx;
		m_cur_req_poi = &request;
		return *this;
	}
	operator bool() {
		if (!m_cur_req_poi) {
			return false;
		}
		struct pointer {
			const vector<int>*& req;
			pointer(const vector<int>*& q) : req(q){}
			~pointer() { req = nullptr; }
		} h(m_cur_req_poi);
		if (m_cur_proc_idx >= m_proc_num
			|| !check_precondition()) {
			return false;
		}
		try_alloc();
		return check_safety();
	}
protected:
	virtual void initialize_prevs() {
		int available[3]{ 14, 20, 15 };
		int max[4][3]{ {5, 9, 8}, {8, 8, 4}, {6, 9, 8}, {5, 8, 9} };
	}
	void initialize_working_state() {
		m_work.resize(m_res_num);
		memcpy((char*)&m_work[0], (char*)&m_available[0],
			m_available.size() * sizeof(eletype));
		m_finish.resize(m_proc_num);
		//for (auto& i : m_finish) {
		//	i = false;
		//}
		//for_each(m_finish.begin(), m_finish.end(), [](bool& b) {b = false; });
	}
	bool check_precondition() const {
		size_t sz = min(m_available.size(), m_cur_req_poi->size());
		sz = min(sz, m_need.size());
		for (int i = 0; i < sz; i++) {
			if (m_available[i] < (*m_cur_req_poi)[i]
				|| m_need[m_cur_proc_idx][i] < (*m_cur_req_poi)[i]) {
				return false;
			}
		}
		return true;
	}
	void try_alloc() {
		transform(m_available.begin(), m_available.end(),
			m_cur_req_poi->begin(), m_available.begin(),
			[](eletype& i, eletype j) {return i -= j; });
		transform(m_allocation[m_cur_proc_idx].begin(),
			m_allocation[m_cur_proc_idx].end(),
			m_cur_req_poi->begin(),
			m_allocation[m_cur_proc_idx].begin(),
			[](eletype& i, eletype j) {return i += j; });
		transform(m_need[m_cur_proc_idx].begin(),
			m_need[m_cur_proc_idx].end(),
			m_cur_req_poi->begin(),
			m_need[m_cur_proc_idx].begin(),
			[](eletype i, eletype j) {return i - j; });
	}

	template<class LITER, class RITER>
	bool compare(LITER lbeg, LITER lend, RITER rbeg) {
		while (*lbeg <= *rbeg && lbeg != lend) (++lbeg, ++rbeg);
		return lbeg == lend;
	}

	bool check_safety() {
		initialize_working_state();
		for (uint i = 0; i < m_proc_num; i++) {
			if (m_finish[i]) {
				continue;
			}
			if (compare(m_work.begin(), m_work.end(), m_need[i].begin())) {
				transform(m_work.begin(), m_work.end(), m_allocation[i].begin(), 
					m_work.begin(), [](eletype& i, eletype j) {return i += j; });
				m_finish[i] = true;
			}
		}
		return true;
	}

	uint m_proc_num{ 4 };
	uint m_res_num{ 3 };
	uint m_cur_proc_idx{ 0 };
	const vector<int>* m_cur_req_poi{ nullptr };
	vector<int> m_available{ move(vector<int>(m_res_num)) };
	vector<vector<int>> m_max{ m_proc_num, vector<int>(m_res_num) };
	vector<vector<int>> m_allocation{ m_proc_num, vector<int>(m_res_num) };
	vector<vector<int>> m_need{ m_proc_num, vector<int>(m_res_num) };
	vector<int> m_work{ move(vector<int>(m_res_num)) };
	vector<bool> m_finish{ move(vector<bool>(m_proc_num)) };
};




int main()
{
	BankerAlgorithm bk;
	if (bk.initialize().set_cur_proc_req(3, {9, 0, 8})) {
	}

    std::cout << "Hello World!\n"; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
