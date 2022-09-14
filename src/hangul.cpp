#include<hangul.h>
#include<opencv2/opencv.hpp>
using namespace std;

string GetUnicodeChar(unsigned int code);
int main() {
	auto mat = cv::imread("len.jpg");
	cv::imshow("title", mat);
	//ucschar dest[100];
	HangulInputContext* hic = hangul_ic_new("2");
	string s;
	while(int key = cv::waitKey()) { // 한영전환 234, 한자 228, back 8
		//cout << key << endl;
		if(key == -1) break;
		hangul_ic_process(hic, key);
		const ucschar *commit = hangul_ic_get_commit_string(hic);
		const ucschar *preedit = hangul_ic_get_preedit_string(hic);
		if(*commit != 0) s += GetUnicodeChar(*commit);
		cout << (s + GetUnicodeChar(*preedit)) << endl;
		//while(commit[0] != 0) continue;
		//hangul_jamos_to_syllables(dest, 100, commit, hangul_syllable_len(commit, 100));
		//cout << "commit: " << GetUnicodeChar(*commit) << endl;
	}
	hangul_ic_delete(hic);
}

