#include<hangul.h>
#include<opencv2/opencv.hpp>
using namespace std;

int main() {
// 지정된 위치의 한자 사전 파일을 로딩한다.
// 아래 코드에서는 libhangul의 한자 사전 파일을 로딩하기 위해서
// NULL을 argument로 준다.
HanjaTable* table = hanja_table_load(NULL);
// "삼국사기"에 해당하는 한자를 찾는다.
HanjaList* list = hanja_table_match_exact(table, "삼");
if (list != NULL) {
    int i;
    int n = hanja_list_get_size(list);
    for (i = 0; i < n; ++i) {
        const char* hanja = hanja_list_get_nth_value(list, i);
        printf("한자: %s\n", hanja);
    }
    hanja_list_delete(list);
}
hanja_table_delete(table);
}

