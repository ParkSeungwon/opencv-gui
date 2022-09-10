모든 위젯 클래스는 cv::Mat(3채널)를 가진다.

윈도우 위젯은 widgets_라는 자식 위젯들의 벡터를 가진다.

operator+ : 벡터에 추가

operator<< : mat_에 위젯rect 카피(zIndex, 겹치는 부분 고려 하여 )

operator>> : mat_에서 위젯 rect 제거(zIndex, 겹치는 부분 고려하여)

start: 

- sort by zIndex
- mat_에 자식 위젯 카피
- imshow call

loop: waitKey call

